#!/bin/bash

# Author: Josh Ibad
# Program: CPSC 351 - HW 2 - IPC

echo "Removing old files if necessary"
rm *.out
rm *.o

echo ""
echo "Starting CPSC 351: HW 2 - By Josh Ibad"
echo ""

#Part I
echo "Part I: Receiver and Sender w/ Message Queues"
./build-reg.sh

read -n 1 -s -r -p "Press any key to continue..."

echo ""
echo ""
echo "BONUS: Receiver and Sender w/ Signals"
./build-sig.sh

read -n 1 -s -r -p "Finished. Press any key to exit..."
echo "End of program"
