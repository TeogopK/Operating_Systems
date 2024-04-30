#!/bin/bash
set -euo pipefail

if [ ! ${#} -eq 2 ] ; then
	echo "Script usage is <script> <dir> <dir>" >&2
	exit 1
fi

if [ ! -d ${1} ] ; then
	echo "First parameter must be an existing directory"
	exit 1
fi

if [ ! -d ${2} ] ; then
	echo "Second parameter must be an existing directory"
	exit 1
fi

#checks if second directory is empty
if find ${2} -mindepth 1 | head -n 1 | egrep -q '.' ; then
	echo "Second parameter must be an empty directory"
	exit 1
fi

#store the absolute paths to the given directories
realPathSource=$(realpath ${1})
realPathDest=$(realpath ${2})

#stores all potential files to copy with a relative path excluding the starting directory of the find
#this paths are both correct as relative paths in the given and wanted directory
allSourcesWithoutDirArr=($(find ${1} -type f -printf '%P\n'))

#stores all the files that must be copied, ignoring the ones that mustn't
toCopySourcesArr=()
#stores the new paths of all the files that must be copied
toCopyDestArr=()

#loops through all potential files
for file in ${allSourcesWithoutDirArr[@]}
do
	#gets the full path where the file currently is
	fullpath=$(echo "${realPathSource}/${file}")
	
	#checks if it has a name for a potential swp file
	if [[ ${fullpath}  =~ /\.[^/]+\.swp$ ]]; then 
	# alt --> if echo "${fullpath}" | egrep -q '/\.[^/]+\.swp$' ; then 

		#checks if the potential swp file has an original file
		newpath=$(echo "${fullpath}" | sed -E 's:/\.([^/]+)\.swp$:/\1:')

		if [ ! -f ${newpath} ] ; then
			#if orginal file does not exist, the first file is not a swp and must be copied
			toCopySourcesArr+=($(realpath "${fullpath}"))
			toCopyDestArr+=($(echo "${realPathDest}/${file}"))
		fi
	else 
		#the file is not a potential swp file and must be copied
		toCopySourcesArr+=($(realpath "${fullpath}"))
		toCopyDestArr+=($(echo "${realPathDest}/${file}"))
	fi
done


#loops through all the files that must be copied
for i in ${!toCopySourcesArr[@]}
do
	#gets the path consisting of only directories
	newDirectory=$(echo "${toCopyDestArr[i]}" | sed -E 's:/[^/]+$::')
	# alternative is --> newDirectory=$(dirname "${toCopyDestArr[i]}")

	#creates the needed directories
	mkdir -v -p "${newDirectory}"

	#copies the files
	cp -v -i "${toCopySourcesArr[i]}" "${toCopyDestArr[i]}"

done

exit 0
