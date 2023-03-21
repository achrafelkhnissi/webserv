#!/bin/bash

for N in {1..100}
do
    ruby client.rb $((($N % 6) + 1)) & #>/dev/null &
done

wait
