#!/bin/sh

rand=2019
# rand = 2019 + 1 + 2 + ... + 45 = 3054
rand=3054

for i in `seq 1 100`
do
    for j in 1 2
    do
        echo "playing ./test $j $i $rand"
        echo "T = $i, order = $j, rand = $rand" >> result.dat
        ./test $j $i $rand >> result.dat
    done
done

