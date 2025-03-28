#!/bin/bash

PASSWORD=ScaleStorageCluster
echo "What user would you like to add?"

read USER

useradd $USER
echo $PASSWORD | passwd  --stdin $USER
usermod -aG wheel $USER
