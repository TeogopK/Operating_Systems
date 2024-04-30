#!/bin/bash

if [[ ${#}  -eq 0 ]] ; then
	echo 'Script needs at least one file as paramter' 1>&2
	exit 2;
fi

newSerial=$(echo "$(date +'%Y%m%d')00")
todayDate=$(date +'%Y%m%d')

OLD_IFS="${IFS}"
IFS=$'\n'

successCounter=0

for file in "${@}"
do
	if [[ ! -f ${file} ]] ; then
		echo "File is not an existing file; ${file}; status 10" 1>&2
		continue

	fi

	regexAllEntries="[[:blank:]]*([a-z0-9]+\.)+[[:blank:]]+[[:digit:]]*[[:blank:]]+IN[[:blank:]]+(A|NS|AAA|SOA)[[:blank:]]*(.*)"
	
	entriesArr=$(cat "${file}" | sed -E 's/;(.*)//g' | awk 'BEGIN {i=0} $0 ~ /.*\(.*/ {i=1} $0 ~ /.*\).*/ {i=0} i==1 {printf $0" "} i==0 {print $0}' | egrep -x "${regexAllEntries}" | sed -E 's:\(|\)::g')

	if [[ -z "${entriesArr}" ]]; then
		echo "File has no resourse entry; ${file}; status 11" 1>&2
		continue

	fi

	regexSOAEntry="[[:blank:]]*([a-z0-9]+\.)+[[:blank:]]+[[:digit:]]*[[:blank:]]+IN[[:blank:]]+(SOA)[[:blank:]]+([a-z0-9]+\.)+[[:blank:]]+([a-z0-9]+\.)+[[:blank:]]+(([[:digit:]]+[[:blank:]]+){4}([[:digit:]]+){1})[[:blank:]]*"

	if ! echo "${entriesArr}" | head -n 1 | egrep -qx "${regexSOAEntry}" ; then
		echo "First entry is not a SOA entry; ${file}; status 12" 1>&2
		continue

	fi

	if [[ ! $(echo "${entriesArr}" | egrep -x "${regexSOAEntry}" | wc -l) -eq 1 ]]; then
		echo "There is more than one SOA entry' ${file}; status 13" 1>&2
		continue

	fi


	regexSOASerial="([[:blank:]]*([a-z0-9]+\.)+[[:blank:]]+[[:digit:]]*[[:blank:]]+IN[[:blank:]]+(SOA)[[:blank:]]+([a-z0-9]+\.)+[[:blank:]]+([a-z0-9]+\.)+[[:blank:]]+)([[:digit:]]+)(.*)"

	serial=$(echo "${entriesArr}" | head -n 1 | egrep -x "${regexSOAEntry}" | sed -E "s/${regexSOASerial}/\6/")	
	oldDate=$(echo "${serial}" | egrep -o '^[[:digit:]]{8}')
	
	if [[ ${oldDate} -lt ${todayDate} ]] ; then
		sed -i -E "s/${serial}/${newSerial}/" "${file}" 

		echo "File successfully updated; ${file}; status 1000"
		successCounter=$(( successCounter + 1 ))

	elif [[ ${oldDate} -gt ${todayDate} ]] ; then
		echo "SOA date is from the future; ${file}; status 14" 1>&2
		continue


	else
		digits=$(echo "${serial}" | sed -E 's/^[[:digit:]]{8}//')

		if [[ "${digits}" == "99" ]] ; then
			echo "SOA date can not be incremented; ${file}; status 15" 1>&2
			continue

		elif [[ "${digits}" == "09" ]] ; then
			uppSerial=$(echo "${todayDate}10")

		elif [[ $(echo "${digits}" | egrep -o '^[[:digit:]]') -eq "0" ]] ; then
			lastDigit=$(echo "${digits}" | egrep -o '[[:digit:]]$')
			lastDigit=$(( lastDigit + 1 ))
			uppSerial=$(echo "${todayDate}0${lastDigit}")

		else 
			digits=$((digits + 1))
			uppSerial=$(echo "${todayDate}${digits}")

		fi

		sed -i -E "s/${serial}/${uppSerial}/" "${file}" 

		echo "File successfully updated; ${file}; status 1000"
		successCounter=$(( successCounter + 1 ))

	fi

done

if [[ ${successCounter} -gt 0 ]] ; then
	echo "Files updated successfully ${successCounter} out of ${#}"
	exit 0
fi

echo "No files were updates successfully" 
exit 1
