#ifndef __CONSUMER_H__
#define __CONSUMER_H__

#include "../Items/WorkItem.h"
#include "../Queue/Queue.h"
#include "../Schedule/ScheduledObject.h" 

class Consumer : public ScheduledObject {
private:
	Queue<WorkItem> *m_workq;
	unsigned int	m_id;

	void consume_item(WorkItem &item);

public:
	Consumer(Queue<WorkItem> *workQ, unsigned int id = 0, int thread_priority = 0, 
			 int policy = SCHED_OTHER, int which_core = 0xFF);
	~Consumer();

protected:
	void work();
};

#endif
