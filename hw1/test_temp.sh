#!/bin/bash

set +x
set +e

echo "checking for README"
if [ ! -e "./README" ]
then
        echo "Error: No README file"
        exit 1
fi

echo "checking for Makefile"
if [ ! -e "./Makefile" ]
then
        echo "Error: No Makefile file"
        exit 1
fi

echo "Running make"
make
rc=$?
if [ $rc -ne 0 ]
then
        echo "Error when running the make command"
        exit 1
fi

echo "Running make again"
make
rc=$?
if [ $rc -ne 0 ]
then
        echo "Error when running the make command again"
        exit 1
fi

if [ ! -e "./secure_house" ]
then
        echo "Error: Running make did not create the secure_house file"
        exit 1
fi

if [ ! -x "./secure_house" ]
then
        echo "Error: secure_house is not executable"
        exit 1
fi

INPUT_CASE="INSERT KEY david k1
TURN KEY pat
ENTER HOUSE jh
CHANGE LOCKS jh

"

CORRECT_OUTPUT="KEY k1 INSERTED BY david
SUCCESS pat TURNS KEY k1
ACCESS ALLOWED
LOCK CHANGED
"

echo "Testing your program"
OUTPUT=$( echo -n "$INPUT_CASE" | ./secure_house jh k1 k2)

echo "Your program's output is as follows:"
echo "------------------------------------"
echo "$OUTPUT"
echo "------------------------------------"

DIFF=$(diff -aBw <(echo "$OUTPUT") <(echo "$CORRECT_OUTPUT"))
rc=$?
if [ $rc -ne 0 ]
then
	echo "Error: did not pass the basic test case on the website."
	echo "$DIFF"
else
	echo "SUCCESS!"
fi
