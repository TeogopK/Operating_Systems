#!/bin/bash

if [[ ! ${#} -eq 3 ]] ; then
	echo "Script usage is <script> <file> <name> <nickname>" 1>&2
	exit 1
fi

if [[ ! -f ${1} ]] ; then
	echo "First parameter must be an existing file" 1>&2
	exit 2
fi

user=$(cat passwd | awk -F ':' -v name="${2}" 'BEGIN {regex="^"name} $5 ~ regex {print $1}')

if [[ -z ${user} ]] ; then
	echo "No user with such name found" 1>&2
	exit 3
fi

if [[ $(echo "${user}" | wc -l) -eq 1 ]] ; then
	echo "${user} : ${3}" >> "${1}"
else
	echo "${user}" | awk '{print NR, $0}'
	read -p "Enter number of record to save: " recordNumber

	oneUser=$( echo "${user}" | awk '{print NR, $0}'| awk -v userNumber="${recordNumber}" 'NR == userNumber {print $2}')

	if [[ -z ${oneUser} ]]; then
		echo "Invalid number of record" 2>&1
		exit 10
	fi

	echo "${oneUser} : ${3}" >>"${1}"
fi

exit 0
