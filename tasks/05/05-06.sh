#!/bin/bash

if [[ ! ${#} -eq 1 ]] ; then
	echo "Script needs exactly one parameter"
	exit 1
fi

str=${1}

if echo "${str}" | egrep -xq '[[:alnum:]]*' ; then
	echo "${str} has only letters or numbers"
else
	echo "${str} is invalid"
fi

exit 0
