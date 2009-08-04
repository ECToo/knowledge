#!/bin/bash
LOCALDIR=`pwd`

for i in `find \`pwd\` -type d | grep -v svn | grep -v build`
do 
	cd $i
	for j in `ls | grep Makefile.wii`
	do 
		make -f $j clean
	done
	for j in `ls | grep knowledge.log`
	do 
		rm $j
	done
done
cd $LOCALDIR

