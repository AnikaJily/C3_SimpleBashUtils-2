.PHONY: all s21_grep
SHELL := /bin/bash
FLAG = -Wall -Werror -Wextra
FILE =s21_grep.c

all: s21_grep 
s21_grep: 
	gcc $(FLAG) s21_grep.c -o s21_grep

tests: s21_grep
	-diff <(./s21_grep He $(FILE)) <(grep He $(FILE))
	-diff <(./s21_grep -v He $(FILE)) <(grep -v He $(FILE))
	-diff <(./s21_grep -i He $(FILE)) <(grep -i He $(FILE))
	-diff <(./s21_grep -iv He $(FILE)) <(grep -iv He $(FILE))
	-diff <(./s21_grep -in He $(FILE)) <(grep -in He $(FILE))
	-diff <(./s21_grep -e He $(FILE)) <(grep -e He $(FILE))
	-diff <(./s21_grep -c He $(FILE)) <(grep -c He $(FILE))
	-diff <(./s21_grep -l He $(FILE)) <(grep -l He $(FILE))
	-diff <(./s21_grep -n He $(FILE)) <(grep -n He $(FILE))
	-diff <(./s21_grep -h He $(FILE)) <(grep -h He $(FILE))
	-diff <(./s21_grep -f 2.txt $(FILE)) <(grep -f 2.txt $(FILE))
	

rebuild: clean all

clean:
	rm -rf *.o *.out s21_grep
