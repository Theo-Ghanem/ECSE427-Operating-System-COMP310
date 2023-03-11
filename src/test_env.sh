#!/bin/bash
cd ..
rm -r env
mkdir env
cd src
make clean
make mysh
cd ..
cp src/mysh env/mysh
chmod +x env/mysh
cp src/test_runner_A2.sh env/test_runner_A2.sh
chmod +x env/test_runner_A2.sh
cp -a testcases/assignment2/. env/
cd env
./test_runner_A2.sh
cd ..
rm -r env