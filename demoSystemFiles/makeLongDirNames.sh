#!/bin/bash

##### variables #####
dirName=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 200 | head -n 1)
gpfsBase=/gpfs/fs1
NUMBER=25
##### variables #####


cd $gpfsBase

for i in $(seq 1 $NUMBER)
do
        mkdir $dirName
                if [ $? -ne 0 ]
                then
                echo "Script failed"
                exit
                fi
        cd $dirName
done
