#!/bin/sh

# 2つのプログラムのためのwrapper

while true
do
    do_a_transaction
    if play_again
    then
        continue
    fi
    break    
done
