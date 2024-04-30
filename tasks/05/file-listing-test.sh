#!/bin/bash

(
	echo "TEST1: No IFS"
	arr=($(find . -type f | egrep -v './.*\.'))

	for i in "${arr[@]}"
	do
		echo "I am a file: ${i}"
	done
)

(
	echo "TEST2: With IFS"

	OLD_IFS="${IFS}"	
	IFS=$'\n'

	arr2=($(find . -type f | egrep -v './.*\.'))

	for i in "${arr2[@]}"
	do
		echo "I am a file: ${i}"
	done
)
	
