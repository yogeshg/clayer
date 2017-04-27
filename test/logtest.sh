#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <server_address:port_num>"
    exit
fi
SERVERADDR=$1

for i in `seq 1 20`;
do
    ab -n 1000 -c 10  -H "Accept-Encoding: gzip, deflate" http://$1/work & # Run GETWORK/multithreading test
    ab -n 1000 -c 10 -H "Accept-Encoding: gzip, deflate" http://$1/info & # Run GETINFO test
    ab -n 100 -c 10 -H "Accept-Encoding: gzip, deflate" http://$1/hello & # Run illegal access test
    ab -n 500 -c 10 -H "Accept-Encoding: gzip, deflate" http://$1/ & # Regular GET
done

