#!/bin/bash

rm -rf monitored

mkdir monitored

monitor_path="`pwd`/monitored"

./trabalho-sistemas-computacao $monitor_path `pwd` &

sleep 1;

cp matrixes1.in $monitor_path/matrix1.ready

cp matrixes1.in $monitor_path/matrix2.ready

sleep 1;