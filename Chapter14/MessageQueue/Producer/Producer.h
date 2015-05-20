#ifndef __PRODUCER_H__
#define __PRODUCER_H__

#include "../Items/WorkItem.h"
#include "../Schedule/ScheduledObject.h"
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

class Producer : public ScheduledObject {
private:
	unsigned int	m_id;
	unsigned int m_next_item_id;
	unsigned int m_produced_count;
	mqd_t	m_mqd;

	void cook_item(WorkItem &item);

public:
	Producer(const char* msg_q_name, unsigned int id = 0, int thread_priority = 0, 
			 int policy = SCHED_OTHER, int which_core = 0xFF);
	~Producer();

protected:
	void work();
};

#endif
