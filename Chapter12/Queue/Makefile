ifneq ($(MAKECMDGOALS), pre_build)
    cfiles = $(shell ls *.cpp)
    objfiles = $(subst .cpp,.o,$(cfiles))
endif

all: $(objfiles)

pre_build:
	-$(CC) -MM *.cpp > makedep.dep

ifneq ($(MAKECMDGOALS), pre_build)
    include makedep.dep
endif

