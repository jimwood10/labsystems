#!/bin/bash

##### variables #####
gpfsBase=/gpfs/fs1
numberOfDir=10
##### variables #####

cd $gpfsBase

for i in $(seq 1 $numberOfDir)
do
        mkdir $i
                if [ $? -ne 0 ]
                        then
                echo "Script failed"
                exit
                fi
        cd $i
done

