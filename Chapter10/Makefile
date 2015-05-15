export CC = g++

all:
	-(cd ./serialize; make pre_build)
	-(cd ./test; make pre_build)
	-(cd ./serialize; make)
	-(cd ./test; make)

clean:
	find ./ -name makedep.dep | xargs rm -f
	find ./ -name *.o | xargs rm -f

install:
	-@mkdir ./bin #2>/dev/null
	-@mkdir ./lib #2>/dev/null
	@mv ./serialize/*.a ./lib
	@mv ./test/test_tree_serialize ./bin
