compile: vm.c
	gcc vm.c -o vm -lpthread

run:
	./vm addresses.txt lru lru

clean:
	rm -rf *.o vm correct.txt
