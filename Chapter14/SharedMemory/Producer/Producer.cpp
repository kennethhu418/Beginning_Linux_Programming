#include "Producer.h"
#include <sched.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <time.h>

Producer::Producer(Queue<WorkItem> * workQ, unsigned int id, int thread_priority,
				   int policy, int which_core) : m_id(id), m_valid_saved_work_item(false), m_workq(workQ), ScheduledObject(thread_priority, policy, which_core) {
	m_next_item_id = ITEM_STARTING_ID_EACH_PRODUCER * id;
	srand(time(NULL));
}

Producer::~Producer() {

}


void Producer::cook_item(WorkItem &item) {
	item.id = m_next_item_id++;
	int datalen = rand() % (MAX_WORK_ITEM_LEN - 1);
	for(int i = 0; i < datalen; ++i)
		item.data[i] = 'a' + rand() % 26;
	item.data[datalen] = '\0';
}

void Producer::work() {
	if(!m_valid_saved_work_item) {
		cook_item(m_saved_work_item);
		m_valid_saved_work_item = true;
	}
	if(m_workq->push(m_saved_work_item)) {
		m_valid_saved_work_item = false;
	}
}

