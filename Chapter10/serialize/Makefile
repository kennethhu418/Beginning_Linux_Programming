cfiles = $(shell ls *.cpp)
objfiles = $(subst .cpp,.o,$(cfiles))

libserialize.a : $(objfiles)
	${AR} r $@ $<

pre_build:
	$(CC) -MM *.cpp > makedep.dep

ifneq ($(MAKECMDGOALS), pre_build)
    include makedep.dep
endif
