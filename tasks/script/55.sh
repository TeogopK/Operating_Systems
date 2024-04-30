#!/bin/bash
set -euo pipefail

if [[ ! ${#} -eq 1 ]] ; then
	echo 'Script usage is <script> <dir>' 1>&2
	exit 1
fi

if [[ ! -d ${1} ]] ; then
	echo 'First parameter must be an existing dir' 1>&2
	exit 1
fi



