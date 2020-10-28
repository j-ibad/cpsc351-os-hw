/** 
 * 	Program Name: "Forking Processes" (Quiz 2 for CPSC 351, Fall 2020)
 * 	Details: Quiz 2, using fork and wait to properly create a multi-process program.
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
 * Program name: "Forking Processes"
 * Programming Languages: One module in C++
 * Date program began:     2020-Oct-21
 * Date program completed: 2020-Oct-21
 * Files in program:	processes.cpp
 * Status: Completed (Finished as of 2020-Oct-21). Testing on Ubuntu 20.04, g++9.3.0 success.
 *
 * References:
 *	Prof. Mikhail Gofman's Quiz 2 Starter Code: "skeleton.cpp" and Example Code: "fork.c"
 *
 * Purpose:
 *	Proof-of-concept program where a parent process forks into two children processes, 
 *	both of which fork into two other children processes. All processes display their
 *	pid, and all children processes display their parent's pid. The parents must wait
 *  for their children to terminate before terminating itself.
 *
 * This file:
 *	Filename: processes.cpp
 *	Language: C++
 *	Compile: g++ -o main.out processes.cpp
 *	Run: ./main.out
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
		for( int i=0; i<len; i++) {std::cout << vec[i] << std::endl;}
		std::cout << std::endl;
		
		int retVal = -1;
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
					retVal = search_index;
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
					printf("Key found at index %d\n", retVal);
					for(int j=0; j<N; j++){
						std::cout << "[Child " << j << "] "<< child_pid[j] << std::endl;
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
	
	
	/*std::string urlBuffer;
	std::ifstream file;
	file.open("urls.txt");
	//file.open("urls.txt");
	if( file ){
		int linecount = 0;
		pid_t pid;
		while( getline(file, urlBuffer) ){
			pid = fork();		//Create a child
			if (pid < 0){ 			// Error check if child successfully created
				perror("fork");
				exit(-1);
			}else if (pid == 0) { 	//Child B
				if(execlp("/usr/bin/wget", "wget", urlBuffer.c_str(), NULL) == -1){
					std::cerr << "Failed to download " << urlBuffer << std::endl;
					perror("wget");
					exit(-1);
				}else{
					exit(0);
				}
			}else{
				if(wait(NULL) == -1){// Wait for the child process to terminate
					std::cerr << "Error encountered while waiting." << std::endl;
					perror("wait");
					exit(-1);
				}
			}
			linecount++;
		}
	}else{
		std::cerr << "File urls.txt not found. Terminating..." << std::endl;
		exit(-1);
	}
	return 0;*/
}
