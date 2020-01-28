#!/bin/sh

rand=2019
# rand = 2019 + 1 + 2 + ... + 45 = 3054
# rand = 3054

#####
# rand=2019 # iter 1
# rand=2020 # iter 2
# rand=116 # iter 3
# rand=182 # iter 4
# rand=1 # iter 5
#####

for i in `seq 1 100`
do
    rand=`expr $rand + $i`
    echo "playing ./test $i $rand"
    echo "T = $i rand = $rand"
    ./test $i $rand >> result.dat
done

