INCLUDES := -I ../Common

ifneq ($(MAKECMDGOALS), pre_build)
	cfiles := $(shell ls *.cpp)
	objfiles := $(subst .cpp,.o,$(cfiles))
	objfiles += $(ls ../Common/*.o)
endif

Client:$(objfiles)
	$(CC) $(INCLUDES) $(CFLAGS) -o $@ $<

$(objfiles):$(cfiles)
	$(CC) -c $(INCLUDES) $(CFLAGS) -o $@ $<

pre_build:
	$(CC) $(INCLUDES) -MM *.cpp > makedep.dep

ifneq ($(MAKECMDGOALS), pre_build)
include makedep.dep
endif

