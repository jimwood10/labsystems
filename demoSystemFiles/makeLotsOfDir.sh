#!/bin/bash
gpfsBase=/gpfs/fs1
cd $gpfsBase

for i in {1..5}
do
	mkdir $i
		if [ $? -eq 0 ]
			then
		echo "Script failed"
		exit
		fi
	cd $i
done
