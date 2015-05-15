#ifndef __PRODUCER_H__
#define __PRODUCER_H__

#include "../Items/WorkItem.h"
#include "../Queue/Queue.h"
#include "../Schedule/ScheduledObject.h"

class Producer : public ScheduledObject {
private:
	Queue<WorkItem> *m_workq;
	unsigned int	m_id;
	unsigned int m_next_item_id;
	WorkItem	m_saved_work_item;
	bool		m_valid_saved_work_item;

	void cook_item(WorkItem &item);

public:
	Producer(Queue<WorkItem> *workQ, unsigned int id = 0, int thread_priority = 0, 
			 int policy = SCHED_OTHER, int which_core = 0xFF);
	~Producer();

protected:
	void work();
};

#endif
