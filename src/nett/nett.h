#ifndef NETT_H 
#define NETT_H 

#include <stdlib.h>
#include "nettheaders.h"
#include "nett_enumerations.h"
#include <curl/curl.h>

typedef struct {

  char *contents;
  size_t contents_size;
  int error;
  HttpStatus status_code;


  // internal data
  CURL* __handler;
  struct curl_slist *__headers_ref;



} NettResponse;


NettResponse *init_response();
void destroy_response(NettResponse *res);



void nett_get(const char *url, NettHeaders *headers, NettResponse *res);
void nett_post(const char *url, NettHeaders *headers, const char *body, NettResponse *res);
void nett_patch(const char *url, NettHeaders *headers, const char *body,NettResponse *res);
void nett_put(const char *url, NettHeaders *headers, const char *body,NettResponse *res);
void nett_delete(const char *url, NettHeaders *headers, const char *body,NettResponse *res);






#endif 
