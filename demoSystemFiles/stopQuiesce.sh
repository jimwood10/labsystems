#!/bin/bash

echo "What node to quiesce?"

read NODE

echo "Which filesystem to snaphot"

read FS

echo "What is the mount point of the FS?"

read MOUNT

mmdsh -N $NODE mmfsadm test quiesce $FS daemon 33 & mmdsh -N $NODE mmfsadm test quiesce $FS kernel 43210 ; mmchattr -I defer $MOUNT; mmfsadm test quiesce $FS kernel 0 & mmcrsnapshot $FS test.$(date +"%s")
