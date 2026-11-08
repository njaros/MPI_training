all: build run

build:
	mpicc merge_sort.c -o merge_sort.exe

run:
	mpirun --use-hwthread-cpus -np 6 merge_sort.exe

clean:
	rm -f merge_sort.exe
