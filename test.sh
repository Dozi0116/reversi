#!/bin/sh

rand=2019

for i in `seq 1 100`
do
    rand=`expr $rand + $i`
    for j in 1 2
    do
        echo "playing ./test $j $i $rand"
        echo "T = $i, order = $j, rand = $rand" >> result.dat
        ./test $j $i $rand >> result.dat
    done
done

