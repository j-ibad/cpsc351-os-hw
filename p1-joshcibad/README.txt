README for Assignment 1 for CPSC 351, Prof. Gofman, Fall 2020
Time-stamp: <2020-10-28 20:56:17 Josh Ibad>
DISCLAIMER: BONUS WAS ATTEMPTED
------------------------------------------------------------
	Program Name: Assignment 1 for CPSC 351, Fall 2020
	Details: A three part program. The first consists of a "shell" which takes user
	input and passes it to the system to run the command. The second consists of a 
	comparison between a serial downloader and a parallel file downloader. The third
	part consists of a parallel linear search program.
	Copyright (C) 2020  Josh Ibad

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License version 3 as 
	published by the Free Software Foundation.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	A copy of the GNU General Public License v3 is available here:
	<https://gnu.org/licenses/>
------------------------------------------------------------
	Author name: Josh Ibad
	Author email: joshcibad@csu.fullerton.edu

	Program Name: Assignment 1 for CPSC 351, Fall 2020
	Purpose: A three part program. The first consists of a "shell" which takes user
	input and passes it to the system to run the command. The second consists of a 
	comparison between a serial downloader and a parallel file downloader. The third
	part consists of a parallel linear search program.
	Programming Languages: 
	Date program began:     2020-Oct-27
	Date program completed: 2020-Oct-27

	Files in program:	shell.cpp, serial.cpp, parallel.cpp, multi-search.cpp,
		build.sh, README.txt, urls.txt, multi-search-items.txt
		Part I
		  shell.cpp = The source code file for the Part 1: Shell

		Part II
		  serial.cpp = Source code file for Part 2: Serial v Parallel File Downloading
			The serial file creates child processes one at a time to download the next
			url found in urls.txt.

		  parallel.cpp = Source code file for Part 2: Serial v Parallel File Downloading
			The parallel file creates multiple child processes immediately, each assigned
			to download a url found in urls.txt, concurrently.
		
		  urls.txt = A plaintext file consisting of a list of URIs to be downloaded by
			the Serial and Parallel File Downloaders in Part 2.	

		Part III
		  multi-search.cpp = Source code file for Bonus Part 3: Parallel Linear Search.
			The program accepts a filename from which to search, a key to search for, and
			the number of processes to run concurrently. The program then loads the file
			contents into a vector and creates the specified number of processes to
			linearly search for the specified key concurrently. Once a child processes 
			finds the key, the value is sent to the parent (using a pipe) and the other
			children are killed.
		  
		  multi-search-items.txt = An example file containing a list of random words
			on which one could test the multi-search program.

		Miscellaneous:
		  build.sh = Bash script for compiling and running all programs.
		
		  README.txt = (This file). Read me describing program, author details, etc.
	
	Status: Completed (Finished as of 2020-Oct-27). Testing on Ubuntu 20.04, g++9.3.0 success.
 
	References:
		Prof. Mikhail Gofman's Sample Code in Titanium and Lecture Notes
		
Compilation and Execution instructions:
	Part 1
	  Compile: 	g++ -o shell.out shell.cpp
	  Run:		./shell.out

	Part 2
	  Compile: 	g++ -o serial.out serial.cpp
	  Run:		time ./serial.out
	  Compile: 	g++ -o parallel.out parallel.cpp
	  Run:		time ./parallel.out
	  
	Part 3
	  Compile:	g++ -o multi-search multi-search.cpp
	  Run:		./multi-search <FILENAME> <SEARCH_KEY> <NUMBER_OF_PROCESSES>

Warnings:
	* The program was ran under tuffix distro with Ubuntu 20.04 OS and g++ v9.3.0.
	  The program is expected but not guaranteed to work for other operating systems or
	  other compilers or previous version thereof.

	* URIs in urls.txt has been changed to Ubuntu 18.04.5 since 16.04.5 urls were not working

----------------------------------------------------------------------------------------------------
Write Up:
Part 1: No Write-up

Part 2:
A.) Results
	Serial:
	  Run 1
		real    3m49.394s
		user    0m11.431s
		sys     0m37.607s
	  Run 2
		real    3m50.733s
		user    0m10.663s
		sys     0m35.735s
	  Run 3
		real    4m1.682s
		user    0m10.808s
		sys     0m36.333s
	Parallel:
	  Run 1
		real    3m53.099s
		user    0m12.285s
		sys     0m42.662s
	  Run 2
		real    3m51.222s
		user    0m13.037s
		sys     0m45.943s
	  Run 3
		real    3m56.936s
		user    0m13.000s
		sys     0m42.848s
Notes: Variation in time may have been caused by other people using the internet in the household,
or simple variations in internet usage in my very own device while running these commands

B.) Questions
	1.) 	Real time is the real human time (like a wall clock time) since start to end of execution.
		User time is only the amount of CPU time allocated to the program within the process. 
		Sys time is only the amount of CPU time spent within the kernel (aka, running system 
			related tasks), instead of library code.

	2.) For this program, sys is longer than user. This is because the majority of the program is done
	within system calls to fork, execpl, and wait.

	3.) Most of the runs resulted in the serial downloader finishing faster. I think this is because
	the serial downloading does not have as much tasks to do in terms of scheduling and orchestrating
	the running of parallel processes. The parallel downloader required the system to work more on
	switching between the two processes and making sure the right process obtains the right data and
	resources, making it run slightly slower. I do think this is only the case because we have a small
	list of 2 URLs, and only a small machine to run it on.

	4.) When ran on more files, the parallel downloader quickly catched up in speed. I think this is
	because the added overhead from orchestrating multiple processes to run concurrently immediately
	became small in comparison to the multiple large files being downloaded, allowing the parallel to
	catch up immediately in terms of speed.
	
Part 3: BONUS ATTEMPTED. No write-up.
	
