#include "gc_test.h"

#include <stdio.h>
//#include <unistd.h> // usleep()

#include <gc/gc.h>
#include <gc/gc_cpp.h>
#include <gc/gc_allocator.h>
#include <gc/javaxfc.h>

#include <vector>

static int global_cnt = 0;

class Hoge: public gc_cleanup
{
  char* ptr;
  int cnt;
public:
  Hoge(): cnt(++global_cnt) {
    printf("hoge ctor: %d\n", cnt);
    ptr = new (GC) char[100000];
  }

  virtual ~Hoge() { printf("     dtor: %d\n", cnt); }
};

void cpp_test() {
  auto vec = new(GC) std::vector< Hoge *, gc_allocator<Hoge *> >();
  for (int i=0; i<100; i++)
  {
      vec->push_back(new(GC) Hoge());
  }
}

int gc_test(int argc, char **argv)
{
  GC_INIT();

  for(int i=0; i<100; i++) cpp_test();
  //usleep(0);
  GC_gcollect();

  //GC_finalize_all();

  return 0;
}
