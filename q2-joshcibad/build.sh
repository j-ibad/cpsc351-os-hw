#!/bin/bash

# Author: Josh Ibad
# Program: CPSC 351 - Quiz 2 - Forking Processes

echo "Removing old files if necessary"
rm ./main.out

echo "Compiling processes.cpp"
g++ -o main.out processes.cpp

echo "Running program"
./main.out
