#!/bin/bash

copyDir="${1}"

if [[ ! -d ${copyDir} ]] ; then
	echo "Script needs first parameter to be an exisitng directory" 1>&2
	exit 1
fi

if [[ -z ${2} ]] ; then
	pasteDir=$(date +'%Y%m%d')
	mkdir -p "${pasteDir}" 
else
	pasteDir="${2}"
	if [[ ! -d ${pasteDir} ]] ; then
		echo "Second parameter must be an existing directory" 1>&2
		exit 2
	fi
fi

realPasteDir=$(realpath ${pasteDir})

OLD_IFS="${IFS}"
IFS=$'\n'

files=($(find "${1}" -type f -mmin -45 -printf '%P\n' 2>/dev/null))

IFS="${OLD_IFS}"

for file in "${files[@]}" ; do
	fileCopy=$(echo "$(realpath ${copyDir})/${file}")
	
	filePaste=$(echo "${realPasteDir}/${file}")
	filePasteDir=$(dirname "${filePaste}")

	mkdir -p "${filePasteDir}"
	cp -vi "${fileCopy}" "${filePaste}"
done

tarName=$(echo "$(basename ${realPasteDir}).tar")

tar -cf "${tarName}" "${realPasteDir}"

exit 0
