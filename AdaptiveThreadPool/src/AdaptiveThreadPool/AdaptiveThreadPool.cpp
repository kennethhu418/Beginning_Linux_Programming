#include "AdaptiveThreadPool.h"

void	AdaptiveThreadPool::initializeStatistics() {
	unsigned int workers_per_core = mConfig.getWorkersPerCore();
	sem_init(&mStatCtrl.mGlobalLock_Next, 0, 1);
	memset(mStatCtrl.mWorkerProcessedCountArr, 0, MAX_WORKER_COUNT * sizeof(mStatCtrl.mWorkerProcessedCountArr[0]));
	mStatCtrl.mNextGlobalJobWorker = mStatCtrl.mJobWorkerWithLeastJob = 0;
	for(int i = 0; i < MAX_WORKER_COUNT; ++i) {
		// TODO: Optimize so that we allocate mJobQueueCount locks
		sem_init(&mStatCtrl.mCoreLock_Next_Arr[i], 0, 1);
		mStatCtrl.mNextJobWorkerOnEachCoreArr[i] = mStatCtrl.mJobWorkerWithLeastJobOnEachCoreArr[i] = i * workers_per_core;
	}
}

bool	AdaptiveThreadPool::configAndStartWorkers() {
	// Check the configuration and adjust it if it is not legal 
	if(!checkAndAjustConfig())
		return false;

	srand(time(NULL));

	// Create job queues
	createJobQueues();

	// Create job workers
	createJobWorkers();

	// Assign job queues to workers
	assignQueuesToWorkers();

	// Start the job workers
	for(unsigned int i = 0; i < mJobWorkerCount; ++i) {
		mJobWorkerArr[i]->start();
	}

	// Create the timer thread
	createTimer();

	return true;
}

bool AdaptiveThreadPool::checkAndAjustConfig() {
	unsigned int core_num = ThreadPoolConfig::getCoreCount();
	unsigned int workers_per_core = mConfig.getWorkersPerCore();
	mJobWorkerCount = core_num * workers_per_core;
	if(mJobWorkerCount > MAX_WORKER_COUNT) {
		std::cerr << "Too many workers created!" << std::endl;
		return false;
	}

	if(mConfig.getSchedulePolicy() != THREADSCHED_DEFAULT
	   && getuid() != 0) {
		// non root user can only set default scheduler policy
		std::cerr << "Sorry, you are not the root user. You cannot set scheduler policy other than\
			THREADSCHED_DEFAULT." << std::endl;
		return false;
	}

	int min_priority = sched_get_priority_min(mConfig.getSchedulePolicy());
	int max_priority = sched_get_priority_max(mConfig.getSchedulePolicy());
	int thread_priority = mConfig.getThreadPriority();
	if(thread_priority < min_priority) {
		thread_priority = min_priority;
	}
	else if(thread_priority > max_priority) {
		thread_priority = max_priority;
	}

	// The max priority in THREADSCHED_REALTIME_NO_SLICE is reserved
	// for the timer thread.
	if(mConfig.getSchedulePolicy() == THREADSCHED_REALTIME_NO_SLICE
	   && thread_priority == max_priority) {
		thread_priority = max_priority - 2;
		if(thread_priority < min_priority) {
			thread_priority = min_priority;
		}
	}

	mConfig.setThreadPriority(thread_priority);

	// Check Queues
	switch(mConfig.getQueueType()) {
	case JOBQUEUE_TYPE_EXCLUSIVE:
		mJobQueueCount = mJobWorkerCount;
		break;
	case JOBQUEUE_TYPE_SHARED:
		mJobQueueCount = (mJobWorkerCount + mConfig.getQueueShareFactor() - 1) / mConfig.getQueueShareFactor;
		break;
	default:
		assert(false);
	}

}

void AdaptiveThreadPool::createJobQueues() {
	JobQueueType	queueType = mConfig.getQueueType();
	for(unsigned int i = 0; i < mJobQueueCount; ++i) {
		mJobQueueArr[i] = JobQueue::createJobQueue(queueType);
	}
}

void AdaptiveThreadPool::createJobWorkers() {
	unsigned int core_num = ThreadPoolConfig::getCoreCount();
	unsigned int workers_per_core = mConfig.getWorkersPerCore();
	unsigned int worker_id = 0;

	JobWorkerConfig	workerConfig = (JobWorkerConfig)mConfig;
	for(unsigned int core = 0; core < core_num; ++core) {
		workerConfig.mWhichCore = core;
		for(unsigned int i = 0; i < workers_per_core; ++i, ++worker_id) {
			mJobWorkerArr[worker_id] = JobWorker::createJobWorker(worker_id, workerConfig);
		}
	}
}

void AdaptiveThreadPool::assignQueuesToWorkers()  {
	unsigned int nextJobWorker = 0;
	unsigned int workers_per_q = mConfig.getQueueShareFactor();
	for(unsigned int q = 0; q < mJobQueueCount; ++q) {
		for(unsigned int i = 0; i < workers_per_q; ++i) {
			mJobWorkerArr[nextJobWorker++]->assignQ(mJobQueueArr[q]);
		}
	}
}

void AdaptiveThreadPool::createTimer() {
	mTimerRun = true;
	assert(0 == pthread_create(&mTimerThread, NULL, timer_thread_func, (void*)this));
}

void* AdaptiveThreadPool::timer_thread_func(void* arg) {
	AdaptiveThreadPool* thread_pool = (AdaptiveThreadPool*)arg;
	// Adjust the scheduler policy, thread priority
	ThreadSchedulePolicy sched_policy = THREADSCHED_DEFAULT;
	int	priority = 0;
	if(0 == getuid()) { //If we are root, we can set real time priority
		sched_policy = THREADSCHED_REALTIME_NO_SLICE;
		priority = sched_get_priority_max(sched_policy);
	}
	sched_param sched;
	sched.sched_priority = priority; 
	if(pthread_set_sched_param(pthread_self(), sched_policy, &sched)) {
		std::cerr << "Error setting timer schedule policy with error: " << strerror(errno) << std::endl;
	}

	const timespec sleepTime = thread_pool->mConfig.getStatisticsUpdatingTimeSpec();
	const unsigned int core_count = ThreadPoolConfig::getCoreCount();
	const unsigned int workers_per_core = thread_pool->mConfig.getWorkersPerCore();

	// Timer thread do not need to bind to any specific core
	while(thread_pool->mTimerRun) {
		nanosleep(&sleepTime, NULL);
		
		//Update the global statistics - the worker with least job
		unsigned long long leastJobs = thread_pool->mJobWorkerArr[0]->getJobCount();
		unsigned int	leastJobWorker = 0;
		unsigned long long jobNum = 0;
		for(unsigned int i = 1; i < mJobWorkerCount; ++i) {
			jobNum = thread_pool->mJobWorkerArr[i]->getJobCount();
			if(jobNum < leastJobs) {
				leastJobs = jobNum;
				leastJobWorker = i;
			}
		}
		thread_pool->mStatCtrl.mJobWorkerWithLeastJob = leastJobWorker;

		//Update the each core statistics - the worker with least job on each core
		unsigned int startingWorkerIndex = 0;
		for(unsigned int core = 0; core < core_count; ++core) {
			leastJobs = thread_pool->mJobWorkerArr[startingWorkerIndex]->getJobCount();
			leastJobWorker = startingWorkerIndex;
			for(unsigned int i = 1; i < workersPerCore; ++i) {
				jobNum = thread_pool->mJobWorkerArr[startingWorkerIndex + i]->getJobCount();
				if(jobNum < leastJobs) {
					leastJobs = jobNum;
					leastJobWorker = startingWorkerIndex + i;
				}
			}

			thread_pool->mStatCtrl.mJobWorkerWithLeastJobOnEachCoreArr[core] = leastJobWorker;
			startingWorkerIndex += workersPerCore;
		}
	
	}
	
	pthread_exit(NULL);
	return NULL;
}

void AdaptiveThreadPool::stopWorkers() {
	//stop timer
	mTimerRun = false;
	pthread_join(mTimerThread, NULL);

	sem_destroy(&mStatCtrl.mGlobalLock_Next);
	for(unsigned int i = 0; i < MAX_WORKER_COUNT; ++i) {
		sem_destroy(&mStatCtrl.mCoreLock_Next_Arr[i]);
	}

	// Deallocate all workers
	for(unsigned int i = 0; i < mJobWorkerCount; ++i) {
		delete mJobWorkerArr[i];
	}

	// Deallocate all queues
	for(unsigned int i = 0; i < mJobQueueCount; ++i) {
		delete mJobQueueArr[i];
	}
}

bool	AdaptiveThreadPool::addJob(Job &addedJob, JobAssignPolicy assignPolicy) {
	unsigned int whichWorker = 0;

	switch(assignPolicy) {
	case JOB_ASSIGN_SAME_CORE_ROUND_ROBIN:
		unsigned int cur_core = sched_getcpu();
	    sem_wait(&mStatCtrl.mCoreLock_Next_Arr[cur_core]);
		whichWorker = mStatCtrl.mNextJobWorkerOnEachCoreArr[cur_core]++;
		if(mStatCtrl.mNextJobWorkerOnEachCoreArr[cur_core] == (cur_core + 1) * mConfig.getWorkersPerCore()) {
			mStatCtrl.mNextJobWorkerOnEachCoreArr[cur_core] = cur_core * mConfig.getWorkersPerCore();
		}
	    sem_post(&mStatCtrl.mCoreLock_Next_Arr[cur_core]);
		break;
	case JOB_ASSIGN_SAME_CORE_LEAST_JOBS:
		unsigned int cur_core = sched_getcpu();
		whichWorker = mStatCtrl.mJobWorkerWithLeastJobOnEachCoreArr[cur_core];
		break;
	case JOB_ASSIGN_ROUND_ROBIN:
		sem_wait(&mStatCtrl.mGlobalLock_Next);
		whichWorker = mStatCtrl.mNextGlobalJobWorker++;
		if(mStatCtrl.mNextGlobalJobWorker == mJobWorkerCount) {
			mStatCtrl.mNextGlobalJobWorker = 0;
		}
		sem_post(&mStatCtrl.mGlobalLock_Next);
		break;
	case JOB_ASSIGN_LEAST_JOBS:
		whichWorker = mJobWorkerWithLeastJob;
		break;
	case JOB_ASSIGN_SAME_CORE_RANDOM:
		unsigned int cur_core = sched_getcpu();
		whichWorker = cur_core * mConfig.getWorkersPerCore() + rand() % mConfig.getWorkersPerCore();
		break;
	case JOB_ASSIGN_RANDOM:
		whichWorker = rand() % mJobWorkerCount;
		break;
	default:
		assert(false);
	}

	return mJobWorkerArr[whichWorker]->addJob(job);
}
