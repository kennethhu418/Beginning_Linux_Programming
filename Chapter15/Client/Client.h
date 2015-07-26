#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <../Common/WorkItem.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include <iostream>
#include <ctime>

#define WORK_TIMES_TO_CHECK_TIME	10

class Client
{
private:
	unsigned int m_id;
	char	m_server_name[256];
	int		m_server_port;
	int		m_socket;
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

	bool connect() {
		hostent* hostinfo = gethostbyname(m_server_name);
		if(hostent == NULL) {
			std::cerr << "Error getting host by name with " << strerror(errno) << std::endl;
			return false;
		}

		sockaddr_in sin;
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		this->m_socket = sockfd;
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = *(in_addr*)*(hostinfo->h_addr_list);
		sin.sin_port = htons((short)m_server_port);

		if(connect(sockfd, (sockaddr*)&sin, sizeof(sin)) == -1) {
			std::cerr << "Fail to connect to server with " << strerror(errno) << std::endl;
			return false;
		}

		return true;
	}

	bool disconnect() {
		close(m_socket);
	}

	void work(unsigned int seconds_to_run) {
		srand(time(NULL));
		time_t	target_time = seconds_to_run + time(NULL);
		while(work) {
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
