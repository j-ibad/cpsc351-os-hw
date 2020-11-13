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
 *	The serial file creates child processes one at a time to download the next
 *	url found in urls.txt.
 *
 * This file:
 *	Filename: serial.cpp
 *	Language: C++
 *	Compile: g++ -o serial.out serial.cpp
 *	Run: time ./serial.out
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
	return 0;
}
