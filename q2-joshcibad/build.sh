#!/bin/bash 

#Program: "Bubble Sorting an Array of Integers" (HW 3 for CPSC 240-03, Fall 2020)
#Author: Josh Ibad

#Delete some un-needed files
rm *.o
rm *.out

echo "The script file for Program \"Bubble Sorting an Array of Integers\" has begun"

#Compilation
echo "Compile processes.cpp"
g++ -o main.out processes.cpp

# Run program
echo "Run the program \"Bubble Sorting an Array of Integers\":"
echo " "	#Blank line merely for appearances
./main.out

echo "The script file will terminate"
