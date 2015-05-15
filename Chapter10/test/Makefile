ifneq ($(MAKECMDGOALS), pre_build)
  cfiles := $(shell ls *.cpp)
  objfiles = $(subst .cpp,.o,$(cfiles)) $(shell ls ./util/*.o) $(shell ls ../serialize/*.o)
endif

test_tree_serialize : $(objfiles)
	$(CC) -o $@ $(objfiles)

pre_build:
	$(CC) -MM *.cpp > makedep.dep 
	cd ./util; make pre_build
	cd ./util; make #Since we need the object files in the subdirectory, we need to build the subdirectory before we can make ourself!!!

ifneq ($(MAKECMDGOALS), pre_build)
    include makedep.dep
endif
