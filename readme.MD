# nett
A lightweight HTTP sync/async client wrapper built on top of libcurl, written in C.  
It supports GET, POST, PUT, PATCH and DELETE requests, custom headers, and buffered responses.

## Features
- Simple synchronous and asynchronous HTTP requests  
- GET, POST, PUT, PATCH and DELETE  
- Custom headers  
- Automatic response buffer growth using realloc  
- Minimal and dependency-free (besides libcurl)  
- Thread-safe initialization of libcurl  

## Requirements
- GCC or Clang  
- libcurl  
- pthreads  

## Improvements 
- Improve async support
- Add perform_all methods to make multiple requests in parallel
