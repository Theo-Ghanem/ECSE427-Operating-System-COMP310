#!/bin/bash

# Change this variable to run another assignment tests
search_dir=$(pwd)/../testcases/assignment1

echo
echo -e "Running all test cases in \033[1m$(basename ${search_dir})/\033[0m:"
echo

total_test_cases=0
failed_test_cases=0

for file in "$search_dir"/*.txt
do
  if [[ $file == *_result.txt ]]; then
    continue
  fi

  total_test_cases=$((total_test_cases + 1))

  result_file="${file%.*}_result.txt"
  execution_results=$(mktemp)
  timeout 1 ./mysh < ${file} > ${execution_results}
  diff_output=$(diff --ignore-trailing-space --ignore-space-change ${result_file} ${execution_results})

  if [ -z "${diff_output}" ]; then
    echo -e "\033[32mPASS\033[0m $(basename ${file})"
  else
    failed_test_cases=$((failed_test_cases + 1))
    echo -e "\033[31mFAIL\033[0m $(basename ${file})"

    echo
    echo "Differences between expected output and actual output:"
    echo
    echo "${diff_output}"
    echo
    echo
  fi
done

if test ${failed_test_cases} -ne 0; then
  echo -e "\033[31mFailed ${failed_test_cases} test cases out of ${total_test_cases}. See details above.\033[0m"
fi

echo