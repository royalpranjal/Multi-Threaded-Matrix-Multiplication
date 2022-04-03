# A Makefile file that compiles the matmult.cpp program

matmult: matmult.cpp
	g++ -o matmult matmult.cpp -pthread 
