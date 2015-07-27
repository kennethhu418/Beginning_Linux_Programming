export CC=g++ -g 

all:
	cd Client; make pre_build
	cd Server; make pre_build
	cd Client; make
	cd Server; make

.PHONY: clean
clean:
	find ./ -name "*.dep" | xargs rm -rf
	find ./ -name "*.o" | xargs rm -rf
	-rm -rf ./bin

.PHONY: install
install:
	-mkdir bin
	-mv Client/Client bin
	-mv Server/Server bin
	-cp Test/test.sh bin
	-chmod 555 bin/test.sh
