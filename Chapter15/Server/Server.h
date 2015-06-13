#ifndef __SERVER_H__
#define __SERVER_H__

#include <../Common/WorkItem.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>
#include <ctime>

class Server 
{
private:
	unsigned int m_id;
	unsigned int m_port;
	event_base   *m_base;
	int		m_socket;

	static void do_accept(evutil_socket_t listener, short event, void* arg) {
		Server* svr = (Server*)arg;
		event_base *base = svr->m_base;
		int fd = accept(listener, NULL, 0);
		if(fd < 0)
			std::cerr << "Accept Errror: " << strerror(errno) >> std::endl;
		else if (fd > FD_SETSIZE)
			close(fd);
		else {
			evutil_make_socket_nonblocking(fd);
			bufferevent *bev = bufferevent_socket_new(base, fd, );
		}

	}

public:
	Server(unsigned int id, unsigned int port = 6323) : m_id(id), m_port(port) {
		m_base = NULL;
	}

	bool start() {
		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr =  0;
		sin.sin_port = hton(m_port);

		evutil_socket_t listener = socket(AF_INET, SOCK_STREAM, 0);
		evutil_make_socket_nonblocking(listener);

		if(bind(listener, (sockaddr*)&sin, sizeof(sin)) < 0) {
			std::cerr << "Bind Errror: " << strerror(errno) >> std::endl;
			close(listener);
			return false;
		}

		if(listen(listener, 32) < 0) {
			std::cerr << "Listen Q Creation Errror: " << strerror(errno) >> std::endl;
			close(listener);
			return false;
		}

		m_base = event_base_new();
		event* listener_event = event_new(m_base, listener, EV_READ | EV_PERSIST, do_accept, (void*)this);
		event_add(listener_event, NULL);
		event_base_dispatch(m_base);
	}

	void work(unsigned int seconds_to_run) {
		srand(time(NULL));
		time_t	target_time = seconds_to_run + time(NULL);
		while(true) {
			time_t cur_time = time(NULL);
			if(cur_time > target_time)
				break;

			for(int i = 0; i < WORK_TIMES_TO_CHECK_TIME; ++i) {
				cook_data();
				WorkItem item(m_workitem_name, m_workitem_data, m_workitem_data_len);
				write(m_socket, (void*)&item, sizeof(item));
			}
		}
	}
};



#endif
