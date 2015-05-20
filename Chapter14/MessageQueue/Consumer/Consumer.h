#ifndef __CONSUMER_H__
#define __CONSUMER_H__

#include "../Items/WorkItem.h"
#include "../Schedule/ScheduledObject.h" 
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

class Consumer : public ScheduledObject {
private:
	unsigned int	m_id;
	unsigned int    m_consumed_count;
	mqd_t	m_mqd;

	void consume_item(WorkItem &item);

public:
	Consumer(const char* msg_q_name, unsigned int id = 0, int thread_priority = 0, 
			 int policy = SCHED_OTHER, int which_core = 0xFF);
	~Consumer();

protected:
	void work();
};

#endif
