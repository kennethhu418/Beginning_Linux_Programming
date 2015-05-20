#!/bin/bash
# This shell has only one argument - the time to run this simulation in seconds

declare -i num_cores=$(nproc)
declare -i time_to_run=10
declare -i cur_core=0

if [ -n $1 ]; then
	time_to_run=$1
fi

while [ $cur_core -lt $num_cores ]; 
do
	logfile='/tmp/producer_consumer_sim_'${cur_core}'.log'
	./ProducerConsumerSim -c ${cur_core} -n ${cur_core} -t ${time_to_run} > $logfile &
	cur_core=$(($cur_core + 1))
done
