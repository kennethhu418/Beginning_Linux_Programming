#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <errno.h>
#include <cstdlib>
#include "./SharedMem.h"
#include "../Producer/Producer.h"
#include "../Consumer/Consumer.h"
#include <sys/wait.h>

static const char* optoptions = "hc:n:t:";

using namespace std;

void print_help() {
	cout << "-h show this help" << endl;
	cout << "-c On which core this sim runs" << endl;
	cout << "-n The ID of the producer and consumer." << endl;
	cout << "-t The time in seconds to run simulation" << endl;
}

int main(int argc, char **argv) {
	int		opt = 0;
	int		id = 0;
	int	    target_core = 0;
	int		seconds_to_run = 10;

	while( -1 != (opt = getopt(argc, argv, optoptions))) {
		switch(opt) {
		case 'h':
			print_help();
			_exit(0);
		case 'c':
			target_core = atoi(optarg);
			break;
		case 'n':
			id = atoi(optarg);
			break;
	    case 't':
			seconds_to_run = atoi(optarg);
			break;
		case '?':
			cerr << "Invalid options: " << (char)opt << endl;
			break;
		}
	}

	// Create the message queue name
	char shared_mem_name[30];
	sprintf(shared_mem_name, SHARED_MEM_NAME_PATTERN, id);
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = QUEUE_SIZE;
	attr.mq_msgsize = sizeof(WorkItem);

	mqd_t mqd = mq_open(shared_mem_name, O_RDWR | O_CREAT, 0644, &attr);
	if(mqd == (mqd_t)-1) {
		cout << "Create message queue failed with error " << strerror(errno) << endl;
		return -1;
	}

	char core_str[4];
	char id_str[12];
	char time_str[12];
	itoa(target_core, core_str, 10);
	itoa(id, id_str, 10);
	itoa(seconds_to_run, time_str, 10);

	// Create the Producer simulation
	pid_t producer_pid = fork();
	if(producer_pid == 0) {
		execl("ProducerSim", "ProducerSim", "-c", core_str, "-n", id_str, "-t", time_str, NULL);
	}
	else if(producer_pid < 0) {
		cout << "Error on creating producersim child process" << endl;
		_exit(-2);
	}
	else
		cout << "Producer Pid is " << producer_pid << endl;

	// Create the Consumer simulation
	pid_t consumer_pid = fork();
	if(consumer_pid == 0) {
		execl("ConsumerSim", "ConsumerSim", "-c", core_str, "-n", id_str, "-t", time_str, NULL);
	}
	else if(consumer_pid < 0) {
		cout << "Error on creating consumersim child process" << endl;
		exit(-2);
	}
	else
		cout << "Consumer Pid is " << consumer_pid << endl;

	// Wait for consumer and producer process to finish
    int status;
	if(producer_pid > 0)
		waitpid(producer_pid, &status, 0);
	if(consumer_pid > 0)
		waitpid(consumer_pid, &status, 0);
	
	mq_close(mqd);
	if(mq_unlink(shared_mem_name) == -1) {
		cout << "Fail to delete message queue with error " << strerror(errno) << endl;
	}  

	cout << "====== ProducerConsumer " << id << " simulation finishes." << endl;
	_exit(0);
}
