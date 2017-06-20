#!/bin/bash

echo "------------------------"
echo ""
echo "Changing to lab5 folder"
echo ""
cd /home/lcom/svn/lab5

echo "Making executable"
echo ""

make

for i
do
	if [ $i = 0 ]; then
		echo "service run `pwd`/lab5"
		service run `pwd`/lab5
	elif [ $i = 1 ]; then
		echo "service run `pwd`/lab5 -args \"init 0x105 3\""
		service run `pwd`/lab5 -args "init 0x105 3"
	elif [ $i = 2 ]; then
		echo "service run `pwd`/lab5 -args \"square 10 10 50 0x00f\"(blue square)"
		service run `pwd`/lab5 -args "square 150 150 50 0x00F"
	elif [ $i = 3 ]; then
		echo "service run `pwd`/lab5 -args \"line 50 50 300 550 0x002\" (green)"
		service run `pwd`/lab5 -args "line 50 50 300 550 0x002"
	elif [ $i = 4 ]; then
		echo "service run `pwd`/lab5 -args \"xpm 50 50 5\""
		service run `pwd`/lab5 -args "xpm 50 50 5"
	elif [ $i = 5 ]; then
		echo "service run `pwd`/lab5 -args \"move 50 50 5 1 300 3\""
		service run `pwd`/lab5 -args "move 50 50 5 1 300 3"
	elif [ $i = 6 ]; then
		echo "service run `pwd`/lab5 -args \"controller\""
		service run `pwd`/lab5 -args "controller"
	elif [ $i = 7 ]; then
		echo "service run `pwd`/lab5 -args \"ascii\""
		service run `pwd`/lab5 -args "ascii"
	fi
done
tail -f /var/log/messages 
echo ""
echo "Done"