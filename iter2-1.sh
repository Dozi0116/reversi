#!/bin/sh

##########
i=8 # max winrate
# i=84 # min winrate
##########
rand=8192
echo "playing ./iter_test $i $rand"
echo "T = $i rand = $rand"
./iter_test2 $i $rand >> iter2-1.csv

