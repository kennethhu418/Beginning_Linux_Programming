#include "Consumer.h"
#include <sched.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <time.h>

Consumer::Consumer(Queue<WorkItem> * workQ, unsigned int id, int thread_priority,
				   int policy, int which_core) : ScheduledObject(thread_priority, policy, which_core)  {
	m_workq = workQ;
	m_id = id;
}

Consumer::~Consumer() {

}

void Consumer::consume_item(WorkItem &item) {
#if 0
	std::cout << "Consumer " << m_id << " consumed following item:"<<std::endl;
	std::cout << "\t\tid: " << item.id << std::endl;
	std::cout << "\t\tdata: " << item.data << std::endl;
#endif
}

void Consumer::work() {
	WorkItem item;
	if(m_workq->pop(item))
		consume_item(item);
}
