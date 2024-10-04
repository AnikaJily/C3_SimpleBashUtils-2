#ifndef GREP_H
#define GREP_H

//е - используем для более сложные регулярные выражений, например куда будут
//входить ^, [], $ и тд без е кусок текста - просто шаблон, а не регвыражение
typedef struct arguments {
  int e, i, v, c, l, n, h, s, f, o;
  char pattern[1024];  // string that contains a regular expression

  int len_pattern;

} arguments;

#endif