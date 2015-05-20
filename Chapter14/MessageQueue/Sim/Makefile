ifneq ($(MAKECMDGOALS), pre_build)
    allobjfiles = $(shell ls ../Consumer/*.o ../Producer/*.o ../Schedule/*.o)
endif

all: ProducerConsumerSim ProducerSim ConsumerSim

ProducerConsumerSim : $(allobjfiles) ProducerConsumerSim.o
	$(CC) $(CFLAGS) -o ProducerConsumerSim $(allobjfiles) ProducerConsumerSim.o $(LOADLIBES) 

ProducerSim : $(allobjfiles) ProducerSim.o
	$(CC) $(CFLAGS) -o ProducerSim $(allobjfiles) ProducerSim.o $(LOADLIBES) 

ConsumerSim : $(allobjfiles) ConsumerSim.o
	$(CC) $(CFLAGS) -o ConsumerSim $(allobjfiles) ConsumerSim.o $(LOADLIBES) 

pre_build:
	$(CC) -MM *.cpp > makedep.dep

ifneq ($(MAKECMDGOALS), pre_build)
    include makedep.dep
endif

