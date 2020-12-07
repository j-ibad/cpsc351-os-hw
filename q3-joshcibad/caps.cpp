/**
 * Josh Ibad
 * Professor: Dr. Gofman
 * CPSC 351 - 03, Fall 2020
 */
 
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char** argv){
	/* 1. Check arguments */
	if(argc < 2){
		fprintf(stderr, "USAGE: %s <string>\n", argv[0]);
		exit(1);
	}
	
	int str_size = strlen(argv[1])+1;
	char* recv_str = (char*) calloc(str_size, sizeof(char));
	if( recv_str == NULL ){
		perror("calloc");
		exit(1);
	}
	
	/* 2a. Create two pipes and a child process */
	int pipe_p[2], pipe_c[2];
	pid_t pid;
	/* 2a. Create two pipes */
	if( pipe(pipe_p) < 0 ){
		perror("pipe");
		exit(1);
	}
	if( pipe(pipe_c) < 0 ){
		perror("pipe");
		exit(1);
	}
	/* 2b. Fork a child process */
	pid = fork();
	
	
	if(pid < 0){ //Error
		perror("fork");
		exit(1);
	}else if(pid > 0){ //Parent
		/* 2c. Parent writes to pipe_p, and reads from pipe_c,
			close pipe_p read end and pipe_c write end */
		close( pipe_p[0] ); //Close parent pipe read end
		close( pipe_c[1] ); //Close child pipe write end
		
		/* 3. Send user input string to child */
		if( write(pipe_p[1], argv[1], str_size) < 0 ){
			perror("write");
			exit(1);
		}
		
		/* 5. Receive string and print it */
		if( read(pipe_c[0], recv_str, str_size) < 0){
			perror("read");
			exit(1);
		}
		fprintf(stderr, "[Parent] Got string: %s\n", recv_str);
		
		/* Close up remaining pipe ends */
		close( pipe_p[1] );
		close( pipe_c[0] );
	}else{ //Child
		/* 2c. Child reads from pipe_p, and writes to pipe_c,
			close pipe_p write end and pipe_c read end */
		close( pipe_p[1] ); //Close parent pipe write end
		close( pipe_c[0] ); //Close child pipe read end
		
		/* 4. Receive string, capitalize it, and send back. */
		/* 4a. Receive string */
		if( read(pipe_p[0], recv_str, str_size) < 0){
			perror("read");
			exit(1);
		}
		/* 4b. Capitalize string */
		for( int str_i=0; str_i<str_size; str_i++ ){
			recv_str[str_i] = toupper( recv_str[str_i] );
		}
		/* 4c. Send back capitalized string */
		if( write(pipe_c[1], recv_str, str_size) < 0 ){
			perror("write");
			exit(1);
		}
		
		/* Close up remaining pipe ends */
		close( pipe_p[0] );
		close( pipe_c[1] );
	}
	
	/* Clean up resources and return */
	free( recv_str );
	return(0);
}