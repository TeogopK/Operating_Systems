#!/bin/bash
set -euo pipefail

if [[ ! ${#} -eq 3 ]] ; then
	echo 'Script usage is <script> <file1> <file2> <file3>'
	exit 1
fi

if [[ ! -f ${1} ]] ; then
	echo 'First parameter must be an existing file'
	exit 2
fi

if [[ ! -f ${2} ]] ; then
	echo 'Second parameter must be an existing file'
	exit 3
fi

paste file1 file2 -d $'\n' > file3

exit 0
