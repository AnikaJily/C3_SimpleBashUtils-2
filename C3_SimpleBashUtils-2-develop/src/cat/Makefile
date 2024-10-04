.PHONY: all s21_cat
SHELL := /bin/bash
FLAG = -Wall -Werror -Wextra
FILE =s21_cat.c

all: s21_cat 
s21_cat: 
	gcc $(FLAG) s21_cat.c -o s21_cat

tests: s21_cat
	-diff <(./s21_cat $(FILE)) <(cat $(FILE))
	-diff <(./s21_cat -v $(FILE)) <(cat -v $(FILE))
	-diff <(./s21_cat -t $(FILE)) <(cat -t $(FILE))
	-diff <(./s21_cat -e $(FILE)) <(cat -e $(FILE))
	-diff <(./s21_cat -b $(FILE)) <(cat -b $(FILE))
	-diff <(./s21_cat -n $(FILE)) <(cat -n $(FILE))
	-diff <(./s21_cat -s $(FILE)) <(cat -s $(FILE))
	-diff <(./s21_cat -enst $(FILE)) <(cat -enst $(FILE))
	-diff <(./s21_cat --number-nonblank $(FILE)) <(cat -b $(FILE))
	-diff <(./s21_cat --squeeze-blank $(FILE)) <(cat -s $(FILE))
	-diff <(./s21_cat --number $(FILE)) <(cat -n $(FILE))

rebuild: clean all

clean:
	rm -rf *.o *.out s21_cat

