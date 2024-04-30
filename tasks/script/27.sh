#!/bin/bash
set -eou pipefail

if [[ ${#} -lt 1 ]] || [[ ${#} -gt 2 ]] ; then
    echo "Script usage is <script> <dirname> (optinal)<file>" 1>&2
    exit 1
fi

if [[ ! -d ${1} ]] ; then
    echo "First parameter must be an existing dir" 1>&2
    exit 1
fi

# skipping check for if file exist/ it may be created

broken=$(find -L ${1} -type l 2>/dev/null)
brokenCount=$(echo "${broken}" | wc -l)

temp_file=$(mktemp -q)

IFS=$'\n'
links=($(find ${1} -type l 2>/dev/null))
for link in ${links[@]}
do
	if ! echo "${broken}" | egrep -q -x "${link}" ; then
		toPath=$(realpath ${link})
		echo "${link} -> ${toPath}" >> ${temp_file}
	fi
done

echo "Broken symlinks: ${brokenCount}" >> ${temp_file}

if [[ ${#} -eq 2 ]] ; then
    cat ${temp_file} > ${2}
else
    cat ${temp_file}
fi

rm ${temp_file}
exit 0
