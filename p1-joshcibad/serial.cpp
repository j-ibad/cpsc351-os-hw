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
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>

/**
 * The main function
 */
int main(){
	std::string urlBuffer;
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
				if(execlp("/usr/bin/wget", "wget", urlBuffer, NULL) == -1){
					std::cerr << "Failed to download " << urlBuffer << std::endl;
					exit(-1);
				}else{
					exit(0);
				}
			}else{
				if(wait(NULL) == -1){// Wait for the child process to terminate
					std::cerr << "Error encountered while waiting." << std::endl;
					exit(-1);
				}
			}
			linecount++;
		}
	}else{
		std::cerr << "File urls.txt not found. Terminating..." << std::endl;
		exit(-1);
	}
	return 0;
}
