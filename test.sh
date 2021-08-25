#!/bin/bash

PROG=$1
DIR=$2

for TEST in $DIR/*.in;
do
    #TEST=${f%.in}

    #valgrind --error-exitcode=15 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all 
    ./${PROG} < ${TEST} > test.out 2> test.err

    DIFF1=$(diff test.out ${TEST%.in}.out)
    DIFF2=$(diff test.err ${TEST%.in}.err)

    echo -e "$DIFF1"
    echo -e "$DIFF2"
    
    if [[ $DIFF1 == "" && $DIFF2 == "" ]];
    then
      echo -e "$TEST - ok"
    else
      echo -e "$TEST - wrong"
    fi

    rm test.out test.err
done