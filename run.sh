#!/bin/bash

numTrials=$1
totalScore=0

#empty results file
>results

counter=1

while [ $counter -le $numTrials ]
do
   score=`./game $RANDOM 2>&1 >/dev/null | egrep -o '[0-9]+'`
   totalScore=`expr $totalScore + $score`
   echo "Round $counter: $score" >>results
   counter=`expr $counter + 1`
done
avgScore=`expr $totalScore / $numTrials`
echo "Average score of $avgScore over $numTrials trials"
echo "Round results stored in 'results' file"
