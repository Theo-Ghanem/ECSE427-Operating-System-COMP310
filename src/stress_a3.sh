#!/bin/bash

touch test_a3_results.txt
for i in {1..1000}
do
    echo "Iteration $i" >> test_a3_results.txt
    ./test_env_a3.sh >> test_a3_results.txt
done