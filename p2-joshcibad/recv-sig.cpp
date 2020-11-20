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
#include <cstring>
#include <string>
#include "msg.h"    // For the message struct

using namespace std;

/* The size of the shared memory segment */
#define SHARED_MEMORY_CHUNK_SIZE 1000

pid_t sender_pid, recv_pid;
string fileName;

/* The ids for the shared memory segment and the message queue */
int shmid;

/* The pointer to the shared memory */
void *sharedMemPtr = NULL;
void *marker = NULL;

/**
 * The function for receiving the name of the file
 * @return - the name of the file received from the sender
 */
string recvFileName(){
	string fileName;	/* The file name received from the sender */
	
	long* tempPtr = (long*) sharedMemPtr;
	sender_pid = *tempPtr;
	tempPtr++;
	sharedMemPtr=(void*)tempPtr;
	
	long fileNameSize = *tempPtr;
	tempPtr++;

	char fileNameBuffer[fileNameSize];
	strncpy(fileNameBuffer, (char*) tempPtr, fileNameSize);
	
	/* Return the received file name */
	fileName = string(fileNameBuffer);
	
	kill(sender_pid, SIGUSR1);
	
	return fileName;
}

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 */
void init(int& shmid, void*& sharedMemPtr){
	recv_pid = getpid();
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
		
		/* Allocate a shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE. */
		temp_shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE,  IPC_CREAT | S_IRUSR | S_IWUSR);
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
	//Write PID of reciever program to shared memory
	*temp_sharedMemPtr = (long) recv_pid;
	temp_sharedMemPtr++; //Increment by long to preserve the pid in memory
	
	/* Store the IDs and the pointer to the shared memory region in the corresponding function parameters */
	shmid = temp_shmid;
	sharedMemPtr = (void*) temp_sharedMemPtr;
}
 

/**
 * The main loop
 * @param fileName - the name of the file received from the sender.
 * @return - the number of bytes received
 */
unsigned long mainLoop(const char* fileName){
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR2);
	/* The size of the message received from the sender */
	long msgSize = -1;
	
	/* The number of bytes received */
	long numBytesRecv = 0;
	
	/* The string representing the file name received from the sender */
	string recvFileNameStr = fileName;
	
	/* Append __recv to the end of file name */
	recvFileNameStr += "__recv";
	
	/* Open the file for writing */
	FILE* fp = fopen(recvFileNameStr.c_str(), "w");
			
	/* Error checks */
	if(!fp){
		perror("fopen");	
		exit(-1);
	}
	pause();
		
	/* Keep receiving until the sender sets the size to 0, indicating that
 	 * there is no more data to send.
 	 */	
	while(msgSize != 0){
		long* tempPtr = (long*) sharedMemPtr;
		/* TODO: Receive the message and get the value of the size field. The message will be of 
		 * of type SENDER_DATA_TYPE. That is, a message that is an instance of the message struct with 
		 * mtype field set to SENDER_DATA_TYPE (the macro SENDER_DATA_TYPE is defined in 
		 * msg.h).  If the size field of the message is not 0, then we copy that many bytes from 
		 * the shared memory segment to the file. Otherwise, if 0, then we close the file 
		 * and exit.
		 *
		 * NOTE: the received file will always be saved into the file called
		 * <ORIGINAL FILENAME__recv>. For example, if the name of the original
		 * file is song.mp3, the name of the received file is going to be song.mp3__recv.
		 */
		msgSize = (long)(*tempPtr);
		tempPtr++;
		
		/* If the sender is not telling us that we are done, then get to work */
		if(msgSize != 0){
			/* Record the number of bytes received */
			char file_content_buffer[msgSize];
			strncpy(file_content_buffer, (char*) tempPtr, msgSize);
			
			/* Save into the file the data in shared memory */
			numBytesRecv = fwrite(file_content_buffer, 1, msgSize, fp);
			
			/* Tell the sender that we are ready for the next set of bytes. */
			kill(sender_pid, SIGUSR2);
			pause();
		}else{	/* We are done */
			/* Close the file */
			kill(sender_pid, SIGUSR2);
			fclose(fp);
		}
	}
	return numBytesRecv;
}

/**
 * Performs cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 */
void cleanUp(const int& shmid, void* sharedMemPtr){
	shmdt(sharedMemPtr);	/* Detach from shared memory */
	shmctl(shmid, IPC_RMID, NULL);	//Mark shared memory for deallocation after last detachment.
}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */
void ctrlCSignal(int signal){ cleanUp(shmid, sharedMemPtr); }

void usr1Signal(int signal){
	printf("%s\n", "RECEIVER RECEIVED SIGUSR1");
	fileName = recvFileName();
	
	fprintf(stderr, "The number of bytes received is: %lu\n", mainLoop(fileName.c_str()));

	cleanUp(shmid, sharedMemPtr);	//Detach and deallocated shared mem and message queue
	exit(0);
}

void usr2Signal(int signal){
	printf("%s\n", "RECEIVER RECEIVED SIGUSR2");
	
}


int main(int argc, char** argv){
	signal(SIGINT, ctrlCSignal);		//Installs SIGINT handler
	
	init(shmid, sharedMemPtr);
	
	signal(SIGUSR1, usr1Signal); 
	signal(SIGUSR2, usr2Signal); 

	for(;;){ sleep(1); }
	return 0;
}
