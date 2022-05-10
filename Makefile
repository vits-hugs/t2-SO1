CC=g++
all:cpu.cc cpu.h main_class.cc main_class.h main.cc traits.h
	g++ cpu.cc main_class.cc main.cc traits.h -o main



