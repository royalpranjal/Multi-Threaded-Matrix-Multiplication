#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
#include <ctime>
#include <iomanip>

using namespace std;

vector< vector<int> > mat1;
vector< vector<int> > mat2;
vector< vector<int> > mat3;


int curr_i,curr_j = 0;

int _m, _n, _p = 0;

/* So that data will not get intermixed while using multiple threads*/
pthread_mutex_t lock;

/* POOL OF THREADS*/
pthread_t *threads;

/* Whether the thread is created or terminated*/
int *threadstatus;

/* Each thread computes a single element in matrix*/
void* compute(int id){
	while(true){
		int loc_i, loc_j;

		pthread_mutex_lock(&lock); //So that data will not get intermixed with multiple threads.
		loc_i = curr_i;
		loc_j = curr_j;
		if(curr_i < _m){
			curr_j++;
			if(curr_j >= _p){
				curr_i++;
				curr_j = 0;
			}
		}
		pthread_mutex_unlock(&lock);
		if(loc_i < _m){
			mat3[loc_i][loc_j] = 0;
			for(int i = 0;i < _n;i++){
				mat3[loc_i][loc_j] += mat1[loc_i][i] * mat2[i][loc_j];
			}
		}
		if(threadstatus[id-1] == 0){
			pthread_exit(0);
		}
	}
}

/*  Main Function  */
int main(){
	int m, n, p, num;
	num = 0;

	while(true){
		int no, i, j, entry;
		m = n = p = i = j = 0;

		string in1, in2, out1;

		/* Taking input of Matrices from user */
		cout << "\nEnter M, N, P where we multiply matrix A [MxN] & matrix B [NxP]" << endl;
		cout << "Enter M : ";
		cin >> m;
        cout << "Enter N : ";
		cin >> n;
		cout << "Enter P : ";
		cin >> p;
		cout << "Enter the number of threads : ";
		cin >> no;
		
		while(m <= 0 || n <= 0 || p <= 0 || no <= 0) {
			if(m <= 0){
				cout << "Integer should be greater than 0";
				cout << "Enter M : ";
				cin >> m;
			}
			if(n <= 0){
				cout << "Integer should be greater than 0";
				cout << "Enter N : ";
				cin >> n;
			}
			if(p <= 0){
				cout << "Integer should be greater than 0";
				cout << "Enter P : ";
				cin >> p;
			}
			if(no <= 0){
				cout << "Integer should be greater than 0";
				cout << "Enter the number of threads : ";
				cin >> no;
			}
		}

		cout<< "\nEnter name of input file for Matrix A: ";
		cin >> in1;
		
		// Allocating rows
		mat1.resize(m);
		mat2.resize(n);
		mat3.resize(m);

		// Allocating columns
		for(int i = 0;i < m; i++){
			mat1[i].resize(n);
			mat3[i].resize(p);
		}

		for(int i = 0;i < n;i++){
			mat2[i].resize(p);
		}

        // Opening the input file
		ifstream file;
		file.open(in1.c_str());
		
		//***Error checking if the file exists***
		if(!file.is_open()){
			cout << "Cannot open file\n";
			return;
		}
		i = 0;
		j = 0;

		/* Taking input of Matrices from files for Matrix A */
		while (1){
			file >> entry;
			mat1[i][j] = entry;
			j++;
			if(j >= n){
				i++;
				j = 0;
			}	
			if(i >= m)
				break;
		}

		cout << "\nMatrix A of order " << m << " x" << n << " is : \n";

		for(i = 0; i < m; i++){
			for(j = 0; j < n; j++){
				cout << setw(5) << mat1[i][j] << " ";
			}
			cout << endl;
		}
		file.close();


		cout << "\nEnter name of input file for Matrix B: ";
		cin >> in2;

		file.open(in2.c_str());
		
		if(!file.is_open()){
			cout << "Cannot open the file\n";
		}	return;

	        i = 0;
		j = 0;

		/* Taking input of Matrices from files for Matrix B */
		while (1){
			file >> entry;
			mat2[i][j] = entry;
			j++;
			if(j >= p){
				i++;
				j = 0;
			}
			if(i >= n){
				break;
			}
		}

		cout << "\nMatrix B of order " << n << " x" << p << " is : \n";
		for(i = 0; i < n; i++){
			for(j = 0; j < p; j++){
				cout << setw(5) << mat2[i][j] << " ";
			}
			cout << endl;
		}
		file.close();

		cout << endl;

		//to prevent intermixing of data
		pthread_mutex_lock(&lock);
		if(num > no){
			for(;num > no; num--){
                // deleting threads which are extra
				threadstatus[num-1] = 0;
				cout<<"Thread "<<num<<" terminated\n";
			}
			threads = (pthread_t *) realloc(threads, sizeof(pthread_t) * num);
			cout << "\nThere are " << num << " threads in the thread pool.\n\n";
		}

		curr_i = 0;
		curr_j = 0;
		_m = m;
		_n = n;
		_p = p;

		pthread_mutex_unlock(&lock);

        // Starting the clock to note execution time
        double duration;
        clock_t start;
        start = clock();

		//Creating threads
		if(no > num){
			threads = (pthread_t *) realloc(threads, sizeof(pthread_t) * no);
			threadstatus = (int *) realloc(threadstatus, sizeof(int) * no);
			for(;num < no; num++){
				threadstatus[num] = 1;
				pthread_create(&threads[num], NULL, (void *(*) (void *)) compute, (void *) (num + 1));
				cout << "Thread " << (num + 1) << " created\n";
			}
			cout << "\nThere are " << num <<" threads in the thread pool.\n\n";
		}

		cout << "Enter name of output file : ";
		cin >> out1;

		/***OPENING OUTPUT FILE***/
		ofstream file2;
		file2.open(out1.c_str());


		/*Final thread that is main function gives final output*/
		// bounded wait
		while(curr_i <_m);
       		file2 << "\nThe resultant matrix is - \n";
		for(int i = 0;i < m;i++){
			for(int j = 0;j < p;j++){
				file2<<mat3[i][j]<<"\t";
			}
			file2<<"\n";
		}

		// printing the time
        cout << "time: " << (clock()-start)/double(CLOCKS_PER_SEC) << endl;

		cout << "\nIf u wantto continue, press 'y'. Else press 'n': \n";
		cin >> in1;
		if(in1 != "y"){
			break;
		}
		file2.close();
		/***CLOSING OUTPUT FILE***/
	}

	return 0;
}
