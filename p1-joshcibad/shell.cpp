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
 *	Acts a shell, accepting commands (no arguments), and forks a child process to execute the
 *	command, with the parent waiting for its termination. User can type help for a simple
 *	help messag,e and must enter "exit" to exit the shell program.
 *
 * This file:
 *	Filename: shell.cpp
 *	Language: C++
 *	Compile: g++ -o shell.out shell.cpp
 *	Run: ./shell.out
 */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

/**
 * The main function
 */
int main(){
	string cmdBuff; //Command buffer
	pid_t pid; // ID of child process
	do{ //Keep running until the user has typed "exit"
		/* Prompt the user to enter the command */
		cerr << "cmd>";
		cin >> cmdBuff;
		
		if (cmdBuff == "help" || cmdBuff == "?"){ //Help message
			cerr << "Enter a command and press ENTER to run it." << endl <<
				"Type exit and press ENTER to exit the shell" << endl;
		}else if(cmdBuff != "exit" || cmdBuff == ""){//If the user wants to exit
			pid_t pid = fork();		//Create a child
			if (pid < 0){ 			// Error check if child successfully created
				perror("fork");
				exit(-1);
			}else if (pid == 0) { //Child B
				if(execlp(cmdBuff.c_str(), cmdBuff.c_str(), 0) == -1){
					perror(cmdBuff.c_str());
					exit(-1);
				}else{
					exit(0);
				}
			} else { // Parent
				if(wait(NULL) == -1){// Wait for the child process to terminate
					perror("wait");
					exit(-1);
				}
			}
		}
	}while(cmdBuff != "exit");
	return 0;	
}
