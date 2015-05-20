#include "Producer.h"
#include <sched.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <cstring>
#include <errno.h>

Producer::Producer(const char* msg_q_name, unsigned int id, int thread_priority,
				   int policy, int which_core) : m_id(id), m_mqd((mqd_t)-1), ScheduledObject(thread_priority, policy, which_core) {
	m_next_item_id = ITEM_STARTING_ID_EACH_PRODUCER * id;
	m_mqd = mq_open(msg_q_name, O_WRONLY);
	if(m_mqd == (mqd_t)-1) {
		std::cout << "Fail to open msg queue in Producer " << id << std::endl;
		std::cout << "Error: " << strerror(errno) << std::endl;
	}
	srand(time(NULL));
	m_produced_count = 0;
}

Producer::~Producer() {
	mq_close(m_mqd);
	std::cout << "Producer " << m_id << " produces " << m_produced_count << " items." << std::endl;
}


void Producer::cook_item(WorkItem &item) {
	item.id = m_next_item_id++;
	int datalen = rand() % (MAX_WORK_ITEM_LEN - 1);
	for(int i = 0; i < datalen; ++i)
		item.data[i] = 'a' + rand() % 26;
	item.data[datalen] = '\0';
}

void Producer::work() {
	if(m_mqd == (mqd_t)-1)
		return;

	WorkItem item;
	cook_item(item);
	timespec ts = get_expire_time();
	ssize_t	sentBytes = mq_timedsend(m_mqd, (char*)&item, sizeof(WorkItem), 5, &ts); 
	if(sentBytes == 0)
		++m_produced_count;
	else if(errno != ETIMEDOUT) {
		std::cout << "Error is " << strerror(errno) << std::endl;
		std::cout << "Oops, fail to send message from Producer " << m_id << ". Bytes = " << sentBytes << std::endl;
	}
}

