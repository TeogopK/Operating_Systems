#!/bin/bash

read -p "Enter a username: " username

countSessions=$(who | awk '{print $1}' | egrep -x "${username}" | wc -l)

echo "${username} has ${countSessions} active sessions!"
