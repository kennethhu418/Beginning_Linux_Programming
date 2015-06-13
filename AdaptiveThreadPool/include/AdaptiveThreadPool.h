#ifndef __ADAPTIVE_THREADPOOL_H__
#define __ADAPTIVE_THREADPOOL_H__

#include <semaphore.h>
#include <ctime>
#include "WorkerConf.h"


class AdaptiveThreadPool
{
private:
	string		mName;
	ThreadPoolConfig mConfig;
	JobWorker*	mJobWorkerArr[MAX_WORKER_COUNT];
	unsigned int  mJobWorkerCount;
	JobQueue*	mJobQueueArr[MAX_WORKER_COUNT];
	unsigned int  mJobQueueCount;

	// Following fields are statistics data, which is used
	// for job assign and debug purpose
	typedef struct _StatisticCtrl
	{
		sem_t	mGlobalLock_Next;
		sem_t   mCoreLock_Next_Arr[MAX_WORKER_COUNT];
		unsigned long long	mWorkerProcessedCountArr[MAX_WORKER_COUNT];
		unsigned int	mNextGlobalJobWorker;
		unsigned int	mNextJobWorkerOnEachCoreArr[MAX_WORKER_COUNT];
		unsigned int	mJobWorkerWithLeastJob;
		unsigned int	mJobWorkerWithLeastJobOnEachCoreArr[MAX_WORKER_COUNT];
	}StatisticCtrl mStatCtrl;

	// Timer thread for updating the statistics
	pthread_t		mTimerThread;
	bool			mTimerRun;

public:
	AdaptiveThreadPool(const string &name, const ThreadPoolConfig &config) {
		mName = name;
		mConfig = config;

		initializeStatistics();
		
		if(!configAndStartWorkers()) {
			std::cerr << "Workers Creation Fail!" << std::endl;
		}
	}
	
	~AdaptiveThreadPool() {
		stopWorkers();
	}

	string	getPoolName() {
		return mName;
	}

	ThreadPoolConfig getPoolConfig() {
		return mConfig;
	}

	bool	addJob(Job &addedJob, JobAssignPolicy assignPolicy = JOB_ASSIGN_SAME_CORE_ROUND_ROBIN);

	void	cancelJobs();

	void	pause();

	void	resume();

	unsigned long long getJobCount() {
		unsigned long long jobCount = 0;
		for(int i = 0; i < mJobWorkerCount; ++i) {
			jobCount += mJobWorkerArr[i]->getJobCount();
		}
		return jobCount;
	}


private:
	bool configAndStartWorkers();
	void stopWorkers();
	bool checkAndAdjustConfig();
	void createJobQueues();
	void createJobWorkers();
	void createTimer();
	void assignQueuesToWorkers();
	void initializeStatistics();

	static void* timer_thread_func(void* arg);
};


#endif
