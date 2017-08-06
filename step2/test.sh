#! /bin/bash

i=1
while [ $i -le 22 ]
do
	echo "testcase: $i"
	#./build/Micro ./testcases/input/testi$i.micro > ./testcases/myout/myout$i.out
	inputfilename="./testcases/input/test$i.micro"
	outputfilename="./testcases/myout/myout$i.out"
	standardout="./testcases/output/test$i.out"
	./build/Micro $inputfilename > $outputfilename
	diff -b -B $outputfilename $standardout
	echo
	(( i++ ))
done
