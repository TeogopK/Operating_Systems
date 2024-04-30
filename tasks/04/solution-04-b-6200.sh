#!/bin/bash

usersWithPsArr=($(ps -e -o user= | awk '{print $1}' | sort | uniq ))

for user in ${usersWithPsArr[@]}
do	
	if ! who | awk '{print $1}' | sort | uniq | egrep -qx "${user}" ; then
		echo "${user}"
	fi

done
