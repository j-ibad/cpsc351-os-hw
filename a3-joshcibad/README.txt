README for Assignment 3 for CPSC 351, Prof. Gofman, Fall 2020
Time-stamp: <2020-12-07 12:04:17 Josh Ibad>
------------------------------------------------------------
Author name: Josh Ibad
Author email: joshcibad@csu.fullerton.edu

Program Name: Assignment 3 for CPSC 351, Fall 2020
Purpose:  A multithreaded program that creates the specified number of threads,
each thread creating 100 random numbers between 0 and 100,000 and placing it
in a shared binary search tree, in a synchronized fashion (using mutexes).
Programming Languages: 
Date program began:     2020-Dec-12
Date program completed: 2020-Dec-12

	Files in program:	README.txt, build.sh, BinaryTree.hpp, thread.cpp
		Part I
		  threads.cpp = Main source code file for the program. Checks the arguments,
		  creates the specified number of threads. Each thread runs a function which
		  generates a random number [0, 100000], acquires a mutex lock for the BST,
		  adds the random number to the BST, and releases the mutex lock. 100 numbers
		  are generated and added by each thread. The parent thread waits for the
		  execution of all child threads and then prints the BST contents in-order.
		  
		  BinaryTree.hpp = Header (and implementation) file of the Binary Search Tree
		  custom made for the program. The BST is unbalanced and only implements two 
		  interfacable methods (add & print) due to assignment needs.
		  
		Miscellaneous:
		  build.sh = Bash script for compiling the program, and prints the instructions
		  for running the executable on the command line.
		
		  README.txt = (This file). Read me describing program, author details, etc.
	
	Status: Completed (Finished as of 2020-Dec-12). Testing on Ubuntu 20.04, g++9.3.0 success.
 
	References:
		Prof. Mikhail Gofman's Sample Code in Titanium and Lecture Notes
		Random number generator: https://www.cplusplus.com/reference/cstdlib/rand/
		
Compilation and Execution instructions:
	Part I
	  Compile:	g++ -o threads threads.cpp -pthreads
	  Run:		./threads <NUMBER OF THREADS>

Warnings:
	* The program was ran under tuffix distro with Ubuntu 20.04 OS and g++ v9.3.0.
	  The program is expected but not guaranteed to work for other operating systems or
	  other compilers or previous version thereof.

	* For clarity, the results are printed to stderr, with one number per line.
		* Tabs were considered, but ended up outputting the numbers broken up amongst
		lines, which made it seem like it was done wrong.
		* This may require a scrollable and buffered terminal to scroll up to inspect
		the output. If your terminal is not scrollable, please redirect the output
		to a file and inspect said file.
