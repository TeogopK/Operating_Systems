#!/bin/nash

if [[ ! ${#} -eq 3 ]] ; then
	echo "Script usage is <script> <number> <left-end> <right-end>" 1>&2
	exit 10
fi

regexNumber=$(echo "0|-?[1-9]+[0-9]*")

for parameter in ${@}
do
	if ! echo "${parameter}" | egrep -qx "${regexNumber}" ; then
		echo "All parameters must be whole numbers" 1>&2
		exit 3
	fi
done

if [[ ${2} -gt ${3} ]] ; then
	echo "The range is swapped" 2>&1
	exit 2
fi

if [[ ${1} -ge ${2} && ${1} -le ${3} ]] ; then
	echo "Number is within the range"
	exit 0
else
	echo "Number is NOT in the given range" 2>&1
	exit 1
fi

exit 20
