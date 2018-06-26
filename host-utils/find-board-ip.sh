#!/bin/bash

# $1 -> ip subnet
# $2 -> board mac (can be substring)

fping -a -g $1/24
arp -a | grep -i $2
