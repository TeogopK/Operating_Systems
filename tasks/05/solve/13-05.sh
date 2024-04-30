#!/bin/nash

if [[ ! ${#} -eq 1 ]] ; then
	echo 'Script usage is <script> <username>' 1>&2
	exit 1
fi

if ! cat /etc/passwd | cut -d ':' -f 1 | egrep -qx "${1}" ; then
	echo 'User does not exist' 1>&2
	exit 2
fi

until who | awk '{print $1}' | egrep -qx "${1}" 
do
	sleep 1
	echo checking
done

echo "User has logged in" 
exit 0
