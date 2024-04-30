#!/bin/bash
set -eou pipefail

if [[ ! ${#} -eq 2 ]] ; then
    echo "Script usage is <script> <dirname> <string>" 1>&2
    exit 1;
fi

if [[ ! -d ${1} ]] ; then
    echo "First parameter must be an existing dir" 1>&2
    exit 1
fi

paths=$(find ${1} -mindepth 1 -maxdepth 1 -type f -printf '%f\n' 2>/dev/null \
	| egrep -x "vmlinuz-[0-9]+\.[0-9]+\.[0-9]+-${2}")

# old variant
# version=$(echo "${paths}" | awk -F '-' '{print $2}' \
#	| sort -t '.' -snr -k 3 | sort -t '.' -snr -k 2 \
#	| sort -t '.' -snr -k 1 | head -n 1)

version=$(echo "${paths}" | awk -F '-' '{print $2}' | sort -t '.' -nr -k1,1 -k2,2 -k3,3 | head -n 1)


echo "${paths}" | awk -F '-' -v vers=${version} '$2==vers {print $0}'

exit 0
