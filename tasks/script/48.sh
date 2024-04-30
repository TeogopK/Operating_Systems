#!/bin/bash

if [ ! ${#} -eq 2 ] ; then
	echo 'Script usage is <script> <dir1> <dir2>'
	exit 1
fi

if [ ! -d ${1} ] ; then
	echo 'First parameter must be a directory'
	exit 1
fi

if [ ! -d ${2} ] ; then
	echo 'Second parameter must be a directory'
	exit 1
fi

if find ${2} -mindepth 1 | head -n 1 | egrep -q '*' ; then
	echo 'Second directory must be empty'
	exit 1 
fi


startDir=$PWD
cd ${1}

allArr=($(find . -type f))

toCopyArr=()

for afile in ${allArr[@]}
do
	if [ ! $(echo ${afile} | egrep '/\..*\.swp$') ] 
	then
		echo "IN iF"
		toCopyArr+=( ${afile} )
	else
		echo "Out of IF"
		realPath=$(echo "${afile}" | sed 's:/\.\(.*\).swp$:/\1:')
		if [ ! -f ${realPath} ] ; then
			toCopyArr+=( ${afile} )
		fi
	fi	

done

finalPathsArr=()
for apath in ${toCopyArr[@]}
do
	finalPathsArr+=($(realpath ${apath}))
done

cd ${startDir}
cd ${2}

for index in ${!toCopyArr[@]}
do
	dirName=$(dirname ${toCopyArr[index]})
	mkdir -p ${dirName}
	cp -p -i ${finalPathsArr[index]} ${toCopyArr[index]}
done

echo 'Script finished successfully'
exit 0
