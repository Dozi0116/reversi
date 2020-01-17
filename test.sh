#!/bin/sh

rand=2019
# rand = 2019 + 1 + 2 + ... + 45 = 3054
# rand = 3054

for i in `seq 1 100`
do
    rand=`expr $rand + $i`
    echo "playing ./test $i $rand"
    echo "T = $i rand = $rand"
    ./test $i $rand >> result.dat
done

