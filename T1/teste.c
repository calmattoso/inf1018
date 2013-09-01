#include <stdio.h>
#include "bigint.h"

int main(){
  BigInt a, b, res;
  int _a, _b, cmp;


  while(scanf("%d %d", &_a, &_b) == 2){
    printf("a: ");
    big_val(a, _a);
    big_print(a);

    printf("b: ");
    big_val(b, _b);
    big_print(b);
    
    printf("\nSum\n");
    big_sum(res, a, b);
    big_print(res);

    printf("\nSubtraction\n");
    big_sub(res, a, b);
    big_print(res);

    printf("\nMultiplication (unsigned)\n");
    big_umul(res, a, b);
    big_print(res);

    printf("\nMultiplication (signed)\n");
    big_mul(res, a, b);
    big_print(res);

    printf("\nComparison (unsigned)\n");
    cmp = big_ucmp(a, b);
    if(cmp == 1)
      printf("a > b\n");
    else if(cmp == 0)
      printf("a == b\n");
    else
      printf("a < b\n");

    printf("\nComparison (signed)\n");
    cmp = big_cmp(a, b);
    if(cmp == 1)
      printf("a > b\n");
    else if(cmp == 0)
      printf("a == b\n");
    else
      printf("a < b\n");

    printf("\nShift left\n");
    big_shl(res, a, 2);
    big_print(res);

    printf("\nShift right\n");
    big_shr(res, a, 2);
    big_print(res);

    printf("\n");
  }

  return 0;
}