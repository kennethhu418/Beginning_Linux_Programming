#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <ctime>
#include <errno.h>

#define WAIT_TIME 1 // try to get sem lock every 1 seconds

template <class T>
class Queue {
private:
	int		m_id;
	int		m_front;
	int		m_cur_count;
	int		m_size;
	int		m_pushed_count;
	int		m_popped_count;
	sem_t	m_lock;
	sem_t	m_pop_sem;
	sem_t	m_push_sem;

	timespec get_expire_time() {
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += WAIT_TIME;
		return ts;
	}

	T* transform_data_addr() {
		return (T*)((char*)this + sizeof(*this));
	}

public:
	// Caller gurantees that enough memory should be allocated for this Queue
	// The memory allocated after this Queue is used as the data region
	Queue(int size = 100, int id = 0) : m_size(size), m_front(0), m_cur_count(0) {
		T* data_arr = new ((char*)this + sizeof(*this))T[size];
		
		if(sem_init(&m_lock, 1, 1) < 0)
			std::cout << "Initialize m_lock failed" << std::endl;
		if(sem_init(&m_push_sem, 1, size) < 0)
			std::cout << "Initialize m_push_sem failed" << std::endl;
		if(sem_init(&m_pop_sem, 1, 0) < 0)
			std::cout << "Initialize m_pop_sem failed" << std::endl;

		m_pushed_count = 0;
		m_popped_count = 0;
		m_id = id;
	}

	~Queue() {
		sem_destroy(&m_lock);
		sem_destroy(&m_push_sem);
		sem_destroy(&m_pop_sem);
	}

	void report() {
		std::cout << "Queue " << m_id << ": Pushed " << m_pushed_count << " items and popped " << m_popped_count << " items." << std::endl;
	}

	bool push(const T &t) {
		T* data_arr = transform_data_addr();
		timespec ts = get_expire_time();
		if(0 != sem_timedwait(&m_push_sem, &ts)) {
			return false;
		}
		sem_wait(&m_lock);
		int slot = (m_front + m_cur_count) % m_size;
		++m_cur_count;
		++m_pushed_count;
		sem_post(&m_lock);
		data_arr[slot] = t;
		sem_post(&m_pop_sem);
		return true;
	}

	bool pop(T& target) {
		T* data_arr = transform_data_addr();
		timespec ts = get_expire_time();
		if(sem_timedwait(&m_pop_sem, &ts) != 0) {
			return false;
		}
		sem_wait(&m_lock);
		int slot = m_front;
		m_front = (m_front + 1) % m_size;
		--m_cur_count;
		++m_popped_count;
		sem_post(&m_lock);
		target = data_arr[slot];
		sem_post(&m_push_sem);
		return true;
	}
};



#endif
