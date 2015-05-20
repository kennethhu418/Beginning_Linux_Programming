export CC = g++
export LOADLIBES = -lpthread -lrt
export CFLAGS := $(CFLAGS) -pthread -ggdb
export CXXFLAGS := $(CXXFLAGS) -pthread -ggdb

all:
	(cd ./Schedule; make pre_build) #begin prebuild
	(cd ./Consumer; make pre_build)
	(cd ./Producer; make pre_build)
	(cd ./Sim; make pre_build)
	(cd ./Schedule; make) #begin build
	(cd ./Consumer; make)
	(cd ./Producer; make)
	(cd ./Sim; make)

install:
	-mkdir ./bin
	-rm -rf ./bin/*
	-cp -a ./Sim/ProducerConsumerSim ./Sim/ProducerSim ./Sim/ConsumerSim ./Sim/RunSim.sh ./bin
	-chmod 555 ./bin/ProducerConsumerSim ./bin/ProducerSim ./bin/ConsumerSim ./bin/RunSim.sh

clean:
	find ./ -name *.o | xargs rm -f
	find ./ -name makedep.dep | xargs rm -f

