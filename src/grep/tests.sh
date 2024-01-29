#!/bin/bash
flags=( "i" "v" "c" "l" "n" "h" "s" "o")

tests=("pattern test.txt")
SUCCESS=0
FAIL=0
COUNTER=0
DIFF=""

# Функция для запуска тестов
run_test() {
    # Запуск s21_grep и grep с одинаковыми параметрами
    ./s21_grep "$@" > s21_output.txt
    grep "$@" > output.txt
		let "COUNTER++"
    # Сравнение вывода
    diff s21_output.txt output.txt
    if [ $? -eq 0 ]; then
				let "SUCCESS++"
        echo "$COUNTER - Success: ./s21_grep $@"
    else
		    let "FAIL++"
        echo "$COUNTER - Fail: ./s21_grep $@"
    fi
}


echo "#######################"
echo "SINGLE FLAG TESTS"
echo "#######################"
printf "\n"
run_test -e name test.txt
run_test -i name test.txt
run_test -v name test.txt
run_test -c name test.txt
run_test -l name test.txt
run_test -n name test.txt
run_test -h name test.txt
run_test -s name test.txt
run_test -o name test.txt
run_test -f file.txt test.txt


echo "#######################"
echo "MULTI FLAG TESTS"
echo "#######################"
printf "\n"

# Тесты
run_test -e my -e name -e name test.txt
run_test -e my -e qwe test.txt
run_test -ie Name test.txt
run_test -ve name test.txt
run_test -ce name test.txt
run_test -le name test.txt
run_test -ne name test.txt
run_test -he name test.txt
run_test -se name test.txt
run_test -oe name test.txt
run_test -ic Name test.txt
run_test -vc name test.txt
run_test -lc name test.txt
run_test -nc name test.txt
run_test -hc name test.txt
run_test -sc name test.txt
run_test -on name test.txt
run_test -vn name test.txt
run_test -cn name test.txt
run_test -ln name test.txt
run_test -hn name test.txt
run_test -sn name test.txt
run_test -on name test.txt
run_test -il Name test.txt
run_test -vl name test.txt
run_test -cl name test.txt
run_test -nl name test.txt
run_test -hl name test.txt
run_test -sl name test.txt
run_test -ol name test.txt
run_test -io Name test.txt
run_test -vo name test.txt
run_test -co name test.txt
run_test -lo name test.txt
run_test -no name test.txt
run_test -ho name test.txt
run_test -so name test.txt
run_test -iv name test.txt
run_test -cl name test.txt
run_test -cv name test.txt
run_test -lv name test.txt
run_test -nv name test.txt
run_test -hv name test.txt
run_test -sv name test.txt
run_test -ov name test.txt

run_test -ive name test.txt
run_test -cle name test.txt
run_test -nhe name test.txt
run_test -sf file.txt test.txt
run_test -oive name test.txt

printf "\n"
echo "FAILED: $FAIL"
echo "SUCCESSFUL: $SUCCESS"
echo "ALL: $COUNTER"
printf "\n"