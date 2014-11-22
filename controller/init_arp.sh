#!/bin/bash
if [ $# -lt 1 ]; then
	echo "usage $0 index"
	exit 1
fi

if [ $1 -eq 1 ]; then
	arp -s 10.0.0.2 00:00:00:00:00:02
	arp -s 10.0.0.3 00:00:00:00:00:03
fi

if [ $1 -eq 2 ]; then
	arp -s 10.0.0.3 00:00:00:00:00:03
	arp -s 10.0.0.1 00:00:00:00:00:01
fi

if [ $1 -eq 3 ]; then
	arp -s 10.0.0.2 00:00:00:00:00:02
	arp -s 10.0.0.1 00:00:00:00:00:01
fi
