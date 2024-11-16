#!/bin/bash

num_pings=1000
end_ip="192.168.100.6"
gate="2080"

for((i=1; i<=num_pings; i++)); do 
	echo "Ping $i para $end_ip:"
	ping -c 1 $end_ip
	echo
done