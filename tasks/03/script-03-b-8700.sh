#!/bin/bash

if [ ! ${#} -eq 1 ] ; then
	echo 'Script usage is <script> <filename>!'
	exit 1;	
fi

if [ ! -f ${1} ] ; then
	echo 'Parameter must be an existing file'
	exit 1;	
fi

arr=($(cat ${1} | cut -d ':' -f 4 | sort | uniq -c | sort -n -r | head -n 5 | sed 's:^[[:space:]]*::g' | cut -d ' ' -f 2))

for i in "${arr[@]}"
do
	cat /etc/group | awk -v gidValue="$i" -F ':' '$3==gidValue {print $1}'
	cat /etc/group | awk -v gidValue="${i}" 'BEGIN {FS=":"} $3 == gidValue {print $1}'

done
