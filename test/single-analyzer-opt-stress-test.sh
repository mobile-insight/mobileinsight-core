#!/bin/bash

FILES="/var/www/mobile_insight/upload/"

OUTPUT="./stress-test-result.txt"

rm $OUTPUT


for f in $(find $FILES -name '*.mi2log' -or -name '*.qmdl');
do
	echo "Test with $f">> $OUTPUT

	python single-analyzer-generic-test.py $f 2>> $OUTPUT
	echo "====\n\n">>$OUTPUT
done


