#!/bin/bash
# This script checks if the machine is connected to the internet, it prints all the IP addresses in the terminal if it is, otherwise it prints the string "Not connected".
 
ip=`hostname -I`
if [ -n "$ip" ]; then
    for i in $ip
        do
	echo "Connected to $i"
	done
else
    echo "Not connected!"
fi
