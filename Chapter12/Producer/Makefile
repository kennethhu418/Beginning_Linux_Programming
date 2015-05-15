ifneq ($(MAKECMDGOALS), pre_build)
    cfiles = $(shell ls *.cpp)
    objfiles = $(subst .cpp,.o,$(cfiles))
    allobjfiles = $(objfiles) $(shell ls ../Queue/*.o ../Schedule/*.o)
endif


all: $(allobjfiles)

pre_build:
	$(CC) -MM *.cpp > makedep.dep

ifneq ($(MAKECMDGOALS), pre_build)
    include makedep.dep
endif
