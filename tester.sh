#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

shopt -s nullglob
declare -i FAILED_TESTS=0
FAILED_LIST=""
FAILED_TESTS_FOLDER="./Failed"

if [ -d "$FAILED_TESTS_FOLDER" ]
	then
		rm -r "$FAILED_TESTS_FOLDER"
	fi
mkdir "$FAILED_TESTS_FOLDER"

g++ -o wet1.exe -std=c++11 -DNDEBUG -Wall *.cpp

for i in Tests/*.in
do
	testName="${i##*/}"
	testName="${testName%.*}"
	printf "<<<<<<<<<<< $testName >>>>>>>>>>>"
	./wet1.exe < $i > ${i%.*}.result
	diff ${i%.*}.result ${i%.*}.out
	#rm ${i%.*}.result

	if [ $? -eq 0 ]
	then
		printf "$testName: ${GREEN}pass${NC},   "
	else
		printf "$testName: ${RED}fail${NC},   "
		FAILED_TESTS+=1
		FAILED_LIST+="$i - OUTPUT\n"
		cp "$i" "$FAILED_TESTS_FOLDER/${i##*/}"
		cp "${i%.*}.out" "$FAILED_TESTS_FOLDER/$testName.out"
	fi
	valgrind --log-file=${i%.*}.valgrind_log --leak-check=full ./wet1.exe < $i > ${i%.*}.vresult 1>/dev/null 2>/dev/null
	rm ${i%.*}.vresult
	if [ -f ${i%.*}.valgrind_log ]
	then
		cat ${i%.*}.valgrind_log | grep "ERROR SUMMARY: 0" > /dev/null
		if [ $? -eq 0 ]
		then
			printf "Leak: ${GREEN}pass${NC}\n"
			rm ${i%.*}.valgrind_log
		else
			printf "Leak: ${RED}fail${NC}\n"
			cat ${i%.*}.valgrind_log
			FAILED_TESTS+=1
			FAILED_LIST+="$i - MEMORY\n"
			cp "$i" "$FAILED_TESTS_FOLDER/${i##*/}"
			cp "${i%.*}.out" "$FAILED_TESTS_FOLDER/$testName.out"
			rm ${i%.*}.valgrind_log
		fi
	else
		printf "Leak: ${RED}couldn't get valgrind file${NC}\n"
		FAILED_TESTS+=1
	fi
done

if [ ${FAILED_TESTS} -eq 0 ]; then
	printf "\n${GREEN} All tests passed :)${NC}\n\n"
else
	printf "\n${RED} Failed ${FAILED_TESTS}${NC} tests.\n${FAILED_LIST}\n\n"
fi
