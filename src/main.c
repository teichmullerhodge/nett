#include "nett/nett.h"
#include "suspend/suspend.h"
#include <unistd.h>
void *async_post(){

  NettResponse *res = init_response();
  nett_post("https://httpbin.org/post",
            NULL,
            "{\"name\":\"Hodge\",\"power\":9000}",
            res);

  return res;
}


int main(int argc, char **argv){

  (void)argc;
  (void)argv;

  Suspend *s = init_suspend();
  s->cb = async_post;

  Eventually res = suspend(s);
  res.await(res.id);
  NettResponse *req = s->data;
  printf("Contents: %s\n", req->contents);
  printf("All done!");

  destroy_response(req);
  suspend_destroy(s);
  nett_end();
}
