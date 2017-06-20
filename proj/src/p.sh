#!/bin/bash

echo "------------------------"
echo ""
echo "Changing to project folder"
echo ""
cd /home/lcom/svn/proj/src

echo "Making executable"
echo ""
make

for i
do
	if [ $i = default ]; then
		echo "service run `pwd`/proj -args \"default\""
		service run `pwd`/proj -args "default"
	elif [ $i = save ]; then
		echo "service run `pwd`/proj -args \"save 5\" (save every 5 seconds)"
		service run `pwd`/proj -args "save 5"
	elif [ $i = main0 ]; then
		echo "service run `pwd`/proj -args \"main 7 0 5 home/lcom/svn/proj/src/test.txt\""
		service run `pwd`/proj -args "main 7 0 5 home/lcom/svn/proj/src/test.txt"
	elif [ $i = main1 ]; then
		echo "service run `pwd`/proj -args \"main 0 15 5\""
		service run `pwd`/proj -args "main 0 15 5"
		
	elif [ $i = -1 ]; then
		service down proj
		echo "service run `pwd`/proj -args \"restart_graphics\""
		service run `pwd`/proj -args "restart_graphics"
	fi
done
tail -f /var/log/messages