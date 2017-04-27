#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <server_address:port_num>"
    exit
fi
SERVERADDR=$1

ab -n 500 -c 25  -H "Accept-Encoding: gzip, deflate" http://$1/work # Run GETWORK/multithreading test


