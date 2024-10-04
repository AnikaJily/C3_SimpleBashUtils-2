#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct arguments {
  bool non_blank_rows;  //-b нумерует непустые строки
  bool show_end;  //-e отображает символы конца строки - выводит \n как $.
  bool all_rows;  //-n нумерует все выходные строки
  bool squeezed;  //-s сжимает несколько смежных пустых строк. не больше 1
                  //пустой строки подряд
  bool show_tabs;  //-t отображает табы как ^|
  bool show_all;  //-v заменяет все управляющие символы (неотображаемые) как ^
                  //или M- обохначениях (кроме /n and \t)
  int count;  //кол-во строк / ошибки которые будем обрабатывать
  int empty_lines;  //кол-во пустых строк
} arguments;

/*
int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) { // цикл начинается с argv[1]
    print_file(argv[i]);           // в argv[0] находится "./s21_cat"
  }
  return 0;
}
*/

void cook_non_printed(int *c, arguments *s_arg) {
  if (*c == '\n') {
    if (s_arg->show_end) printf("$");
  } else if (*c == '\t') {
    if (s_arg->show_tabs) {
      printf("^");
      *c = 'I';
    }
  } else if (s_arg->show_all) {
    if (*c <= 31) {
      printf("^");
      *c += 64;
    } else if (*c == 127) {
      printf("^");
      *c = '?';
    } else if (*c >= 128 && *c < 128 + 32) {
      printf("M-^");
      *c -= 64;
    }
  }
}

void cook_empty(const int *c, const int *prev, arguments *s_arg) {
  if (*prev == '\n' && *c == '\n')
    s_arg->empty_lines++;
  else
    s_arg->empty_lines = 0;
}
void cook_squeezed(int *c, FILE *f, arguments *s_arg) {
  if (s_arg->squeezed && s_arg->empty_lines > 1) {
    while (*c == '\n') {
      *c = fgetc(f);
    }
    s_arg->empty_lines = 0;
  }
}

void cook_numbers(const int *prev, arguments *s_arg) {
  if (*prev == '\n' && (s_arg->all_rows || s_arg->non_blank_rows)) {
    if (!(s_arg->non_blank_rows && s_arg->empty_lines > 0)) {
      printf("%6d\t", s_arg->count);
      s_arg->count++;
    }
  }
}

void cat_cook(int *c, int *prev, arguments *s_arg, FILE *f) {
  cook_empty(c, prev, s_arg);
  cook_squeezed(c, f, s_arg);
  if (*c != EOF) {
    cook_numbers(prev, s_arg);
    cook_non_printed(c, s_arg);  //ЗАЧЕМ ПРОВЕРКА НА ЕОФ???
  }
}

void cat_print(FILE *f, arguments *s_arg) {
  int c;
  int prev = '\n';
  if (s_arg) {
    s_arg->empty_lines = 0;
    s_arg->count = 1;
  }
  while ((c = fgetc(f)) != EOF) {
    if (s_arg) {
      cat_cook(&c, &prev, s_arg, f);
    }
    if (c != EOF) fputc(c, stdout);
    prev = c;
  }
}

void cat_file(int argc, char *argv[], arguments *s_arg) {
  /* int i = 0;
   if (s_arg == NULL) {
       i = 1;
   } else {
       if (s_arg->count == 0) {
           i = 1;
       } else {
           i = 2;
       }
   }*/
  int i = s_arg ? 1 + s_arg->count : 1;
  for (; i < argc; i++) {           // цикл начинается с argv[1]
    FILE *f = fopen(argv[i], "r");  // в argv[0] находится "./s21_cat"
    if (f != NULL) {
      cat_print(f, s_arg);
      fclose(f);
    } else {
      fprintf(stderr, "cat: %s: No such file or directory\n", argv[i]);
    }
  }
}

void parse_short(char *argv, arguments *s_arg) {
  int len = strlen(argv);
  for (int j = 1; j < len; j++) {
    if (s_arg->count) {
      switch (argv[j]) {  //сравниваем с аргументом в скобках
        case 'b':
          s_arg->non_blank_rows = true;
          break;
        case 'e':
          s_arg->show_end = true;
          s_arg->show_all = true;
          break;
        case 'E':
          s_arg->show_end = true;
          break;
        case 'n':
          s_arg->all_rows = true;
          break;
        case 's':
          s_arg->squeezed = true;
          break;
        case 't':
          s_arg->show_tabs = true;
          s_arg->show_all = true;
          break;
        case 'T':
          s_arg->show_tabs = true;
          break;
        case 'v':
          s_arg->show_all = true;
          break;
        default:  //если неверный флаг
          s_arg->count = 0;
          break;
      }
    } else {
      break;  //он же всегда ненулевой зачем проверять
    }
  }
}

void parse_options_long(char *argv, arguments *s_arg) {
  if (!strcmp(argv + 2, "number-nonblank"))
    s_arg->non_blank_rows = true;
  else if (!strcmp(argv + 2, "number"))
    s_arg->all_rows = true;
  else if (!strcmp(argv + 2, "squeeze-blank"))
    s_arg->squeezed = true;
  else {
    s_arg->count = 0;
  }
}

void parse_args(int argc, char *argv[], arguments *s_arg) {
  for (int i = 1; i < argc; i++) {  //флаг же должен быть первым аргументом,
                                    //зачем остальные проверять???
    if (argv[i][0] == '-') {  //если флаг больше нигде не найден
      s_arg->count++;  //такой знак, тк у нас указатель на структуру
      if (!strstr(argv[i] + 1, "-"))
        parse_short(argv[i], s_arg);
      else if (argv[i][1] == '-')
        parse_options_long(argv[i], s_arg);
    }
    if (!s_arg->count)
      break;  //если после всех манипуляций выше каунт так и остался 0, то есть
              //флаг был введен неверно, мы уходим
              //МОЖНО ЖЕ ПРОСТО ELSE???
  }
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    cat_print(stdin, NULL);  // when there's only 1 arg, read from stdin
  } else {
    if (argv[1][0] == '-') {
      arguments s_arg = {0};
      parse_args(argc, argv, &s_arg);
      if (!s_arg.count) {
        fprintf(stderr, "s21_cat: invalid options\n");
      } else {
        cat_file(argc, argv, &s_arg);
      }
    } else {
      cat_file(argc, argv, NULL);  //работа с файлом без опций
    }
    return 0;
  }
}
