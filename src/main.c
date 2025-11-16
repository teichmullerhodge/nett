#include "nett/nett.h"
#include "nett/nettheaders.h"



int main(int argc, char **argv) {

  (void)argc;
  (void)argv;

  NettResponse *res = init_response();
  NettHeaders *headers = init_headers();
  nett_headers_add_key_value(headers, "Content-Type", "Application/json");


  nett_get("https://ifconfig.me", headers, res);

  printf("GET Response: %s\n", res->contents);
  
  nett_post("https://httpbin.org/post",
          headers,
          "{\"name\":\"Hodge\",\"power\":9000}",
          res);

  printf("POST Response:\n%s\n\n", res->contents);

  nett_put("https://httpbin.org/put",
         headers,
         "{\"field\": \"updated\"}",
         res);
  
  printf("PUT Response:\n%s\n\n", res->contents);

  nett_patch("https://httpbin.org/patch",
           headers,
           "{\"patch\": true}",
           res);
  
  printf("PATCH Response:\n%s\n\n", res->contents);

  nett_delete("https://httpbin.org/delete",
            headers,
            "{\"delete\": true}",
            res);

  printf("DELETE Response:\n%s\n\n", res->contents);



  destroy_response(res);
  destroy_headers(headers);

}
