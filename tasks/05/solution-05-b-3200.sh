#!/bin/bash

read -p "Enter full path of a directory: " dir

if [[ ! -d "${dir}" ]] ; then
	echo 'Entered path is not a dir!' 1>&2
	exit 1
fi

fileCount=$(find  "${dir}" -type f 2>/dev/null| wc -l)

dirCount=$(find  "${dir}" -mindepth 1 -type d 2>/dev/null| wc -l)

echo "Directory ${dir} has ${fileCount} files and ${dirCount} directories."

exit 0
