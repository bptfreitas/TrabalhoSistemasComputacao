#!/bin/bash

OUTPUT=$1

adds=""

for file in $(ls *.in); do 

	adds="$adds --add-file=$file"

done

#echo $adds

git archive --format=zip --prefix=$1/ $adds -o $OUTPUT.zip HEAD
