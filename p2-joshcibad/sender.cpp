/**
 *	Program: 	CPSC 351 - HW 2
 *	Author: 	Josh Ibad
 *	References:	Dr. Gofman's Sample Codes for Threads, Shared Memory, IPC, etc.
 */

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "msg.h"    // For the message struct

using namespace std;

/* The size of the shared memory segment */
#define SHARED_MEMORY_CHUNK_SIZE 1000
/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void* sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the allocated message queue
 */
void init(int& shmid, int& msqid, void*& sharedMemPtr){
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
	
	/* 2. Use ftok("keyfile.txt", 'a') in order to generate the key.*/
	key_t key = ftok("keyfile.txt", 'a');
	if(key < 0){
		perror("ftok");
		exit(-1);
	}
	
	/* Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE */
	int temp_shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, S_IRUSR | S_IWUSR);
	if(temp_shmid < 0){
		perror("shmget");
		exit(-1);
	}
	
	/* Attach to the shared memory */
	void* temp_sharedMemPtr = shmat(temp_shmid, (void*)0, 0);// shmat(shmid, 0, 0);
	if(temp_sharedMemPtr < 0){
		perror("shmat");
		exit(-1);
	}
	
	/* Attach to the message queue */
	int temp_msqid = msgget(key,  S_IRUSR | S_IWUSR);
	if(temp_msqid < 0){
		perror("msgget");
		exit(-1);
	}
	
	/* Store the IDs and the pointer to the shared memory region in the corresponding function parameters */
	shmid = temp_shmid;
	sharedMemPtr = temp_sharedMemPtr;
	msqid = temp_msqid;
}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */
void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr){
	if( shmdt(sharedMemPtr) < 0 ){ //Detach from shared mem
		perror("shmdt");
		exit(-1);
	}
}

/**
 * The main send function
 * @param fileName - the name of the file
 * @return - the number of bytes sent
 */
unsigned long sendFile(const char* fileName){
	/* A buffer to store message we will send to the receiver. */
	message sndMsg; 
	
	/* A buffer to store message received from the receiver. */
	ackMessage rcvMsg;
		
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
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and
 		 * store them in shared memory.  fread() will return how many bytes it has
		 * actually read. This is important; the last chunk read may be less than
		 * SHARED_MEMORY_CHUNK_SIZE. Save the number of bytes that were actually
		 * read in numBytesSent to record how many bytes were actually send.
 		 */
		
		/* Count the number of bytes sent. */
		char file_content_buffer[SHARED_MEMORY_CHUNK_SIZE];
		numBytesSent = fread(file_content_buffer, 1, SHARED_MEMORY_CHUNK_SIZE, fp);
		file_content_buffer[numBytesSent] = '\0';
		numBytesSent = snprintf((char*) sharedMemPtr, numBytesSent+1, "%s", file_content_buffer);  
		if( numBytesSent < 0){	//Send message
			perror("snprintf");
			exit(-1);
		}
		
		/* Send a message to the receiver telling him that the data is ready
 		 * to be read (message of type SENDER_DATA_TYPE).
 		 */
		sndMsg.mtype = SENDER_DATA_TYPE;
		sndMsg.size = numBytesSent;
		if( msgsnd(msqid, &sndMsg, sizeof(message) - sizeof(long), 0) < 0){	//Send message
			perror("msgsnd");
			exit(-1);
		}
		
		/* Wait until the receiver sends us a message of type RECV_DONE_TYPE telling us 
 		 * that he finished saving a chunk of memory. 
 		 */
		if( msgrcv(msqid, &rcvMsg, sizeof(ackMessage) - sizeof(long), RECV_DONE_TYPE, 0) < 0){
			perror("msgsnd");
			exit(-1);
		}
	}
	

	/** Tell the receiver that we have nothing more to send. We will do this by
 	  * sending a message of type SENDER_DATA_TYPE with size field set to 0. 	
	  */
	sndMsg.mtype = SENDER_DATA_TYPE;
	sndMsg.size = 0;
	if( msgsnd(msqid, &sndMsg, sizeof(message) - sizeof(long), 0) < 0){	//Send message
		perror("msgsnd");
		exit(-1);
	}
		
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

	/* Create an instance of the struct representing the message
	 * containing the name of the file.
	 */
	fileNameMsg msg;
	msg.mtype = FILE_NAME_TRANSFER_TYPE;	/* Set the message type FILE_NAME_TRANSFER_TYPE */
	strncpy(msg.fileName, fileName, fileNameSize);	/* Set the file name in the message */
	
	/* Send the message using msgsnd */
	if( msgsnd(msqid, &msg, sizeof(fileNameMsg) - sizeof(long), 0) < 0){
		perror("msgsend");
		exit(-1);
	}
}


int main(int argc, char** argv){
	/* Check the command line arguments */
	if(argc < 2){
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}
		
	/* Connect to shared memory and the message queue */
	init(shmid, msqid, sharedMemPtr);
	
    sendFileName(argv[1]); //Send file name
	
	fprintf(stderr, "The number of bytes sent is %lu\n", sendFile(argv[1])); //Send file
	
	cleanUp(shmid, msqid, sharedMemPtr);	//cleanup
	return 0;
}
