#!/bin/bash
PROJECT="$1"
FOLDER="$2"
BLANK='\033[0m'
RED='\033[0;31m'
GREEN='\033[0;32m'

for f in "${FOLDER}"/*.in
do
	NAME=${f%.in}
	NAME=${NAME#"${FOLDER}"/}
	
	valgrind --error-exitcode=15 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all -q\
	"${PROJECT}" <$f >${f%.in}mine.out 2>${f%.in}mine.err
	
	if diff ${f%.in}mine.out ${f%.in}.out > /dev/null || diff ${f%.in}mine.err ${f%.in}.err > /dev/null
	then
		echo -e "$GREEN$NAME - OK!$BLANK"
	else
		echo -e "$RED$NAME - WRONG!$BLANK"
	fi
	rm ${f%.in}mine.out ${f%.in}mine.err
done