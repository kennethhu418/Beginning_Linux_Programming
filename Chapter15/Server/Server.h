#ifndef __SERVER_H__
#define __SERVER_H__

#include "WorkItem.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>
#include <netdb.h>
#include <errno.h>
#include <iostream>
#include <ctime>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_EVENTS_RECEIVED 512

class Server 
{
private:
	unsigned int m_id;
	unsigned int m_port;
	int		m_socket;

    void make_socket_nonblock(int socket) {
        int     flags = fcntl(socket, F_GETFL, 0);
        flags |= O_NONBLOCK;
        fcntl(socket, F_SETFL, flags);
    }

public:
	Server(unsigned int id, unsigned int port = 6323) : m_id(id), m_port(port) {

	}

	bool start() {
		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr =  0;
		sin.sin_port = htons(m_port);

		m_socket = socket(AF_INET, SOCK_STREAM, 0);
        make_socket_nonblock(m_socket);

        hostent* info = gethostbyname("localhost");
        sin.sin_addr.s_addr = *(in_addr_t*)*(info->h_addr_list);

		if(bind(m_socket, (sockaddr*)&sin, sizeof(sin)) < 0) {
			std::cerr << "Bind Errror: " << strerror(errno) << std::endl;
			close(m_socket);
			return false;
		}
		else {
			std::cout << "Bound Server to " << inet_ntoa(sin.sin_addr) << ":" << ntohs(sin.sin_port) << std::endl;
		}

		if(listen(m_socket, 32) < 0) {
			std::cerr << "Listen Q Creation Errror: " << strerror(errno) << std::endl;
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
                if(event_result[i].data.fd == m_socket) {
                    // there is new connection, need to accept
                    int new_fd = accept(m_socket, NULL, NULL);
                    make_socket_nonblock(new_fd);
                    event.events = EPOLLIN;
                    event.data.fd = new_fd;
                    epoll_ctl(ev, EPOLL_CTL_ADD, new_fd, &event);

                    std::cout << "Establish connection with a new client " << std::endl;
                }
                else if(event_result[i].events & EPOLLIN) {
                    int n = recv(event_result[i].data.fd, &witem, sizeof(witem), MSG_WAITALL);
                    if(n == 0) {
                        close(event_result[i].data.fd);
                    }
                    else if(n != sizeof(witem)) {
                        std::cerr << "Error receiving non-itegrated data. Encountered Signal Interrupt?" << std::endl;
						std::cerr << "Received " << n << " bytes but we need " << sizeof(witem) << " bytes." << std::endl;
                        close(event_result[i].data.fd);
                    }
                    else {
                        if(witem.validate() == false) {
                            std::cerr << "Error: Received Data is corrupted." <<std::endl;
                            std::cerr << "    WorkItem: " << witem.getName() << std::endl;
                        } 
						else {
							// std::cout << "Successfully verify data." << std::endl;
							std::cout << "> Received valid data " << witem.getName() << std::endl;
						}
                    }
                }
            }
        
        }

        return true;
	}

};



#endif
