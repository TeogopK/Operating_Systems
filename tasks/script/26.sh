#!/bin/bash

if [[ ! ${EUID} -eq 0 ]] ; then
    echo "Script must be run as root" 1>&2
    exit 1
fi
# is variant [[ ! $(id -u) -eq 0 ]]

totalInfo=$(ps -eo user=,rss= | sort -k 1 | awk 'BEGIN {usr=""; count=0; sum=0} $1==usr {count++; sum+=$2} $1!=usr && count!=0 && usr!=""{printf("%s %.2f\n",usr, sum/count); usr=$1; count=1; sum=$2} $1!=usr && count==0 {usr=$1; count=1; sum=$2} END {printf("%s %.2f\n", usr, sum/count)}')

biggestPsInfo=($(ps -eo user=,pid=,rss= | tr -s [:space:] | sort -t ' ' -k 1,1 -k 3nr,3  | awk 'BEGIN {usr=""} $1!=usr {print $1":"$2":"$3; usr=$1}'))

for psEntry in ${biggestPsInfo[@]}
do
	user=$(echo "${psEntry}" | cut -d ':' -f 1)
	pid=$(echo "${psEntry}" | cut -d ':' -f 2)
	max=$(echo "${psEntry}" | cut -d ':' -f 3)

	if echo "${totalInfo}" | awk -v awkUser=${user} -v awkMax=${max} '$1==awkUser && 2*$2<awkMax {print "Kill"}' | egrep -q '.' ; then
		kill -15 ${pid}
		sleep 2
		kill -9 ${pid} 
		echo "MUST KILL ${pid}"
	fi

done

exit 0
