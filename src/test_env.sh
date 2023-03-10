#!/bin/bash

mkdir myenv
cd src
make 
cd ..
cp src/mysh myenv/mysh
cp src/test_runner_A2.sh myenv/test_runner_A2.sh
cp -a testcases/assignment2/. myenv/
./env/mysh