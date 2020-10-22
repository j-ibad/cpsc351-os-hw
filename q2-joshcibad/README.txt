README for Forking Processes (Quiz 2 for CPSC 351, Prof. Gofman, Fall 2020)
Time-stamp: <2020-10-21 17:45:00 Josh Ibad>
------------------------------------------------------------
	Program Name: "Forking Processes" (Quiz 2 for CPSC 351, Fall 2020)
	Details: Quiz 2, using fork and wait to properly create a multi-process program.
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

	Program Name: "Forking Processes"
	Purpose: Proof-of-concept program where a parent process forks into two children 
	  processes, both of which fork into two other children processes. All processes 
	  display their pid, and all children processes display their parent's pid. The 
	  parents must wait for their children to terminate before terminating itself.
	Programming Languages: One module in C++
	Date program began:     2020-Oct-21
	Date program completed: 2020-Oct-21

	Files in program:	processes.cpp
		
		processes.c = One and only module for Quiz 2, written in C++, using fork and
		  wait to create a multi-processing program properly.
		
		r.sh = Bash script for assembling, compiling, and linking the program
		
		README.txt = (This file). Read me describing program, author details, etc.
	
	Status: Completed (Finished as of 2020-Oct-21). Testing on Ubuntu 20.04, g++9.3.0 success.
 
	References:
		Prof. Mikhail Gofman's Quiz 2 Starter Code: "skeleton.cpp",
			and Example Code: "fork.c"
		
Compilation and Execution instructions:
	Compile: 	g++ -o main.out processes.cpp
	Run:		./main.out

Warnings:
	* The program was ran under tuffix distro with Ubuntu 20.04 OS and g++ v9.3.0.
	  The program is expected but not guaranteed to work for other operating systems or
	  other compilers or previous version thereof.