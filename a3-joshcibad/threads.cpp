/**
 *	Program Name: threads (CPSC351 Assignment 3)
 *	Author: Josh Ibad
 *	Professor: Dr. Gofman
 *	Class: CPSC 351-03, Fall 2020
 *
 *	Ref: Random number generator: https://www.cplusplus.com/reference/cstdlib/rand/
 *	Ref: Dr. Gofman's Lectures and Sample Codes
 */
 
 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "BinaryTree.hpp" 	//Custom written BinaryTree class 
	// (Unbalanced. Only methods useful to this assignment are implemented)


/* Initialize the first mutex */
pthread_mutex_t treeMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * The function to be called by each child thread.
 * 2. Generates 100 random numbers between 0 and 100,000 and adds them to a
 * shared binary search tree whose pointer is inputted to each thread's
 * function call.
 * @param args - Pointer to the shared BST, in the form of a void pointer type.
 */
void* gen_rand(void* args){
	BinaryTree<int> * bin_tree = (BinaryTree<int> *) args;
	
	int rand_num;
	
	for(int tmp_i=0; tmp_i<100; tmp_i++){
		/* 2a. Generate a random number between 0 and 100,000 */
		rand_num = rand() % 100001;
		
		/* 2b. Add the random number to the tree ...
			3. ...using pthread mutexes to ensure synchronization */
		/* 3a. Acquire mutex lock */
		if(pthread_mutex_lock(&treeMutex) != 0){
			perror("pthread_mutex_lock");
			exit(-1);
		}
		/* 2b. Add random number to tree within critical section of execution */
		bin_tree->add(rand_num);
		/* 3b. Release mutex lock */
		if(pthread_mutex_unlock(&treeMutex) != 0){
			perror("pthread_mutex_unlock");
			exit(-1);
		}
	}
	
	pthread_exit(0);
}



int main(int argc, char** argv){
	/* 1. Check arguments */
	if(argc != 2){
		fprintf(stderr, "USAGE: %s <number>\n", argv[0]);
		exit(1);
	}
	int num_threads = atoi(argv[1]);
	if(num_threads <= 0){
		fprintf(stderr, "%s\n", "The <number> parameter must be a postive integer.");
		exit(1);
	}
	
	
	BinaryTree<int> bin_tree; //Binary tree instance to store the numbers;
	srand(time(NULL)); //Seed random number generator.
	
	/* Create num_threads amount of child threads to call the <>
		function. Thier id's will be stored in an array to be joined later*/
	pthread_t* tid = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
	for(int tmp_i=0; tmp_i < num_threads; tmp_i++){
		if( pthread_create( tid+tmp_i, NULL, gen_rand, &bin_tree) != 0 ){
			perror("pthread_create");
			exit(1);
		}
	}
	
	
	/* 4a. Join all created child threads (Await their termination) */
	for(int tmp_i=0; tmp_i < num_threads; tmp_i++){
		if( pthread_join(tid[tmp_i], NULL) != 0 ){
			perror("pthread_join");
			exit(1);
		}
	}
	/* 4b. Print all numbers in the bin tree */
	bin_tree.print();
	
	
	/* Clean up resources and return */
	free( tid );
	return 0;
}