#!/bin/bash

echo "What filesystem are you using?"

read FS

echo "What filesset do you want to add?"

read FSET

mmcrfileset $FS $FSET  --inode-space=new

mmlinkfileset $FS $FSET -J $(mmlsfs fs1 -T | grep mount | awk '{print $2}')/$FSET

mmlsfileset $FS
