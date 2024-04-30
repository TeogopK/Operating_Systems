#!/bin/bash
set -euo pipefail

file=$(mktemp)

ps -eo pid=,ppid= | sort -k 2,2 | awk 'NR == 1 {parent = $2} $2 == parent {count++} $2 != parent {print parent ":" count; parent = $2; count = 1} END {print parent ":" count}' > "${file}"

psArr=($(ps -eo pid=,ppid= | awk '{print $1 ":" $2}'))

for ps in ${psArr[@]}
do
	child=$(echo "${ps}" | cut -d ':' -f 1)
	parent=$(echo "${ps}" | cut -d ':' -f 2)

	countChild=$(cat "${file}" | egrep "^${child}:" | cut -d ':' -f 2)
	countParent=$(cat "${file}" | egrep "^${parent}:" | cut -d ':' -f 2)

	if [[ ${countParent} -lt ${countChild} ]] ; then
		echo "${child}"
	fi
done

rm "${file}"
	
exit 0
