export CC = g++
export LOADLIBES = -lpthread
export CFLAGS := $(CFLAGS) -pthread
export CXXFLAGS := $(CXXFLAGS) -pthread

all:
	(cd ./Schedule; make pre_build) #begin prebuild
	(cd ./Queue; make pre_build)
	(cd ./Consumer; make pre_build)
	(cd ./Producer; make pre_build)
	(cd ./Sim; make pre_build)
	(cd ./Schedule; make) #begin build
	(cd ./Queue; make)
	(cd ./Consumer; make)
	(cd ./Producer; make)
	(cd ./Sim; make)

install:
	-mkdir ./bin
	-rm -rf ./bin/*
	-cp -a ./Sim/ProducerConsumerSim ./bin
	-chmod 555 ./bin/ProducerConsumerSim

clean:
	find ./ -name *.o | xargs rm -f
	find ./ -name makedep.dep | xargs rm -f

