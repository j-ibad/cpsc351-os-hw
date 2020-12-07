/**
 * Josh Ibad
 * Professor: Dr. Gofman
 * CPSC 351 - 03, Fall 2020
 */

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Sums the first 100 numbers from the provided number
 * @param arg - the argument
 */ 
void* sum(void* arg)
{
	// Typecast the pointer
	/* [ANS]: "int number = (int*)arg;" => "int number = *(int*)arg;" */
	int number = *(int*)arg; 
	
	// The sum
	int sum = 0;
	
	// Compute the sum
	for(int num = number; num < number + 100; ++num)
	{
		sum += num;
	}
	
	/* [ANS]: "fprintf(stderr, "Sum = %d\n");" => "fprintf(stderr, "Sum = %d\n", sum);" */
	fprintf(stderr, "Sum = %d\n", sum); 
}

int main()
{
	// An arbitrary number to start from
	int startNum = 10;
	
	/* [ANS]: Declare the pthread_t intended to store the child thread's id */
	pthread_t id; 
	
	// Create a thread
	/* [ANS]: "pthread_create(&id, NULL, sum, startNum)" => "pthread_create(&id, NULL, sum, &startNum)" */
	if(pthread_create(&id, NULL, sum, &startNum) < 0) 
	{
		perror("pthread_create");
		exit(1);
	}
	/* [ANS]: Join the child thread */
	if( pthread_join(id, NULL) != 0){
		perror("pthread_join");
		exit(1);
	}
	
	return 0;
}