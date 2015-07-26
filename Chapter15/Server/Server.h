#ifndef __SERVER_H__
#define __SERVER_H__

#include <../Common/WorkItem.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>
#include <ctime>

#define MAX_EVENTS_RECEIVED 512

class Server 
{
private:
	unsigned int m_id;
	unsigned int m_port;
	int		m_socket;

    void make_socket_nonblock() {
        int     flags = fcntl(m_socket, F_GETFL, 0);
        flags |= O_NONBLOCK;
        fcntl(m_socket, F_SETFL, flags);
    }

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

	}

	bool start() {
		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr =  0;
		sin.sin_port = hton(m_port);

		m_socket = socket(AF_INET, SOCK_STREAM, 0);
        make_socket_nonblock();

        hostent* info = gethostbyname("localhost");
        sin.sin_addr.s_addr = *(*in_addr)*(info->h_addr_list);

		if(bind(m_socket, (sockaddr*)&sin, sizeof(sin)) < 0) {
			std::cerr << "Bind Errror: " << strerror(errno) >> std::endl;
			close(m_socket);
			return false;
		}

		if(listen(m_socket, 32) < 0) {
			std::cerr << "Listen Q Creation Errror: " << strerror(errno) >> std::endl;
			close(m_socket);
			return false;
		}

        int     ev = epoll_create1(0);
        epoll_event     event, event_result[MAX_EVENTS_RECEIVED];
        
        event.events = EPOLLIN;
        event.data.fd = m_socket;
        epoll_ctl(ev, EPOLL_CTL_ADD, m_socket, &event);

        WorkItem witem("default");

        while(true) {
            int cnt = epoll_wait(ev, event_result, MAX_EVENTS_RECEIVED, -1);
            for(int i = 0; i < cnt; ++i) {
                if(event_result[i].fd == m_socket) {
                    // there is new connection, need to accept
                    int new_fd = accept(m_socket, (sockaddr*)&sin, sizeof(sin));
                    make_socket_nonblock(new_fd);
                    event.events = EPOLLIN;
                    event.data.fd = new_fd;
                    epoll_ctl(ev, EPOLL_CTL_ADD, new_fd, &event);

                    std::cout << "Establish connection with client: " << 
                }
                else if(event_result[i].events & EPOLLIN) {
                    int n = read(event_result[i].data.fd, &item, sizeof(item));
                    if(n = 0) {
                        close(event_result[i].data.fd);
                    }
                    else if(n != sizeof(item)) {
                        std::cerr << "Error receiving non-itegrated data." << std::endl;
                        close(event_result[i].data.fd);
                    }
                    else {
                        if(witem.validate() == false) {
                            std::cerr << "Error: Received Data is corrupted." <<std::endl;
                            std::cerr << "    WorkItem: " << witem.getName() << std::endl;
                        } 
                    }
                }
            }
        
        }

        return true;
	}

};



#endif
