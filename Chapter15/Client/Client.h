#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "WorkItem.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>

#define WORK_TIMES_TO_CHECK_TIME	10
#define CLIENT_START_PORT 			6666

class Client
{
private:
	unsigned int m_id;
	char	m_server_name[256];
	int		m_server_port;
	int		m_socket;
	int     m_client_socket;
	char    m_workitem_name[30];
	char	m_workitem_data[DATA_LEN];
	int		m_workitem_data_len;
	unsigned int m_cooked_count;

	void cook_data() {
		char base = rand() % 3 == 0 ? 'A' : 'a';
		m_workitem_data_len = 1 + rand() % DATA_LEN;
		for(int i = 0; i < m_workitem_data_len; ++i)
			m_workitem_data[i] = rand() % 26 + base;
		sprintf(m_workitem_name, "WorkItem_From_Client_%d_%d", 
				m_id, m_cooked_count++);
	}

public:
	Client(unsigned int id, const char* server_name = NULL, int port = 6323) {
		this->m_id = id;
		if(server_name == NULL)
			strcpy(this->m_server_name, "localhost");
		else
			strcpy(this->m_server_name, server_name);
		this->m_server_port = port;
		m_cooked_count = 0;
	}

	bool Connect() {
		hostent* hostinfo = gethostbyname("localhost");
		if(hostinfo == NULL) {
			std::cerr << "Error getting localhost by name with " << strerror(errno) << std::endl;
			return false;
		}

		m_socket = socket(AF_INET, SOCK_STREAM, 0);

		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = *(in_addr_t*)*(hostinfo->h_addr_list);
		sin.sin_port = htons(CLIENT_START_PORT + m_id);

		if(bind(m_socket, (sockaddr*)&sin, sizeof(sin)) < 0) {
			std::cerr << "Bind Errror: " << strerror(errno) << std::endl;
			close(m_socket);
			return false;
		}
		else {
			std::cout << "Bound client " << m_id << "  to " << inet_ntoa(sin.sin_addr) << ":" << ntohs(sin.sin_port) << std::endl;
		}


		hostinfo = gethostbyname(m_server_name);
		if(hostinfo == NULL) {
			std::cerr << "Error getting server by name with " << strerror(errno) << std::endl;
			close(m_socket);
			return false;
		}
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = *(in_addr_t*)*(hostinfo->h_addr_list);
		sin.sin_port = htons((short)m_server_port);

		if(connect(m_socket, (sockaddr*)&sin, sizeof(sin)) == -1) {
			std::cerr << "Fail to connect to server with " << strerror(errno) << std::endl;
			return false;
		}

		return true;
	}

	bool Disconnect() {
		close(m_socket);
	}

	void Work(unsigned int seconds_to_run) {
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
