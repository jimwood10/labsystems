#!/bin/bash

##### variables #####
NUMBER=10
SIZE=10K
LOCATION=/gpfs/fs1
PREFIX=test
SUFFIX=mp3
#
# SIZE eg 10K, 10M, 10G etc
##### variables #####

for i in $(seq 1 $NUMBER)
do

fallocate -l $SIZE $LOCATION/$PREFIX$i.$SUFFIX

done

echo $NUMBER "files of size " $SIZE " created in " $LOCATION
