#!/bin/bash

rm *.o
rm testinput.txt

g++ -o recv-sig.o recv-sig.cpp
g++ -o sender-sig.o sender-sig.cpp

touch testinput.txt
echo "secret text" >> testinput.txt

echo "Contents of test testinput.txt is: "
cat testinput.txt
echo ""


echo "Starting the receiver program"
./recv-sig.o &

sleep 3s #let receiver output in peace

echo "" #blank line

echo "Waiting to make sure that receiver is initialized before starting sender"
echo "Starting sender program in ..."

i=5
while [ $i -ge 1 ]
do
	sleep 1s
	echo "$i"
	i=$(( $i - 1 ))
done

echo "Starting the sender program."
echo "Sender program will take in input from testinput.txt"
./sender-sig.o testinput.txt

sleep 5s

echo ""
echo "Contents of testinput.txt__recv is: "
cat testinput.txt__recv

echo ""
echo "[=====End of execution=====]"

