#!/bin/bash

if [[ ! ${#} -eq 2 && ! ${#} -eq 1 ]] ; then
	echo "Script usage is <script> <number> (optinal)<delimerator>" 1>&2
	exit 1
fi

del=' '

if [[ ! -z ${2} ]] ; then
	del="${2}"
fi

regexNumber=$(echo "0|-?[1-9][0-9]*")

if ! echo "${1}" | egrep -qx "${regexNumber}" ; then
	echo "First parameter must be a whole number"
	exit 2
fi

revDel=$(echo "${del}" | rev)

echo "${1}" | sed 's/./&~/g' | sed 's/~$//' | rev | awk -F '~' -v awkDel="${revDel}" '{for(i=1;i<=NF;i++){if(count == 3 && $i != "-"){count=0; printf awkDel} if($i=="-"){printf $i} if($i ~ /[0-9]/){count++; printf $i} }} END {printf "\n"}' | rev
 
exit 0
