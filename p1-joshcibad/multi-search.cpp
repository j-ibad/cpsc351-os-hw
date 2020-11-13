/** 
 * 	Program Name: Assignment 1 for CPSC 351, Prof. Gofman, Fall 2020
 * 	Details: A three part program. The first consists of a "shell" which takes user
 *	input and passes it to the system to run the command. The second consists of a 
 *	comparison between a serial downloader and a parallel file downloader. The third
 *	part consists of a parallel linear search program.
 *  Copyright (C) 2020  Josh Ibad

 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 3 as 
 * 	published by the Free Software Foundation.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *	A copy of the GNU General Public License v3 is available here:
 *  <https://gnu.org/licenses/>
 **/

/*
 * Author name: Josh Ibad
 * Author email: joshcibad@csu.fullerton.edu
 * 
 * Program name: Assignment 1 for CPSC 351, Prof. Gofman, Fall 2020
 * Programming Languages: Four modules in C++
 * Date program began:     2020-Oct-28
 * Date program completed: 2020-Oct-28
 * Files in program:	shell.cpp, serial.cpp, parallel.cpp, multi-search.cpp
 * Status: Completed (Finished as of 2020-Oct-28). Testing on Ubuntu 20.04, g++9.3.0 success.
 *
 * References:
 *	Prof. Mikhail Gofman's Starter Code and Lecture Notes
 *
 * Purpose:
 *	The program accepts a filename from which to search, a key to search for, and
 *	the number of processes to run concurrently. The program then loads the file
 *	contents into a vector and creates the specified number of processes to
 *	linearly search for the specified key concurrently. Once a child processes 
 *	finds the key, the value is sent to the parent (using a pipe) and the other
 *	children are killed.
 *
 * This file:
 *	Filename: multi-search.cpp
 *	Language: C++
 *	Compile: g++ -o multi-search multi-search.cpp
 *	Run: ./multi-search <FILENAME> <SEARCH_KEY> <NUMBER_OF_PROCESSES>
 */

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int pipefd[2];

int partial_linear_search(std::vector<std::string> vec, std::string key, int beg, int end){
	for(int i=beg; i<end && i<vec.size(); i++){
		if(vec[i] == key){
			return i;
		}
	}
	return -1;
}
/**
 * The main function
 */
int main(int argc, char* argsv[]){
	/* Error check input first */
	if(argc != 4){ //Should have 3 arguments + 1 being the command itself
		printf("%s%s %s\n", "Usage: ", argsv[0], "<FILE_NAME> <KEY> <NUMBER OF PROCESSES>"); 
		return 1;
	}
	/* Load input into properly named variables */
	char * file_name = argsv[1];
	std::string search_key = std::string(argsv[2]);
	int N = atoi(argsv[3]);
	if (N < 0) {
		printf("%s", "Program could not read integer, or the number of processes requested was invalid");
		return 1;
	}
	
	/* Create pipe for children to return resulting index to */
	if(pipe(pipefd) < 0){
		perror("pipe");
		exit(-1);
	}
	
	/* Open file */
	std::ifstream file;
	file.open( file_name );
	if( file ){
		/* Load file contents to a vector */
		std::string buffer;
		std::vector<std::string> vec;
		while (getline(file, buffer)){
			vec.emplace_back(buffer); //use emplace instead of push to do deep copy, due to reuse of buffer address
		}
		
		/* Repeat contents to confirm contents with user */
		std::cout << "Contents of list:" << std::endl;
		int len = vec.size();
		for(int i=0; i<len; i++) {
			printf("[%d] %s\n", i, vec[i].c_str());
		}
		std::cout << "\t***End of contents of list***" << std::endl << std::endl;
		
		int width = len / N;
		pid_t pid, child_pid[N];
		for(int i=0; i<N; i++){
			pid = fork();
			if(pid < 0){
				perror("fork");
				exit(-1);
			}else if(pid == 0){
				int search_index = partial_linear_search(vec, search_key, width * i, width * (i+1));
				if(search_index == -1){
					exit(1);
				}else{
					close(pipefd[0]);
					if(write(pipefd[1], &search_index, sizeof(search_index)) < 0){
						perror("write");
						exit(-1);
					}
					close(pipefd[1]);
					exit(0);
				}
			}else{
				child_pid[i] = pid;
			}
		}
		if(pid > 0){ //The parent
			int status;
			for(int i=0; i<N; i++){
				if(wait(&status) < 1){ perror("wait"); exit(-1);}
				if(WEXITSTATUS(status) == 0){
					/* Read shared memory for index */
					int retVal;
					close(pipefd[1]);
					if(read(pipefd[0], &retVal, sizeof(retVal)) < 0){
						perror("read");
						exit(-1);
					}
					close(pipefd[0]);
					
					/* Print results and kill remaining children */
					printf("Key found at index %d\n", retVal);
					for(int j=0; j<N; j++){
						kill(child_pid[j], SIGKILL);
					}
					return 0;
				}
			}
			std::cout << "Key not found" << std::endl;
			return 0;
		}
		
	}else{
		std::cerr << "Failed to open " << file_name <<
			" or it might be empty." << std::endl;
		return 1;
	}
}
