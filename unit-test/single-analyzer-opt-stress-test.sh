#!/bin/bash

FILES="/var/www/mobile_insight/upload/"

OUTPUT="./stress-test-result.txt"


rm $OUTPUT


#for f in $(find $FILES -name '*.mi2log' -or -name '*.qmdl');
for f in $FILES*;
do
    for ff in "$f"/*;
    do
        echo "Test with $ff">> $OUTPUT
        python single-analyzer-generic-test.py "$ff" 2>> $OUTPUT
        echo "====\n\n">>$OUTPUT
    done
done


