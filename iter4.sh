#!/bin/sh

# rand=2019
# rand = 2019 + 1 + 2 + ... + 45 = 3054
# rand = 3054

#####
# rand=3054 # iter 1
# rand=2020 # iter 2
rand=116 # iter 3
# rand=182 # iter 4
# rand=1 # iter 5
#####

for i in `seq 51 100`
do
    # rand=`expr $rand + $i`
    echo "playing ./iter_test $i $rand"
    echo "T = $i rand = $rand"
    ./iter_test3 $i $rand >> iter4.csv
done

