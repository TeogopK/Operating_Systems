#!/bin/bash

(
for file in ${@}
do
	echo "File is ${file}"
done
)

echo -e "\nTEST2\n"

(
for file in "${@}"
do
	echo "File is ${file}"
done
)
