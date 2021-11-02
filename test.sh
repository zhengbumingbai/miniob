#!/bin/bash
>test.tmp
if [ $# -gt 0 ] && [ $1 == "-c" ]
then
    rm -rf ./build/miniob/db/sys/*
else
    cat input.sql | while read line
    do
        if [ ${#line} -eq 0 ] || [ ${line:0:1} == "#" ]
        then
            echo "$line"
            continue
        fi
        if [ ${line:0:1} == "-" ]
        then 
            continue
        fi
        echo ">> $line"
        echo -n "$line" | ./build/bin/obclient | sed -e "s/miniob > //" 
        echo "------------------------------" 
    done | tee -a test.tmp
fi