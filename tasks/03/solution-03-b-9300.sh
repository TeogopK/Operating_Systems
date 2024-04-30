#!/bin/bash

cat ssa-input.txt

arrays=($(cat ssa-input.txt | awk -F 'Array' 'NR > 2 {print $0}'))

for i in ${arrays[@]}
do
	echo "${i}"
done

#A-2I:0:5 35 36
#A-2I:0:6 34 35
#B-1I:1:1 35 50
#B-1I:1:2 35 49

