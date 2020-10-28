/**
 * 	Program Name: "Forking Processes" (HW 1 Pt. 1 for CPSC 351, Fall 2020)
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
				/*** TODO: If I am child, I will do this: ****/
				/* Call execlp() to replace my program with that specified at the command line.
				 * PLEASE NOTE: YOU CANNOT PASS cmdBuff DIRECTLY to execlp(). It is because 
				 * cmdBuff is an object of type string (i.e., a class) and execlp() expects
				 * an array of characters.  However, you can pass cmdBuff.c_str(), which will
				 * return an array of characters representation of the string object.
				 * 
				 * Also, please do not forget to error check your exelp() system calls.
				 */
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
