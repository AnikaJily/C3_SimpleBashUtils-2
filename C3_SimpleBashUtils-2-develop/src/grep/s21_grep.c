#include "s21_grep.h"

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pattern_add(arguments* arg, char* pattern) {  //для флага f
  if (arg->len_pattern != 0) {  //не первое добавление
    strcat(arg->pattern + arg->len_pattern,
           "|");  //если там чето уже есть, соответственно добавляем эту штуку
    arg->len_pattern++;
  }
  arg->len_pattern +=
      sprintf(arg->pattern + arg->len_pattern, "(%s)",
              pattern);  //почти то же самое, но печается не на экран а в строку
                         //также возвращает кол-во символов которое записалось
}

void add_reg_from_file(arguments* arg, char* filepath) {
  //функция для чтения регулярных выражений из файла
  FILE* f = fopen(filepath, "r");
  if (f == NULL) {  //существует ли
    if (!arg->s) perror(filepath);  //тк s подавляет сообщение об ошибке
    //вывод сообщения об ошибке
    exit(1);  //завершается с кодом ошибки 1 ЗАЧЕММММММММ
  }
  //чтение файла построчно
  char* line = NULL;  //строка, куда бубудт считываться строки из файла
  size_t memlen = 0;  //содержит размер выделенной памяти для line
  int read = getline(
      &line, &memlen,
      f);  //считывает строку из файла f и сохраняет ее в переменную line
  //возвращает кол-во считанных символов
  while (read != -1) {  //продолжается пока не конец файла и пока не произошла
                        //ошибка чтения
    if (line[read - 1] == '\n') line[read - 1] = '\0';
    pattern_add(arg, line);  //вызывается для каждой строки в файле
    read = getline(&line, &memlen, f);
  }
  free(line);  //высвобождение выделенной памяти и закрытие файла
  //?
  fclose(f);
}

arguments argument_parser(int argc, char* argv[]) {
  arguments arg = {0};
  static struct option long_options[] = {

      {"case-insensitive", no_argument, 0, 'i'},
      {"version", no_argument, 0, 'v'},
      {"count", no_argument, 0, 'c'},
      {"lines", no_argument, 0, 'l'},
      {"line-number", no_argument, 0, 'n'},
      {"help", no_argument, 0, 'h'},
      {"silent", no_argument, 0, 's'},
      {"file", no_argument, 0, 'f'},
      {"output", no_argument, 0, 'o'},
      {"expression", required_argument, 0, 'e'},
      {0, 0, 0, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'e':
        arg.e = 1;
        pattern_add(&arg, optarg);
        /*arg.pattern = optarg;*/  //глобальная переменная типа char*
        //хранит в себе указатель на аргумент, который идет после опции
        break;
      case 'i':
        arg.i = REG_ICASE;  //указывает на то, что рег выражение должно быть
                            //искомо без учета регистра символов
        break;
      case 'v':
        arg.v = 1;
        break;
      case 'c':
        arg.c = 1;
        break;
      case 'l':
        arg.l = 1;
        break;
      case 'n':
        arg.n = 1;
        break;
      case 'h':
        arg.h = 1;
        break;
      case 's':
        arg.s = 1;
        break;
      case 'f':
        arg.f = 1;
        add_reg_from_file(&arg, optarg);
        break;
      case 'o':
        arg.o = 1;
        break;
      default:
        exit(1);
    }
  }
  if (arg.len_pattern == 0) {  //проверяем было ли явно указано рег выражение с
                               //помощью опции -е. то есть если не было -е
    //чтобы кусок не воспринимался как путь до файла
    pattern_add(&arg, argv[optind]);  // optind указывает на следующий аргумент
    optind++;
  }
  if (argc - optind == 1) {  //если у нас 1 файл мы не выводим его название
    arg.h = 1;
  }
  return arg;
}

//функция вывода линии по символу
void output_line(char* line, int n) {
  for (int i = 0; i < n; i++) {
    putchar(line[i]);
  }
  if (line[n - 1] != '\n')
    putchar('\n');  //чтобы если в конце файла нет переноса не слепливалось
}

void print_match(regex_t* re, char* line,
                 arguments arg) {  //поиск и печать всех совпадений реги в
                                   //заданной строке line
  regmatch_t match;  //будет хранить инфу о найденном совпадении
  // int offset = 0;  //смещение в строке, 0 - начало строки
  if (!arg.v) {
    int offset = 0;
    while (1) {  //беск цикл, продолжается пока не будут найдены все совпадения
      int result =
          regexec(re, line + offset, 1, &match,
                  0);  //ищем совпадение, начиная с позиции оффсет в лайне
                       //
      if (result != 0) {  //если рез поиска != 0 (т.е совпадение не найдено,
                          //цикл завершается)
        break;
      }
      for (int i = match.rm_so; i < match.rm_eo; i++) {
        //перебор от начального индекса совпадения до коненого.
        putchar(line[i + offset]);  //печатаем каждый символ
      }
      putchar('\n');
      offset += match.rm_eo;
    }
  }
}

// обработка файла
void processFile(arguments arg, char* path, regex_t* reg) {
  FILE* f = fopen(path, "r");
  if (f == NULL) {  //существует ли
    if (!arg.s) perror(path);  //тк s подавляет сообщение об ошибке
    //вывод сообщения об ошибке
    // exit(1); //завершается с кодом ошибки 1 ЗАЧЕММММММММ
    return;
  }
  //чтение файла построчно
  char* line = NULL;  //строка, куда бубудт считываться строки из файла
  size_t memlen = 0;  //содержит размер выделенной памяти для line
  int read = 0;
  int line_count = 1;
  int result_count = 0;
  read = getline(
      &line, &memlen,
      f);  //считывает строку из файла f и сохраняет ее в переменную line
  if (arg.v) arg.o = 0;
  while (read != -1) {  //продолжается пока не конец файла и пока не произошла
                        //ошибка чтения

    int result = regexec(reg, line, 0, NULL,
                         0);  //эта функция выполняет поиск совпадений между
                              //сторокой и рег выражением. Если найдено - 0
    // reg - скомп рег выражение, line - строка, где выполняется поиск
    //РЕАЛИЗАЦИЯ V
    if ((result == 0 && !arg.v) || (arg.v && result != 0)) {
      if (!arg.c && !arg.l) {
        if (!arg.h) printf("%s:", path);  //ЗАЧЕМ В ДВУХ МЕСТАХ ДЕЛАТЬ
        if (arg.n) printf("%d:", line_count);
        if (arg.o) {
          print_match(reg, line, arg);
        } else {
          output_line(line, read);
        }  //если совпадает с результатом то выводим
      }
      result_count++;
    }
    read = getline(&line, &memlen, f);
    line_count++;
    //?
  }

  free(line);  //высвобождение выделенной памяти и закрытие файла
  if (arg.c && !arg.l) {
    if (!arg.h) printf("%s:", path);
    printf("%d\n", result_count);
  }
  if (arg.l) {
    if (result_count == 0) {
      if (arg.c) {
        printf("%s:0\n", path);
      }
    } else {
      if (arg.c) {
        printf("%s:1\n", path);
      }
      printf("%s\n", path);
    }
  }
  fclose(f);
}

void output(arguments arg, int argc, char* argv[]) {
  regex_t re;  //структура для хранения скомпилированного рег выражения
  // regcomp компилирует регулярное выражение из строки, которая находиться в
  // паттерне и сохраняет в ре РЕАЛИЗАЦИЯ I printf("%s, %d!!", arg.pattern,
  // arg.len_pattern);
  int error =
      regcomp(&re, arg.pattern,
              REG_EXTENDED | arg.i);  // ссылка на структуру, паттерн, флаг.
                                      // если все ок - возвращает 0
  // arg.i по сути это флаг регкомпа REG_ICASE, то есть игнор регистра
  if (error) perror("Error");
  for (int i = optind; i < argc; i++) {
    processFile(arg, argv[i], &re);
  }
  regfree(&re);
}

int main(int argc, char* argv[]) {
  arguments arg = argument_parser(argc, argv);
  output(arg, argc, argv);
  return 0;
}
