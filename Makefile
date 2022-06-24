CC=g++
all: main

main: main.o cpu.o main_class.o thread.o system.o debug.o semaphore.o
	g++ main.o cpu.o main_class.o thread.o system.o debug.o semaphore.o -o main

main.o: main.cc
	g++ -c main.cc

system.o: system.cc
	g++ -c system.cc
	
cpu.o: cpu.cc
	g++ -c cpu.cc

main_class.o: main_class.cc
	g++ -c main_class.cc

debug.o: debug.cc
	g++ -c debug.cc

thread.o: thread.cc thread.h
	g++ -c thread.cc

semaphore.o: semaphore.cc semaphore.h
	g++ -c semaphore.cc

clean:
	rm *.o main



