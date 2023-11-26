#!/bin/bash

z=10
n=$1
breadth=$2
length=$3
generator=$4

touch in
echo "$z" > in
for ((i=1; i<=z; i++)); do
    ./"$generator" "$n" -breadth "$breadth" -length "$length" >> in
done
