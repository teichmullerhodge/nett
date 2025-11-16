#include "nettheaders.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


NettHeaders *init_headers(){

  NettHeaders *headers = malloc(sizeof(NettHeaders));
  if(headers == NULL) {
    perror("Malloc failed at init_headers");
    return NULL;
  }
  
  headers->keys = malloc(sizeof(char*) * 6);
  if(headers->keys == NULL){
    perror("Malloc failed at headers->keys init.");
    free(headers);
    return NULL;

  }
  headers->values = malloc(sizeof(char*) * 6);
  if(headers->values == NULL){
    perror("Malloc failed at headers->values init.");
    free(headers->keys);
    free(headers);
    return NULL;

  }

  headers->len = 0;
  headers->capacity = NETTHEADERS_INIT_CAP_SIZE;
  return headers;
}

bool valid_headers(const NettHeaders *headers) {
  if(headers == NULL) { 
    perror("Headers are NULL.");
    return false;
  }
  if(headers->keys == NULL || headers->values == NULL){
    perror("Headers keys and values cannot be null.");
    return false;
  }

  return true;
 
}

void destroy_headers(NettHeaders *headers){

  if(headers == NULL) return;
  if(headers->keys != NULL){
    free(headers->keys);
  }
  if(headers->values != NULL){
    free(headers->values);
  }

  free(headers);
  return;


}
void nett_headers_add_key_value(NettHeaders *headers, char *key, char *value){

  bool ok_headers = valid_headers(headers);
  if(!ok_headers) return;

  if(headers->len >= headers->capacity) {
    headers->keys = realloc(headers->keys, sizeof(char*) * (headers->capacity * 2));
    if(headers->keys == NULL) { 
      perror("Could not realloc headers keys at nett_headers_add_key_value.");
      return;
  }
  headers->values = realloc(headers->values, sizeof(char*) * (headers->capacity * 2));
    if(headers->values == NULL) { 
      perror("Could not realloc headers values at nett_headers_add_key_value.");
      return;
  }
  
  }

  headers->keys[headers->len] = key;
  headers->values[headers->len] = value;
  headers->len++;

}
void nett_headers_remove_key(NettHeaders *headers, const char *key){
  bool ok_headers = valid_headers(headers);
  if(!ok_headers) { return; }
  for(size_t k = 0; k < headers->len; k++) {
    if(strcmp(headers->keys[k], key) == 0){
      headers->keys[k] = NULL;
      headers->values[k] = NULL;
    }
  }
}
