#ifndef __SCHEDULED_OBJECT_H__
#define __SCHEDULED_OBJECT_H__ 

#include <pthread.h>

class ScheduledObject {
private:
	pthread_t	m_thread;
	int	m_thread_policy;
	int	m_thread_priority;
	int	m_which_core;
	bool	m_run;

	static void* run(void* arg);
	void set_scheduler();

public:
	ScheduledObject(int thread_priority = 0, int policy = SCHED_OTHER, int which_core = 0xFF);
	~ScheduledObject();

protected:
	virtual void work();
};

#endif
