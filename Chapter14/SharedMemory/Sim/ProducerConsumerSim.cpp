#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <errno.h>
#include <cstdlib>
#include "./SharedMem.h"
#include "../Producer/Producer.h"
#include "../Consumer/Consumer.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
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

	// Create the shared memory
	char shared_mem_name[30];
	sprintf(shared_mem_name, SHARED_MEM_NAME_PATTERN, id);
	int shm_fd = shm_open(shared_mem_name, O_RDWR | O_CREAT| O_TRUNC, 0666);
	if(shm_fd == -1) {
		cout << "Shared Memory create failure." << endl;
		cout << "Error: " << strerror(errno) << endl;
		_exit(-1);
	}

	ftruncate(shm_fd, SHARED_MEM_SIZE);

	void* shared_mem_addr = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	Queue<WorkItem>* queue = new (shared_mem_addr)Queue<WorkItem>(QUEUE_SIZE, id); 

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
	
	queue->report();
	munmap(shared_mem_addr, SHARED_MEM_SIZE);
	shm_unlink(shared_mem_name);

	cout << "====== ProducerConsumer " << id << " simulation finishes." << endl;
	_exit(0);
}
