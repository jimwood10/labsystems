#!/bin/bash
gpfsBase=/gpfs/fs1
nameLength=20
numberOfFiles=10

cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $nameLength | head -n $numberOfFiles > /tmp/random.txt
while read LINE
do
        touch $gpfsBase/$LINE
                if [ $? -ne 0 ]
                then
                echo "Script failed"
                exit
                fi
done < /tmp/random.txt
