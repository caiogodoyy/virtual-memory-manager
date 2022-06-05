compile: vm.c
	gcc vm.c -o vm -lpthread

run:
	./vm addresses.txt fifo fifo

clean:
	rm -rf *.o vm outputs.txt
