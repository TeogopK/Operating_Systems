#!/bin/bash

read file
read str

echo "File ${file}"
echo "File ${str}"

if [[ -z ${file} || -z ${str} ]]  ; then
	echo 'Script usage is <scipt> <file> <str>' 1>&2
	exit 1
fi

if [[ ! -f ${file} ]] ; then
	echo 'First parameter must be an existing file' 2>&1
	exit 2
fi

cat "${file}" | egrep -q "${str}" 
echo ${?}

exit 0
