#include "nett.h"
#include <curl/curl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static CURL *nett_ref_handler = NULL;
atomic_int initialized = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

CURL *get_handler(){

    (void)lock;
    if (initialized == 0){
        atomic_fetch_add(&initialized, 1);
        curl_global_init(CURL_GLOBAL_ALL);
    }

    nett_ref_handler = curl_easy_init();
    return nett_ref_handler;
}

void reset_response(NettResponse *res){

    if (res){
        res->error = 0;
        if (res->contents != NULL)
            free(res->contents);
        res->contents = malloc(1);
        res->status_code = 0;
        res->contents_size = 0;
    }
}

NettResponse *init_response(){
    NettResponse *res = malloc(sizeof(NettResponse));
    if (res == NULL){
        perror("Malloc failed at response_init.\n");
        return NULL;
    }

    res->contents = malloc(1);
    if (res->contents == NULL){
        free(res);
        perror("Malloc failed at contents in response_init.\n");
        return NULL;
    }
    res->error = 0;
    res->status_code = 0;
    res->contents_size = 0;
    return res;
}

void destroy_response(NettResponse *res){

    if (res != NULL && res->contents != NULL){
        free(res->contents);
        free(res);
    }
}

size_t write_memory_callback(const void *contents, size_t size, size_t nmemb, void *user_data){

    size_t real = size * nmemb;
    NettResponse *res = (NettResponse *)user_data;
    char *ptr = realloc(res->contents, res->contents_size + real + 1);
    if (ptr == NULL){
        perror("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    res->contents = ptr;
    memcpy(&(res->contents[res->contents_size]), contents, real);
    res->contents_size += real;
    res->contents[res->contents_size] = '\0';
    return real;
}


struct curl_slist *set_headers(const NettHeaders *headers){
    struct curl_slist *h = NULL;
    if (headers == NULL)
        return h;
    for (size_t k = 0; k < headers->len; k++){
        if(headers->keys[k] == NULL || headers->values[k] == NULL) continue; // deleted keys are null.
        char full_header[1024];
        snprintf(full_header, sizeof(full_header), "%s: %s", headers->keys[k], headers->values[k]);
        h = curl_slist_append(h, full_header);
    }

    return h;

}

void initiate_request(const char *url, NettHeaders *headers, NettResponse *res) {
    CURL *handler = get_handler();
    reset_response(res);
    if (handler == NULL) {
        perror("Internal handler is null.");
        return;
     }
    struct curl_slist *h = set_headers(headers);

    curl_easy_setopt(handler, CURLOPT_HTTPHEADER, h);
    curl_easy_setopt(handler, CURLOPT_URL, url);
    curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(handler, CURLOPT_WRITEDATA, (void *)res);
    res->__handler = handler;
    res->__headers_ref = h;
    return;
}


void terminate_request(NettResponse *res){

    long code = 0;
    curl_easy_getinfo(res->__handler, CURLINFO_RESPONSE_CODE, &code);
    res->status_code = (HttpStatus)code;
    
    curl_slist_free_all(res->__headers_ref);
    curl_easy_cleanup(res->__handler);
  
}

bool has_result_error(CURLcode res){

  if(res != CURLE_OK){
     char error[512];
     snprintf(error, sizeof(error), "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
     perror(error);
     return true;
   }

   return false;
}

void nett_custom_request(const char* method, const char *url, NettHeaders *headers, const char *body, NettResponse *res){

    if(body == NULL && strcmp(method, "DELETE") != 0) { 
       perror("Body cannot be null."); 
       return; 
    }
    initiate_request(url, headers, res);
    if (res->__handler == NULL)
        return;
    curl_easy_setopt(res->__handler, CURLOPT_CUSTOMREQUEST, method);
    curl_easy_setopt(res->__handler, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(res->__handler, CURLOPT_POSTFIELDSIZE, strlen(body));

    CURLcode result = curl_easy_perform(res->__handler);
    has_result_error(result);
    terminate_request(res);
    return;

}

void nett_get(const char *url, NettHeaders *headers, NettResponse *res){

    initiate_request(url, headers, res);
    if (res->__handler == NULL)
        return;
    CURLcode result = curl_easy_perform(res->__handler);
    has_result_error(result);
    terminate_request(res);
    return;

}

void nett_post(const char *url, NettHeaders *headers, const char *body, NettResponse *res){

    if(body == NULL) { 
       perror("Body cannot be null."); 
       return; 
    }
    initiate_request(url, headers, res);
    if (res->__handler == NULL)
        return;
    curl_easy_setopt(res->__handler, CURLOPT_POST, 1L);
    curl_easy_setopt(res->__handler, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(res->__handler, CURLOPT_POSTFIELDSIZE, strlen(body));
    
    CURLcode result = curl_easy_perform(res->__handler);
    has_result_error(result);
    terminate_request(res);   
    return;
}

void nett_patch(const char *url, NettHeaders *headers, const char *body, NettResponse *res){
  return nett_custom_request("PATCH", url, headers, body, res);
}

void nett_put(const char *url, NettHeaders *headers, const char *body, NettResponse *res){
  return nett_custom_request("PUT", url, headers, body, res);
}

void nett_delete(const char *url, NettHeaders *headers, const char *body, NettResponse *res){
  return nett_custom_request("DELETE", url, headers, body, res);
}

