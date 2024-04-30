#!/bin/bash

if [ ! ${#} -eq 1 ];then
	echo "Script usage is <script> <file>"
	exit 1
fi

if [ ! -f ${1} ];then
	echo "Script parameter should be an existing file"
	exit 1
fi

file=${1}

if [ ! -r ${file} ];then
       echo "File has no read permissions"
       exit 1;
fi
       
lines=$(wc -l < ${file})
words=$(wc -w < ${file})
chars=$(wc -m < ${file})

echo "File ${file} has ${lines} lines, ${words} words, ${chars} chars!"
