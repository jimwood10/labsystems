#!/bin/bash

echo "How many files do you want to add?"

read NUMBER

echo "Where do you want to add them (full path)"

read LOCATION


for i in $(seq 1 $NUMBER)
do
touch $LOCATION/$i

done

echo $NUMBER "files created in " $LOCATION
