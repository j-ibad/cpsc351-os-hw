/**
 *	Program: 	CPSC 351 - HW 2
 *	Author: 	Josh Ibad
 *	References:	Dr. Gofman's Sample Codes for Threads, Shared Memory, IPC, etc.
 */
 
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "msg.h"    // For the message struct

using namespace std;

/* The size of the shared memory segment */
#define SHARED_MEMORY_CHUNK_SIZE 1000

pid_t sender_pid, recv_pid;
char * inputFileName;

/* The ids for the shared memory segment and the message queue */
int shmid;

/* The pointer to the shared memory */
void* sharedMemPtr = NULL;
void* marker = NULL;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 */
void init(int& shmid, void*& sharedMemPtr){
	sender_pid = getpid();
	/* 1. Create a file called keyfile.txt containing string "Hello world"*/
	char file_content_buffer[] = "Hello world";
	FILE* fp = fopen("keyfile.txt", "w");
	if(!fp){	//fopen error checking
		perror("fopen");
		exit(-1);
	}
	if( fwrite(file_content_buffer, 1, 12, fp) != 12 ){	//Write contents to file and error check
		perror("fwrite");
		exit(-1);
	}
	fclose(fp);
	
	int temp_shmid = 0;
	long* temp_sharedMemPtr;
	while(temp_shmid == 0){
		/* 2. Use ftok("keyfile.txt", 'a') in order to generate the key.*/
		key_t key = ftok("keyfile.txt", 'a');
		if(key < 0){
			perror("ftok");
			exit(-1);
		}
		
		/* Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE */
		temp_shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, S_IRUSR | S_IWUSR);
		if(temp_shmid < 0){
			perror("shmget");
			exit(-1);
		}
		
		/* Attach to the shared memory */
		temp_sharedMemPtr = (long*) shmat(temp_shmid, (void*)0, 0);// shmat(shmid, 0, 0);
		if(temp_sharedMemPtr < 0){
			perror("shmat");
			exit(-1);
		}
	}
	marker = temp_sharedMemPtr;
	
	//Retrieve reciever pid to send signals to
	recv_pid = (pid_t) *temp_sharedMemPtr;
	temp_sharedMemPtr++;
	
	/* Store the IDs and the pointer to the shared memory region in the corresponding function parameters */
	shmid = temp_shmid;
	sharedMemPtr = (void*) temp_sharedMemPtr; //Increment by long to preserve receiver pid in memory
}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 */
void cleanUp(const int& shmid, void* sharedMemPtr){
	shmdt(sharedMemPtr);	//Detach from shared mem
}

/**
 * The main send function
 * @param fileName - the name of the file
 * @return - the number of bytes sent
 */
unsigned long sendFile(const char* fileName){
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR2);
	
	/* The number of bytes sent */
	unsigned long numBytesSent = 0;
	
	/* Open the file */
	FILE* fp =  fopen(fileName, "r");

	/* Was the file open? */
	if(!fp){
		perror("fopen");
		exit(-1);
	}
	
	/* Read the whole file */
	while(!feof(fp)){
		long* tempPtr = (long*) sharedMemPtr;
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and
 		 * store them in shared memory.  fread() will return how many bytes it has
		 * actually read. This is important; the last chunk read may be less than
		 * SHARED_MEMORY_CHUNK_SIZE. Save the number of bytes that were actually
		 * read in numBytesSent to record how many bytes were actually send.
 		 */
		
		/* Count the number of bytes sent. */
		char file_content_buffer[SHARED_MEMORY_CHUNK_SIZE];
		numBytesSent = fread(file_content_buffer, 1, SHARED_MEMORY_CHUNK_SIZE, fp);
		memcpy(tempPtr, &numBytesSent, sizeof(long));
		tempPtr++;
		
		file_content_buffer[numBytesSent] = '\0';
		memcpy(tempPtr, file_content_buffer, numBytesSent+1);
		
		//numBytesSent = snprintf((char*) tempPtr, numBytesSent+1, "%s", file_content_buffer);  
		if( numBytesSent < 0){	//Send message
			perror("snprintf");
			exit(-1);
		}
		
		/* Send a signal toreceiver telling him that the data is ready
 		 * to be read.
 		 */
		kill(recv_pid, SIGUSR2);
		
		/* Wait till receiver sends signal that its finished saving a chunk of memory. */
		pause();
	}
	

	long tmpZero = 0;
	/** Tell the receiver that we have nothing more to send*/
	long* tempPtr = (long*) sharedMemPtr;
	memcpy(sharedMemPtr, &tmpZero, sizeof(long));
	kill(recv_pid, SIGUSR2);
	
	/* Close the file */
	fclose(fp);
	
	return numBytesSent;
}

/**
 * Used to send the name of the file to the receiver
 * @param fileName - the name of the file to send
 */
void sendFileName(const char* fileName){
	/* Get the length of the file name */
	int fileNameSize = strlen(fileName)+1; //Add 1 to include null terminator

	/* Make sure the file name does not exceed 
	 * the maximum buffer size in the fileNameMsg
	 * struct. If exceeds, then terminate with an error.
	 */
	if(fileNameSize > MAX_FILE_NAME_SIZE){
		fprintf(stderr, "File name of length %d is too long. Max file name length is %d",
			fileNameSize, MAX_FILE_NAME_SIZE);
		exit(-1);
	}
	
	long* tempPtr = (long*) sharedMemPtr;
	*tempPtr = (long) sender_pid;
	tempPtr++;
	sharedMemPtr=(void*)tempPtr;
	
	*tempPtr = fileNameSize;
	tempPtr++;
	strncpy((char*) tempPtr, fileName, fileNameSize);
	
	kill(recv_pid, SIGUSR1);	//Send signal to recv
}

void usr1Signal(int signal){
	printf("%s\n", "SENDER RECEIVED SIGUSR1");
	
	fprintf(stderr, "The number of bytes sent is %ld\n", sendFile(inputFileName)); //Send file
	
	cleanUp(shmid, sharedMemPtr);	//cleanup
	
	exit(0);
}

void usr2Signal(int signal){
	printf("%s\n", "SENDER RECEIVED SIGUSR2");
}

int main(int argc, char** argv){
	/* Check the command line arguments */
	if(argc < 2){
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}
		
	/* Connect to shared memory and the message queue */
	init(shmid, sharedMemPtr);
	
	signal(SIGUSR1, usr1Signal); 
	signal(SIGUSR2, usr2Signal); 
    
	inputFileName = argv[1];
	sendFileName(inputFileName); //Send file name
	
	for(;;){ sleep(1); }
	
	return 0;
}
