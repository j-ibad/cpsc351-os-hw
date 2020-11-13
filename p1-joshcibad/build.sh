#!/bin/bash

# Author: Josh Ibad
# Program: CPSC 351 - Quiz 2 - Forking Processes

echo "Removing old files if necessary"
rm *.out

#Part I
echo "Part I: Shell"
echo "Compiling shell.cpp"
g++ -o shell.out shell.cpp

echo "Running program"
./shell.out

echo ""
echo "   *** Part I Finished ***"
read -p "Press enter to continue"

#Part II
echo ""
echo "  Part II: Downloader"
echo "Compiling serial.cpp"
g++ -o serial.out serial.cpp

echo "Compiling parallel.cpp"
g++ -o parallel.out parallel.cpp

echo "Running program"
echo "Running and timing serial downloader"
time ./serial.out

rm ubuntu-* 2> /dev/null

echo "Running and timing parallel downloader"
time ./parallel.out

echo ""
echo "   *** Part II Finished ***"
read -p "Press enter to continue"

#Part III
echo "BONUS - Part III: Multi-Search"
ret=1
g++ -o multi-search multi-search.cpp
ret=$?
if [ $ret == "0" ]; then
	echo "Program compiled"
	echo "Run the command: ./multi-search <FILENAME> <SEARCH_KEY> <NUMBER OF PROCESSES>"
	echo "The multi-search-items.txt file has been provided for example, select a word within for testing or supply your own file and search key"
else
	echo "The program failed to compile. This may be due to formatting errors, or version errors. This code runs on Ubuntu 20.4, g++ v9.3.0 and later. It is not guaranteed to work on older versions."
fi

