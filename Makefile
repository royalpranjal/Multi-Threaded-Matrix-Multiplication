# A Makefile file that compiles the matmult.cpp program

matmult: matmult.cpp
	g++ -pthread matmult.cpp
