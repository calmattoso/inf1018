#include <stdio.h>
#include "bigint.h"

int main(){
  BigInt a, b, res;
  int _a, _b;


  while(1){
    scanf("%d %d", &_a, &_b);

    big_val(a, _a);
    big_print(a);

    big_val(b, _b);
    big_print(b);

    printf("Sum\n");
    big_sum(res, a, b);
    big_print(res);

    printf("Subtraction\n");
    big_sub(res, a, b);
    big_print(res);
  }

  return 0;
}