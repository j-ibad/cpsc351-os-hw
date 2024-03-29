./p2-joshcibad/build-reg.sh                                                                         0000777 0001750 0001750 00000001402 13755363501 016434  0                                                                                                    ustar   herreisen                       herreisen                                                                                                                                                                                                              #!/bin/bash

rm *.o
rm testinput.txt

g++ -o recv.o recv.cpp
g++ -o sender.o sender.cpp

touch testinput.txt
echo "secret text" >> testinput.txt

echo "Contents of test testinput.txt is: "
cat testinput.txt
echo ""


echo "Starting the receiver program"
./recv.o &

sleep 3s #let receiver output in peace

echo "" #blank line

echo "Waiting to make sure that receiver is initialized before starting sender"
echo "Starting sender program in ..."

i=5
while [ $i -ge 1 ]
do
	sleep 1s
	echo "$i"
	i=$(( $i - 1 ))
done

echo "Starting the sender program."
echo "Sender program will take in input from testinput.txt"
./sender.o testinput.txt

sleep 5s

echo ""
echo "Contents of testinput.txt__recv is: "
cat testinput.txt__recv

echo ""
echo "[=====End of execution=====]"

                                                                                                                                                                                                                                                              ./p2-joshcibad/build-sig.sh                                                                         0000777 0001750 0001750 00000001432 13755540550 016445  0                                                                                                    ustar   herreisen                       herreisen                                                                                                                                                                                                              #!/bin/bash

rm *.o
rm testinput.txt

g++ -o recv-sig.o recv-sig.cpp
g++ -o sender-sig.o sender-sig.cpp

touch testinput.txt
echo "secret text" >> testinput.txt

echo "Contents of test testinput.txt is: "
cat testinput.txt
echo ""


echo "Starting the receiver program"
./recv-sig.o &

sleep 3s #let receiver output in peace

echo "" #blank line

echo "Waiting to make sure that receiver is initialized before starting sender"
echo "Starting sender program in ..."

i=5
while [ $i -ge 1 ]
do
	sleep 1s
	echo "$i"
	i=$(( $i - 1 ))
done

echo "Starting the sender program."
echo "Sender program will take in input from testinput.txt"
./sender-sig.o testinput.txt

sleep 5s

echo ""
echo "Contents of testinput.txt__recv is: "
cat testinput.txt__recv

echo ""
echo "[=====End of execution=====]"

                                                                                                                                                                                                                                      ./p2-joshcibad/build.sh                                                                             0000777 0001750 0001750 00000000725 13755615734 015700  0                                                                                                    ustar   herreisen                       herreisen                                                                                                                                                                                                              #!/bin/bash

# Author: Josh Ibad
# Program: CPSC 351 - HW 2 - IPC

echo "Removing old files if necessary"
rm *.out
rm *.o

echo ""
echo "Starting CPSC 351: HW 2 - By Josh Ibad"
echo ""

#Part I
echo "Part I: Receiver and Sender w/ Message Queues"
./build-reg.sh

read -n 1 -s -r -p "Press any key to continue..."

echo ""
echo ""
echo "BONUS: Receiver and Sender w/ Signals"
./build-sig.sh

read -n 1 -s -r -p "Finished. Press any key to exit..."
echo "End of program"
                                           ./p2-joshcibad/keyfile.txt                                                                          0000777 0001750 0001750 00000000014 13755615775 016432  0                                                                                                    ustar   herreisen                       herreisen                                                                                                                                                                                                              Hello world                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     ./p2-joshcibad/msg.h                                                                                0000777 0001750 0001750 00000002714 13755027010 015164  0                                                                                                    ustar   herreisen                       herreisen                                                                                                                                                                                                              #define MAX_MSG_PAYLOAD 100

/* The information type */
#define SENDER_DATA_TYPE 1

/* The done message */
#define RECV_DONE_TYPE 2

/* The file name transfer message */
#define FILE_NAME_TRANSFER_TYPE 3

/* The maximum size of the file name */
#define MAX_FILE_NAME_SIZE 100

/**
 * The structure representing the message
 * used by sender to send the name of the file
 * to be transfered to the receiver.
 */
struct fileNameMsg{	
	/* The message type */
	long mtype;	
	
	/* The name of the file */
	char fileName[MAX_FILE_NAME_SIZE];
	
	/**
 	 * Prints the structure
 	 * @param fp - the file stream to print to
 	 */
	void print(FILE* fp){
		fprintf(fp, "%ld %s\n", mtype, fileName);
	}
};

/**
 * The message structure representing the message
 * sent from the sender to the receiver indicating
 * the number of bytes in the shared memory segment 
 * that are ready to read. 
 */

struct message{
	/* The message type */
	long mtype;
	
	/* How many bytes in the message */
	int size;
	
	/**
 	 * Prints the structure
 	 * @param fp - the file stream to print to
 	 */
	void print(FILE* fp){
		fprintf(fp, "%ld %d", mtype, size);
	}
};

/* Struct representing the message sent from the receiver
 * to the sender acknowledging the successful reception and
 * saving of data.
 */
struct ackMessage{
	/* The type of message */
	long mtype;
	
	/**
 	 * Prints the structure
 	 * @param fp - the file stream to print to
 	 */
	void print(FILE* fp){
		fprintf(fp, "%ld\n", mtype);
	}
};
                                                    ./p2-joshcibad/recv-sig.cpp                                                                         0000777 0001750 0001750 00000014435 13755614751 016471  0                                                                                                    ustar   herreisen                       herreisen                                                                                                                                                                                                              /**
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
                                                                                                                                                                                                                                   ./p2-joshcibad/recv.cpp                                                                             0000777 0001750 0001750 00000015476 13755610276 015715  0                                                                                                    ustar   herreisen                       herreisen                                                                                                                                                                                                              /**
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
                                                                                                                                                                                                  ./p2-joshcibad/sender-sig.cpp                                                                       0000777 0001750 0001750 00000014057 13755614737 017016  0                                                                                                    ustar   herreisen                       herreisen                                                                                                                                                                                                              /**
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 ./p2-joshcibad/sender.cpp                                                                           0000777 0001750 0001750 00000014522 13755610163 016220  0                                                                                                    ustar   herreisen                       herreisen                                                                                                                                                                                                              /**
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
                                                                                                                                                                              ./p2-joshcibad/testinput.txt                                                                        0000777 0001750 0001750 00000000014 13755615765 017040  0                                                                                                    ustar   herreisen                       herreisen                                                                                                                                                                                                              secret text
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    