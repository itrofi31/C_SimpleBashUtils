CC = gcc -g -Wall -Werror -Wextra -std=c11

all: s21_grep s21_cat

# cat

s21_cat: cat/s21_cat.o cat/cat_funcs.o
	$(CC) cat/s21_cat.o cat/cat_funcs.o -o cat/s21_cat

s21_cat.o: cat/s21_cat.c
	$(CC) -c cat/s21_cat.c -o cat/s21_cat.o

cat_funcs.o: cat/cat_funcs.c
	$(CC) -c cat/cat_funcs.c -o cat/cat_funcs.o

# grep

s21_grep: grep/s21_grep.o grep/grep_funcs.o
	$(CC) grep/s21_grep.o grep/grep_funcs.o -o grep/s21_grep

s21_grep.o: grep/s21_grep.c
	$(CC) -c grep/s21_grep.c -o grep/s21_grep.o

grep_funcs.o: grep/grep_funcs.c
	$(CC) -c grep/grep_funcs.c -o grep/grep_funcs.o

# test

cat_tests: cat/./tests.sh
	bash cat/./tests.sh

grep_tests: grep/./tests.sh
	bash grep/./tests.sh

clean: 
	rm -rf cat/s21_cat cat/*.o grep/s21_grep grep/*.o

rebuild: clean all
