#!/bin/bash

# Author: Josh Ibad
# Program: CPSC 351 - Quiz 2 - Forking Processes

echo "Removing old files if necessary"
rm *.out

echo "Part I: Shell"
echo "Compiling shell.cpp"
g++ -o shell.out shell.cpp

echo "Running program"
./shell.out

echo "\nPart II: Downloader"
echo "Compiling serial.cpp"
g++ -o serial.out serial.cpp

echo "Compiling parallel.cpp"
g++ -o parallel.out parallel.cpp

echo "Running program\n"
echo "Running and timing serial downloader"
time ./serial.out
echo "Running and timing parallel downloader"
time ./parallel.out

echo "Finished."
