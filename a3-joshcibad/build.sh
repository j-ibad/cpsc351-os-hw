#!/bin/bash

ret=1
g++ -o threads threads.cpp -pthread

ret=$?
if [ $ret == "0" ]; then
	echo "Program compiled"
	echo "Run w/ command: "
	echo "./threads <numberOfThreads>"
else
	echo "The program failed to compile. This may be due to formatting errors, or version errors. This code runs on Ubuntu 20.4, g++ v9.3.0 and later. It is not guaranteed to work on older versions."
fi