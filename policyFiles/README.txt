===================================
For the immutable policies / script
===================================

Edit policies to fit your system
===============================
# mmlsfileset fs1
Filesets in file system 'fs1':
Name                     Status    Path
root                     Linked    /ibm/fs1
testIam                  Linked    /ibm/fs1/testIam
.audit                   Linked    /ibm/fs1/.audit

The policy scripts, list-immutable.txt and makeimmutablelistpolicy.pol,  refer to fileset "worm". 
Change this to your actual fileset. Eg
sed -i 's/worm/testIam/g' makeimmutablelistpolicy.pol

Also edit makeimmutablelistpolicy to show the correct path to makeimmutable.sh

Run policies
============
makeimmutablelistpolicy.txt is set to make mp3 and pdf files immutable
Either populate the fileset/path with some files that will be matched, or change the select clauses.

Check the listing of list-immutable.pol  with:
mmapplypolicy fs1 -P list-immutable.pol -f ./gpfs -I defer
(the naming of this policy, imho, is counter-intuitive. It lists files that are not immutable)

cat gpfs.list.immut

Test makeimmutablelistpolicy 
mmapplypolicy fs1 -P makeimmutablelistpolicy.pol -N <node> -I test
(if running on multiple nodes, which is the default,  the makeimmutable.sh script needs to be in the same path on every node)

Run the makeimmutablelistpolicy
mmapplypolicy fs1 -P makeimmutablelistpolicy.pol -N <node>

mmlsattr -L /ibm/fs1/testIam/*pdf | grep immut
immutable:            yes
immutable:            yes
immutable:            yes
immutable:            yes
immutable:            yes
immutable:            yes
immutable:            yes
immutable:            yes
immutable:            yes
immutable:            yes

mmlsattr -L /ibm/fs1/testIam/*jpg | grep immut
immutable:            no
immutable:            no
immutable:            no
immutable:            no
immutable:            no
immutable:            no
immutable:            no
immutable:            no
immutable:            no
immutable:            no
