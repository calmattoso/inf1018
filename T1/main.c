#include <stdio.h>
#include "bigint.h"

int main(){
  BigInt a, b;

  big_val(a, -25);
  big_print(a);

  big_uval(b, 1024);
  big_print(b);

  return 0;
}