#!/bin/bash
set -euo pipefail

if [[ ${#} -le 0 ]] ; then
	echo "No script parameters given" 1>&2
	exit 1
fi

for file in "${@}"
do
	regexAllTypes="^[[:blank:]]*([a-z0-9]+\.)+[[:blank:]]+[[:digit:]]*[[:blank:]]+IN[[:blank:]]+(SOA|NS|A|AAA)([[:blank:]]+[^;]*)+"

	if ! cat "${file}" | egrep -q "${regex}" ; then
		echo "Exit 1; ${file}; File does not contain a SOA entry" 1>&2

	elif  [[ ! $(cat "${file}" | egrep "${regex}" | wc -l) -eq 1 ]] ; then
	# elif ! cat "${file}" | egrep "${regex}" | wc -l | egrep -q -x "1" ; then
		echo "Exit 2; ${file}; File contains multiple SOA entries" 1>&2

	else
		echo "OK"
	fi
	#check for increasing 99
done
