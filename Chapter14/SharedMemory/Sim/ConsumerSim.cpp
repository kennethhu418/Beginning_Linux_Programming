#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "../Consumer/Consumer.h"
#include "../Queue/Queue.h"
#include "./SharedMem.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

static const char* optoptions = "hc:n:t:";

using namespace std;

void print_help() {
	cout << "-h show this help" << endl;
	cout << "-c On which core this consumer sim runs" << endl;
	cout << "-n The ID of this consumer." << endl;
	cout << "-t The time in seconds to run of this consumer." << endl;
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

	// Attach the shared memory
	char shared_mem_name[30];
	sprintf(shared_mem_name, SHARED_MEM_NAME_PATTERN, id);
	int shm_fd = shm_open(shared_mem_name, O_RDWR, 0666);
	if(shm_fd == -1) {
		cout << "Shared Memory open failure." << endl;
		_exit(-1);
	}

	Queue<WorkItem>* queue = (Queue<WorkItem>*)mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if((void*)queue == MAP_FAILED) {
		cout << "Memory Mapping Failed." << endl;
		_exit(-2);
	}

	cout << "====== Consumer " << id << " starts and will run for " << seconds_to_run << "seconds." << endl;

	Consumer *consumer = new Consumer(queue, id, 1000, target_core == 0 ? SCHED_RR : SCHED_OTHER, target_core);

	sleep(seconds_to_run);

	delete consumer;
	munmap((void*)queue, SHARED_MEM_SIZE);
	shm_unlink(shared_mem_name);


	cout << "====== Consumer " << id << " destroys." << endl;
	_exit(0);
}

