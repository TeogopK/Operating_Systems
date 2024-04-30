#!/bin/bash
ps -eo user=,comm= | sort | awk 'BEGIN {count=0 user=""} {if($1 == user) {if($2 == vim) {count++}} else {if(count > 1) {print user} count=0}}'
