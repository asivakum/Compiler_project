#! /bin/bash

i=1
while [ $i -le 22 ]
do
	echo "testcase: $i"
	inputfilename="/export/home/a/asivakum/ECE573_projects/step3/input/test$i.micro"
        if [ -f $inputfilename ]; then
            #echo "file found"
	    outputfilename="./myout/myout$i.out"
	    standardout="./output/573/test$i.out"
	    ./build/Micro $inputfilename > $outputfilename
	    diff -b -B $outputfilename $standardout
        fi
	echo
	(( i++ ))
done
