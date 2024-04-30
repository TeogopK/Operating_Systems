#!/bin/bash

if [[ ! ${#} -eq 1 ]] ; then
	echo "Script usage is <script> <file>" 1>&2
	exit 1;
fi

if [[ ! -f ${1} ]] ; then
	echo "First parameter must be an existing file!" 1>&2
	exit 2;
fi

level=$(cat "${1}" | egrep  -o "\{|\}" | awk 'BEGIN {max=0} $0 == "{" {count++} $0 == "}" {count--} count > max { max = count } END {print max}')

echo "The deepest nesting is ${level} levels"

exit 0
