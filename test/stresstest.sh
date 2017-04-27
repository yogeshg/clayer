#!/bin/bash
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <request_count> <simultaneous_connection> <server_address:port_num> "
    exit
fi
SERVERADDR=$1

ab -n $1 -c $2  -H "Accept-Encoding: gzip, deflate" http://$3/work | grep "Time per request"


