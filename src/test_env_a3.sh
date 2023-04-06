#!/bin/bash
cd ..
rm -r env > /dev/null 2>&1
mkdir env
cp -a tc/a3/. env/
cd env
search_dir=$(pwd)

for file in "$search_dir"/*.txt
do
  if [[ $file == *_result*.txt ]]; then
    continue
  fi
  if [[ $file == P_* ]]; then
    continue
  fi
  if [[ $file == T_MT* ]]; then
    continue
  fi
  if [[ $file == T_AGING* ]]; then
    continue
  fi

  total_test_cases=$((total_test_cases + 1))

  result_file="${file%.*}_result.txt"
  frame_store_size=$(grep -o 'Frame Store Size = [0-9]*' "$result_file" | cut -d ' ' -f5)
  variable_store_size=$(grep -o 'Variable Store Size = [0-9]*' "$result_file" | cut -d ' ' -f5)
  cd ../src
  make clean > /dev/null 2>&1
  make mysh framesize=$frame_store_size varmemsize=$variable_store_size > /dev/null 2>&1
  cd ..
  cp src/mysh env/mysh
  chmod +x env/mysh
  cd env

  execution_results=$(mktemp)
  timeout 1 ./mysh < ${file} > ${execution_results}
  diff_output=$(diff --ignore-trailing-space --ignore-space-change ${result_file} ${execution_results})

  if [ -z "${diff_output}" ]; then
    echo -e "\033[32mPASS\033[0m $(basename ${file})"
  else
    failed_test_cases=$((failed_test_cases + 1))
    echo -e "\033[31mFAIL\033[0m $(basename ${file})"
    
    # ! Uncomment the following lines to see the diff output
    # echo
    # echo "Differences between expected output and actual output:"
    # echo
    # echo "${diff_output}"
    # echo
    # echo
  fi
done

if test ${failed_test_cases} -ne 0; then
  echo -e "\033[31mFailed ${failed_test_cases} test cases out of ${total_test_cases}. See details above.\033[0m"
fi
cd ..
rm -r env
cd src
make clean > /dev/null 2>&1
echo