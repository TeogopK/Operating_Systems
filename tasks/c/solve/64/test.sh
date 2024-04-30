#!/bin/bash

echo 'TEST 1'
echo -n "Result:   "
echo asdf | ./main -d 'd123' | ./main 'sm' 'fa' | ./main -s 'f'
echo -n 'Expected: '
echo asdf | tr -d 'd123' | tr 'sm' 'fa' | tr -s 'f'
echo ' '

echo 'TEST 2'
echo -n 'Result:   '
echo dasdfd333df3 | ./main -d 'd123' 
echo -n 'Expected: '
echo dasdfd333df3 | tr -d 'd123' 
echo ' '

echo 'TEST 3'
echo -n 'Result:   '
echo dasdfd333df3 | ./main -s '3' 
echo -n 'Expected: '
echo dasdfd333df3 | tr -s '3' 
echo ' '
