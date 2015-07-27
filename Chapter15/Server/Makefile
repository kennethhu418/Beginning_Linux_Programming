INCLUDES := -I ../Common

ifneq ($(MAKECMDGOALS), pre_build)
	cfiles := $(shell ls *.cpp)
	objfiles := $(subst .cpp,.o,$(cfiles))
	objfiles += $(ls ../Common/*.o)
endif

Server:$(objfiles)
	$(CC) $(INCLUDES) $(CFLAGS) -o $@ $<

pre_build:
	$(CC) $(INCLUDES) -MM *.cpp > makedep.dep

$(objfiles):$(cfiles)
	$(CC) -c $(INCLUDES) $(CFLAGS) -o $@ $<

ifneq ($(MAKECMDGOALS), pre_build)
include makedep.dep
endif

