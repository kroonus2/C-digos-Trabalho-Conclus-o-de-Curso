#!/bin/bash

num_pings=1000  # Defina o número de pings
end_ip="192.168.100.201"
port="80"

for ((i=1; i<=num_pings; i++)); do
    echo "Ping $i para $end_ip na porta $port:"
    sudo hping3 -S -p $port -c 1 $end_ip
    echo
done
