#include "./ScheduledObject.h"
#include <sched.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <assert.h>


ScheduledObject::ScheduledObject(int thread_priority, int policy, int which_core) {
	m_run = true;
	m_thread_policy = policy;
	m_thread_priority = thread_priority;
	m_which_core = which_core;
	assert(0 ==  pthread_create(&m_thread, NULL, run, (void*)this));
}

ScheduledObject::~ScheduledObject() {
	m_run = false;
	int *temp_ret = NULL;
	pthread_join(m_thread, (void**)&temp_ret);
}


void ScheduledObject::set_scheduler() {
	int min_priority = sched_get_priority_min(m_thread_policy);
	int max_priority = sched_get_priority_max(m_thread_policy);
	int thread_priority = m_thread_priority;
	if(thread_priority > max_priority)
		thread_priority = max_priority;
	if(thread_priority < min_priority)
		thread_priority = min_priority;
	struct sched_param sched;
	sched.sched_priority = thread_priority;

	// Set Priority of the thread
	if(m_thread_policy == SCHED_OTHER) {
		pthread_setschedparam(pthread_self(), SCHED_OTHER, &sched);
	}
	else if(getuid() != 0) {
		std::cout << "Sorry, non root user. Cannot change sheduler setting." << std::endl;
	}
	else {
		sched.sched_priority = thread_priority;
		pthread_setschedparam(pthread_self(), m_thread_policy, &sched);
	}

	// Set CPU Affinity
	if(m_which_core != 0xFF) {
		cpu_set_t	cpu_mask;
		CPU_ZERO(&cpu_mask);
		CPU_SET(m_which_core, &cpu_mask);
		pthread_setaffinity_np(pthread_self(), sizeof(cpu_mask), &cpu_mask);
	}
}


void* ScheduledObject::run(void* arg) {
	ScheduledObject *obj = (ScheduledObject*)arg;

	obj->set_scheduler();

	sleep(2);

	while(obj->m_run) {
		obj->work();
	}

	pthread_exit(NULL);
	return NULL;
}

void ScheduledObject::work() {
	std::cout << "Default work is running in ScheduleObject!" << std::endl;
}
