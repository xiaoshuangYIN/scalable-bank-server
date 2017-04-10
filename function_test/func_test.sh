#!/bin/bash

../server &
sleep 1
# valid reset = true
for request in  *valid.xml
do
    echo $request
    ./client localhost $request  > temp

    response=${request}.res

    diff temp $response
    if [ "$?" != "0" ]
    then
	echo $request not get correct response
    else
	echo $request get correct response
    fi
done

# reset = false
for request in  *reset_false.xml
do
    echo $request
    ./client localhost $request  > temp

    response=${request}.res

    diff temp $response
    if [ "$?" != "0" ]
    then
	echo $request not get correct response
    else
	echo $request get correct response
    fi
done

# error tests
for request in  *err.xml
do
    echo $request
    ./client localhost $request  > temp

    response=${request}.res

    diff temp $response
    if [ "$?" != "0" ]
    then
	echo $request not get correct response
    else
	echo $request get correct response
    fi
done

rm temp
