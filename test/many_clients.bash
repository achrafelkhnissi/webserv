#!/bin/bash

i=0
while [ $i -le "$1" ]
do
  curl localhost:1337/$i &>/dev/null &
#    wait $!
  echo "Started client $i"
  i=$((i+1))
done

wait
