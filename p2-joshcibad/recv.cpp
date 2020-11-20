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

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr = NULL;

/**
 * The function for receiving the name of the file
 * @return - the name of the file received from the sender
 */
string recvFileName(){
	string fileName;	/* The file name received from the sender */
        
	/* Declare an instance of the fileNameMsg struct to be
	 * used for holding the message received from the sender.
	 */
	fileNameMsg msg;
	
	/* Receive the file name using msgrcv() */
	if( msgrcv(msqid, &msg, sizeof(fileNameMsg) - sizeof(long), FILE_NAME_TRANSFER_TYPE, 0) < 0){
		perror("msgrcv");
		exit(-1);
	}
	/* Return the received file name */
	fileName = string(msg.fileName);
	return fileName;
}

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
	
	/* Allocate a shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE. */
	int temp_shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE,  IPC_CREAT | S_IRUSR | S_IWUSR);
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
	int temp_msqid = msgget(key,  IPC_CREAT | S_IRUSR | S_IWUSR);
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
 * The main loop
 * @param fileName - the name of the file received from the sender.
 * @return - the number of bytes received
 */
unsigned long mainLoop(const char* fileName){
	/* The size of the message received from the sender */
	int msgSize = -1;
	
	/* The number of bytes received */
	int numBytesRecv = 0;
	
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
		
	/* Keep receiving until the sender sets the size to 0, indicating that
 	 * there is no more data to send.
 	 */	
	while(msgSize != 0){
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
		message msg;
		/* Receive the file name using msgrcv() */
		if( msgrcv(msqid, &msg, sizeof(message) - sizeof(long), SENDER_DATA_TYPE, 0) < 0){
			perror("msgrcv");
			exit(-1);
		}
		msgSize = msg.size;
		
		/* If the sender is not telling us that we are done, then get to work */
		if(msgSize != 0){
			/* Record the number of bytes received */
			char file_content_buffer[msgSize];
			strncpy(file_content_buffer, (char*) sharedMemPtr, msgSize);
			
			/* Save into the file the data in shared memory (that was put there
			 *  by the sender) 
			 */
			numBytesRecv = fwrite(file_content_buffer, 1, msgSize, fp);
			
			/* Tell the sender that we are ready for the next set of bytes. 
 			 * I.e., send a message of type RECV_DONE_TYPE. That is, a message
			 * of type ackMessage with mtype field set to RECV_DONE_TYPE. 
 			 */
			ackMessage ack = {RECV_DONE_TYPE};
			if( msgsnd(msqid, &ack, sizeof(ackMessage) - sizeof(long), 0) < 0){
				printf("ERROR in recv");
				perror("msgrcv");
				exit(-1);
			}
		}else{	/* We are done */
			/* Close the file */
			fclose(fp);
		}
	}
	return numBytesRecv;
}

/**
 * Performs cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */
void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr){
	/* Detach from shared memory */
	if( shmdt(sharedMemPtr) < 0){
		perror("shmdt");
		exit(-1);
	}
	
	if( shmctl(shmid, IPC_RMID, NULL) < 0){		//Mark shared memory for deallocation after last detachment.
		perror("shmctl");
		exit(-1);
	}
	
	/* Deallocate the message queue */
	if( msgctl(msqid, IPC_RMID, NULL) < 0){
		perror("msgctl");
		exit(-1);
	}
}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */
void ctrlCSignal(int signal){
	/* TODO: Your program should delete the message
 	 * queue and the shared memory segment before exiting. You may add 
	 * the cleaning functionality in ctrlCSignal().
 	 */
	 
	/* Free system V resources */
	cleanUp(shmid, msqid, sharedMemPtr);
}

int main(int argc, char** argv){
	signal(SIGINT, ctrlCSignal);		//Installs SIGINT handler
	init(shmid, msqid, sharedMemPtr);
	
	string fileName = recvFileName();
	
	/* Go to the main loop */
	fprintf(stderr, "The number of bytes received is: %lu\n", mainLoop(fileName.c_str()));

	cleanUp(shmid, msqid, sharedMemPtr);	//Detach and deallocated shared mem and message queue
	
	return 0;
}
