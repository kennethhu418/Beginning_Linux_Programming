cfiles = $(shell ls *.cpp)
objfiles = $(subst .cpp,.o,$(cfiles)) $(shell ls ../../serialize/*.o)

all: $(objfiles)

pre_build:
	$(CC) -MM *.cpp > makedep.dep

ifneq ($(MAKECMDGOALS), pre_build)
    include makedep.dep
endif
