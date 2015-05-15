ifneq ($(MAKECMDGOALS), pre_build)
    cfiles = $(shell ls *.cpp)
    objfiles = $(subst .cpp,.o,$(cfiles))
    allobjfiles = $(objfiles) $(shell ls ../Consumer/*.o ../Producer/*.o ../Queue/*.o ../Schedule/*.o)
endif


ProducerConsumerSim: $(allobjfiles)
	$(CC) $(CFLAGS) $(LOADLIBES) -o ProducerConsumerSim $(allobjfiles)

pre_build:
	$(CC) -MM *.cpp > makedep.dep

ifneq ($(MAKECMDGOALS), pre_build)
    include makedep.dep
endif

