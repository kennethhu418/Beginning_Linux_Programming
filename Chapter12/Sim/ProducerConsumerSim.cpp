#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include "../Consumer/Consumer.h"
#include "../Producer/Producer.h"
#include "../Queue/Queue.h"

#define DEFAULT_PRODUCERS_PER_CORE 1
#define DEFAULT_CONSUMERS_PER_CORE 1
#define DEFAULT_SIM_TIME_IN_SECS   10
#define DEFAULT_QUEUE_SIZE         100 
#define MAX_SYSTEM_CORES           32

static const char* optoptions = "ht:p:c:s:";

using namespace std;

void print_help() {
	cout << "-h show this help" << endl;
	cout << "-t how many seconds the test will run" << endl;
	cout << "-p how many producers on each core" << endl;
	cout << "-c how many consumers on each core" << endl;
	cout << "-s the queue size" << endl;
}

int main(int argc, char **argv) {
	int		num_cores = sysconf(_SC_NPROCESSORS_ONLN);
	cout << "====== System has " << num_cores << " cores" << endl;
	int		run_seconds = DEFAULT_SIM_TIME_IN_SECS;
	int		producers_per_core = DEFAULT_PRODUCERS_PER_CORE;
	int     consumers_per_core = DEFAULT_CONSUMERS_PER_CORE;
	int		queue_size = DEFAULT_QUEUE_SIZE;
	int		opt = 0;
	int policy_arr[MAX_SYSTEM_CORES];
	Producer**	producer_arr;
	Consumer**	consumer_arr;
	Queue<WorkItem>*		queue_arr[MAX_SYSTEM_CORES];


	while( -1 != (opt = getopt(argc, argv, optoptions))) {
		switch(opt) {
		case 'h':
			print_help();
			return 0;
			break;
		case 't':
			run_seconds = atoi(optarg);
			break;
		case 'p':
			producers_per_core = atoi(optarg);
			break;
		case 'c':
			consumers_per_core = atoi(optarg);
			break;
		case 's':
			queue_size = atoi(optarg);
			break;
		case '?':
			cerr << "Invalid options: " << (char)opt << endl;
			break;
		}
	}

	for(int i = 0; i < num_cores; ++i) {
		policy_arr[i] = SCHED_OTHER;
		queue_arr[i] = new Queue<WorkItem>(queue_size, i);
	}
	// core 0 producers and consumers have real time priority
	policy_arr[0] = SCHED_RR;

	int cur_producer_id = 0, cur_consumer_id = 0;
	producer_arr = new Producer*[num_cores*producers_per_core];
	consumer_arr = new Consumer*[num_cores*consumers_per_core];
	for(int cur_core = 0; cur_core < num_cores; ++cur_core) {
		for(int i = 0; i < consumers_per_core; ++i) {
			consumer_arr[cur_consumer_id] = new Consumer(queue_arr[cur_core], 
					cur_consumer_id, 1000 /*max priority*/, policy_arr[cur_core], cur_core);
			++cur_consumer_id;
		}
		for(int i = 0; i < producers_per_core; ++i) {
			producer_arr[cur_producer_id] = new Producer(queue_arr[cur_core], 
					cur_producer_id, 1000 /*max priority*/, policy_arr[cur_core], cur_core);
			++cur_producer_id;
		}
	}

	sleep(run_seconds);

	for(int i = 0; i < cur_producer_id; ++i)
		delete producer_arr[i];
	delete [] producer_arr;

	sleep(10);
	for(int i = 0; i < cur_consumer_id; ++i)
		delete consumer_arr[i];
	delete consumer_arr;

	for(int i = 0; i < num_cores; ++i)
		delete queue_arr[i];
	
	cout << "======Test Finished" << endl;
	return 0;
}
