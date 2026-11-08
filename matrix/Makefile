all: build run

build:
	mpic++ matrix.cpp -o matrix.exe

run:
	mpirun --use-hwthread-cpus -np 8 matrix.exe

clean:
	rm -f matrix.exe
