#!/bin/bash

echo "How many files do you want to add?"

read NUMBER

echo "What size do you want them to be? eg 10K, 10M, 10G etc"

read SIZE

echo "Where do you want to add them (full path)"

read LOCATION

echo "What prefix?"

read PREFIX

echo "What suffix?"

read SUFFIX


for i in $(seq 1 $NUMBER)
do

# touch $LOCATION/$i.

fallocate -l $SIZE $LOCATION/$PREFIX$i.$SUFFIX

done

# echo $NUMBER "files created in " $LOCATION

echo $NUMBER "files of size " $SIZE " created in " $LOCATION
