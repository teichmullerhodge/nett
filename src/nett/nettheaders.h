#ifndef NETTHEADERS_H
#define NETTHEADERS_H 

#include <stdlib.h>


#define NETTHEADERS_INIT_CAP_SIZE 6

typedef struct {

  char **keys;
  char **values;
  size_t len;
  size_t capacity;

} NettHeaders;

NettHeaders *init_headers(); 

void destroy_headers(NettHeaders *headers);
void nett_headers_add_key_value(NettHeaders *headers, char *key, char *value);
void nett_headers_remove_key(NettHeaders *headers, const char *key);


#endif 
