#ifndef MYSTRING_H
#define MYSTRING_H

int  my_strlen(const char *s);
int  my_strcmp(const char *a, const char *b);
void my_strcpy(char *dest, const char *src);

/* Convert integer n to decimal string in buf; returns number of chars written */
int  my_itoa(int n, char *buf);

/* Find first occurrence of c in s; returns pointer to it or 0 if not found */
const char *my_strchr(const char *s, char c);

/* Split src on delim, store up to max_tokens pointers in tokens[].
   src is modified in-place (NUL bytes inserted). Returns token count. */
int  my_tokenize(char *src, char delim, char **tokens, int max_tokens);

#endif