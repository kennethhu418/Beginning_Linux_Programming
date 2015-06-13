#ifndef __WORKER_CONFIG_H__
#define __WORKER_CONFIG_H__

#include <unistd.h>
#include <pthread.h>
#include <sched.h>

#define MAX_WORKER_COUNT	128
#define WORK_QUEUE_LENGTH_DEFAULT	20
#define UPDATING_POOL_STATISTICS_TIME_DEFAULT	20 //in unit of ms

enum JobWorkerType
{
    // Each jobworker only takes care of his own queue, 
	// he does not care about other job workers.*/
	JOBWORKER_TYPE_INDEPENDENT = 0, 
	// Each jobworker takes care of his own queue if his
	// queue is not empty. Once its queue becomes empty,
	// it helps its adjacent job worker to drain the queue.
	JOBWORKER_TYPE_HELP_ADJACENT = 1, 
	JOBWORKER_TYPE_MAX = 2
};

enum JobQueueType
{
	// This job queue is only consumed by one job worker
	JOBQUEUE_TYPE_EXCLUSIVE = 0,
	// This job queue is shared among job workers
	JOBQUEUE_TYPE_SHARED = 1,
	JOBQUEUE_TYPE_MAX = 2
};

enum ThreadSchedulePolicy
{
	THREADSCHED_DEFAULT = SCHED_OTHER,
	THREADSCHED_REALTIME_WITH_SLICE = SCHED_RR,
	THREADSCHED_REALTIME_NO_SLICE = SCHED_FIFO
};

enum JobAssignPolicy
{
	// When a job is added to the thread pool
	// it will be added to the job worker on
	// the same core with the calling thread
	// If there are more than 1 job workers
	// on the core, round robin assign the job
	JOB_ASSIGN_SAME_CORE_ROUND_ROBIN = 1,
	// When a job is added to the thread pool
	// it will be added to the job worker on
	// the same core with the calling thread
	// If there are more than 1 job workers
	// on the core, assign the job to the
	// worker with the least pending jobs
	JOB_ASSIGN_SAME_CORE_LEAST_JOBS = 2,
	// When a job is added to the thread pool
	// it will be round robin added to one
	// job worker
	JOB_ASSIGN_ROUND_ROBIN = 3,
	// When a job is added to the thread pool
	// it will be added to the job worker
	// with the least pending jobs
	JOB_ASSIGN_LEAST_JOBS =  4,
	// Randomly assign a job to a job worker
	// on the same core with the calling thread
	JOB_ASSIGN_SAME_CORE_RANDOM = 5,
	// Randomly assign a job to a job worker
	JOB_ASSIGN_RANDOM = 6,

	JOB_ASSIGN_MAX = 7
};

class JobWorkerConfig
{
protected:
	JobWorkerType	mJobWorkerType;
	ThreadSchedulePolicy	mThreadPolicy;
	int	mThreadPriority;
	unsigned int mWhichCore;

public:
	JobWorkerConfig(ThreadSchedulePolicy sched_policy = THREADSCHED_DEFAULT, 
		int priority = 0, JobWorkerType workerType = JOBWORKER_TYPE_INDEPENDENT,
		unsigned in whichCore = 0) : mThreadPolicy(sched_policy), mThreadPriority(priority), 
		mJobWorkerType(workerType), mWhichCore(whichCore) {
	}

	void setSchedulePolicy(ThreadSchedulePolicy policy) {
		mThreadPolicy = policy;
	}

	ThreadSchedulePolicy getSchedulePolicy() const {
		return mThreadPolicy;
	}

	void setThreadPriority(int priority) {
		mThreadPriority = priority;
	}

	int getThreadPriority() const {
		return mThreadPriority;
	}

	void setWorkerType(JobWorkerType type) {
		mJobWorkerType = type;
	}

	JobWorkerType getWorkerType() {
		return mJobWorkerType;
	}

};

class ThreadPoolConfig : protected JobWorkerConfig
{
private:
	JobQueueType 	mJobQueueType;
	unsigned int	mWorkersPerCore;
	unsigned int	mQueueLength;
	// Every mQueueShareFactor job workers share one same queue
	unsigned int	mQueueShareFactor;
	timespec		mUpdatingStatisticsTime;

public:
	ThreadPoolConfig(unsigned int workersPerCore = 1, 
		ThreadSchedulePolicy sched_policy = THREADSCHED_DEFAULT, 
		int priority = 0, JobWorkerType workerType = JOBWORKER_TYPE_INDEPENDENT,
		JobQueueType queueType = JOBQUEUE_TYPE_EXCLUSIVE, unsigned int queueLength = WORK_QUEUE_LENGTH_DEFAULT,
		unsigned int queueShareFactor = 1) : JobWorkerConfig(sched_policy, priority, workerType),
		mWorkersPerCore(workerPerCore), mJobQueueType(queueType), mQueueLength(queueLength) {
			setStatisticsUpdatingTimeInMs(UPDATING_POOL_STATISTICS_TIME_DEFAULT);
	}

	void setWorkersPerCore(unsigned int workersPerCore) {
		mWorkersPerCore = workersPerCore;
	}

	unsigned int getWorkersPerCore() const {
		return mWorkersPerCore;
	}

	void setQueueType(JobQueueType type) {
		mJobQueueType = type;
	}

	JobQueueType getQueueType() const {
		return mJobQueueType;
	}

	void setQueueLength(unsigned int len) {
		mQueueLength = len;
	}

	unsigned int getQueueLength() const {
		return mQueueLength;
	}

	void setQueueShareFactor(unsigned int factor) {
		mQueueShareFactor = factor;
	}

	unsigned int getQueueShareFactor() const {
		return mQueueShareFactor;
	}

	void setStatisticsUpdatingTimeInMs(unsigned int time) {
		long long updateTimeNs = time * 1000000;
		mUpdatingStatisticsTime.tv_sec  = updateTimeNs / 1000000000;
		mUpdatingStatisticsTime.tv_nsec = updateTimeNs % 1000000000;
	}

	timespec getStatisticsUpdatingTimeSpec() const {
		return mUpdatingStatisticsTime;	
	}

	static unsigned int getCoreCount() {
		return sysconf(_SC_NPROCESSORS_ONLN);
	}
};
#endif
