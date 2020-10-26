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
#include <stdlib.h>

/* The prototypes */
void parent();
void child1();
void child2();
void child3();
void child4();
void child5();
void child6();

/**
 * The function called by the FIRST child.
 */
void child1(){
	pid_t child_b, child_a = fork();
	if (child_a < 0){ //First fork error
		fprintf(stderr, "1st Fork failed in child1() process");
		exit(-1);
	}else if (child_a == 0) { // Child A 
		child3();
		exit(0);
	} else {
		child_b = fork();
		if (child_b < 0){ //Second fork error
			fprintf(stderr, "2nd Fork failed in child1() process");
			exit(-1);
		}else if (child_b == 0) { //Child B
			child4();
			exit(0);
		} else { //Parent
			fprintf(stderr, "I am child pid=%d; my parent is pid=%d\n", getpid(), getppid());
			wait(NULL);
			wait(NULL);
			exit(0);
		}
	}
}

/**
 * The function called by the SECOND child.
 */
void child2(){
	pid_t child_b, child_a = fork();
	if (child_a < 0){ //First fork error
		fprintf(stderr, "1st Fork failed in child2() process");
		exit(-1);
	}else if (child_a == 0) { // Child A 
		child5();
		exit(0);
	} else {
		child_b = fork();
		if (child_b < 0){ //Second fork error
			fprintf(stderr, "2nd Fork failed in child2() process");
			exit(-1);
		}else if (child_b == 0) {
			child6();
			exit(0);
		} else { //Parent
			fprintf(stderr, "I am child pid=%d; my parent is pid=%d\n", getpid(), getppid());
			wait(NULL);
			wait(NULL);
			exit(0);
		}
	}
}

/**
 * The function called by the THIRD child.
 */
void child3(){
	fprintf(stderr, "I am child pid=%d; my parent is pid=%d\n", getpid(), getppid());
}

/**
 * The function called by the FOURTH child.
 */
void child4(){
	fprintf(stderr, "I am child pid=%d; my parent is pid=%d\n", getpid(), getppid());
}


/**
 * The function called by the FIFTH child.
 */
void child5(){
	fprintf(stderr, "I am child pid=%d; my parent is pid=%d\n", getpid(), getppid());
}

/**
 * The function called by the SIXTH child.
 */
void child6(){	
	fprintf(stderr, "I am child pid=%d; my parent is pid=%d\n", getpid(), getppid());
}

/**
 * The function called by the parent
 */
void parent(){
	pid_t child_b, child_a = fork();
	if (child_a < 0){ //First fork error
		fprintf(stderr, "1st Fork failed in parent() process");
		exit(-1);
	}else if (child_a == 0) { // Child A 
		child1();
		exit(0);
	} else {
		child_b = fork();
		if (child_b < 0){ //Second fork error
			fprintf(stderr, "2nd Fork failed in parent() process");
			exit(-1);
		}else if (child_b == 0) { //Child B
			child2();
			exit(0);
		} else { // Parent
			fprintf(stderr, "I am the original parent; my process ID is pid=%d\n", getpid());
			wait(NULL);
			wait(NULL);
			exit(0);
		}
	}
}


/**
 * The main function
 */
int main(){
	parent();	
}
