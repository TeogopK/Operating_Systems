#!/bin/bash
#set -euo pipefail

old_IFS=${IFS}
IFS=$'\n'

countsArr=($(ps -eo pid=,ppid= | sort -k 2,2 | awk 'NR == 1 {parent = $2} $2 == parent {count++} $2 != parent {print parent, count; parent = $2; count = 1} END {print parent, count}'))

psArr=($(ps -eo pid=,ppid= ))
IFS=${old_IFS}

for ps in "${psArr[@]}"
do
	ps=$(echo $ps | tr -s '[:space:]' | sed 's/^[[:blank:]]*//')

	child=$(echo "${ps}" | cut -d ' ' -f 1)
	parent=$(echo "${ps}" | cut -d ' ' -f 2)

echo "${countsArr[@]}"
	countParent=$(echo "${countsArr[@]}" | egrep "^${parent} " | cut -d ' ' -f 2)

	countChild=$(echo "${countsArr[@]}" | egrep "^${child} " | cut -d ' ' -f 2)

	echo "CHILD $countParent PAR $countChild"

	if [[ ${countParent} -lt ${countChild} ]] ; then
		echo "${child}"
	fi
done
