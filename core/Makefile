
all: main.o sst.o task.o
	cc -o sst main.o sst.o task.o

main.o : main.c sst.h
	cc -c main.c

sst.o : sst.c sst.h
	cc -c sst.c

task.o: ../task.c sst.h
	cc -c ../task.c

clean:
	rm -rf *.o sst


