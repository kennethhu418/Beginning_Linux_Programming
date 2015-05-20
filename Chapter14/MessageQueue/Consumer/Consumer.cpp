#include "Consumer.h"
#include <sched.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <cstring>
#include <errno.h>

Consumer::Consumer(const char* msg_q_name, unsigned int id, int thread_priority,
				   int policy, int which_core) : m_mqd((mqd_t)-1), ScheduledObject(thread_priority, policy, which_core)  {
	m_id = id;
	m_mqd = mq_open(msg_q_name, O_RDONLY);
	if(m_mqd == (mqd_t)-1) {
		std::cout << "Fail to open msg queue in Consumer " << id << std::endl;
		std::cout << "Error: " << strerror(errno) << std::endl;
	}
	m_consumed_count = 0;
}

Consumer::~Consumer() {
	mq_close(m_mqd);
	std::cout << "Consumer " << m_id << " consumes " << m_consumed_count << " items." << std::endl;
}

void Consumer::consume_item(WorkItem &item) {
#if 0
	std::cout << "Consumer " << m_id << " consumed following item:"<<std::endl;
	std::cout << "\t\tid: " << item.id << std::endl;
	std::cout << "\t\tdata: " << item.data << std::endl;
#endif
	m_consumed_count++;
}

void Consumer::work() {
	if(m_mqd == (mqd_t)-1)
		return;

	WorkItem item;
	ssize_t receivedBytes = 0;
	timespec ts = get_expire_time();
	// The reason why we must +1 to the sizeof(WorkItem) is that the mq_receive requires the
	// buffer size must be greater than the msgsize_max
	receivedBytes = mq_timedreceive(m_mqd, (char*)&item, sizeof(WorkItem) + 1, NULL, &ts);
	if(receivedBytes > 0)
		consume_item(item);
	else if(receivedBytes == 0)
		std::cout << "0 length message received" << std::endl;
	else if(errno != ETIMEDOUT) {
		std::cout << "Error is " << strerror(errno) << std::endl;
		std::cout << "Oops, fail to receive message from Consumer " << m_id << ". Bytes = " << receivedBytes << std::endl;
	}
}
