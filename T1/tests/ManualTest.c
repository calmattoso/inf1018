#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "bigint.h"

void big_dump(BigInt a);

const char *help = 
"HELP\n\n"
"A entrada passada no cmd deve ser UM dos valores\n"
"abaixo, ou sua SOMA. No caso da soma, as operacoes\n"
"de cada termo desta serao executadas:\n\n"
"    1 -> big_val || big_uval \n"
"    2 -> big_sum && big_sub  \n"
"    4 -> big_mul && big_umul \n"
"    8 -> big_cmp && big_ucmp \n"
"   16 -> big_shl && big_shr  \n\n"
"A cada iteracao, o programa pede 2 numeros inteiros\n"
"para realizar os testes escolhidos. Escreva exit para\n"
"sair.\n"
"\nEx: Se a entrada for 6 (2 + 4), serao testadas\n"
"a soma/subtracao e a multiplicao dos 2 numeros \n"
"fornecidos pelo usuario.\n\n";

int main(int argc, char *argv[]){
  BigInt a, b, res;
  char input[100];
  int _a, _b, cmp, teste, d_bits;

  if(argc < 2){
    printf("Faltou o tipo de teste! Passe --help para ler as intrucoes!\n");
    exit(1);
  }

  if(!strcmp("--help", argv[1])){
    printf(help);

    return 0;
  }

  teste = atoi(argv[1]);

  fflush(stdin);
  fflush(stdout);

  while(printf("Diga 2 inteiros (ou exit para sair):\n") && scanf(" %[^\n]%*c", input) == 1){
    if(!strcmp(input, "exit")){
      printf("-x- Over -x-\n");

      return 0;
    }

    if(sscanf(input, " %d %d ", &_a, &_b) != 2){
      printf("Sao necessarios 2 inteiros! Tente novamente.\n");
      continue;
    }

    big_val(a, _a);
    big_val(b, _b);

    if(teste & 1){
      printf("\nInitialization\n");
      
      printf("a: ");
      big_dump(a);

      printf("b: ");
      big_dump(b);
    }
    
    if(teste & 2){
      printf("\nSum\n");
      big_sum(res, a, b);
      big_dump(res);

      printf("\nSubtraction\n");
      big_sub(res, a, b);
      big_dump(res);
    }

    if(teste & 4){
      printf("\nMultiplication (unsigned)\n");
      big_umul(res, a, b);
      big_dump(res);

      printf("\nMultiplication (signed)\n");
      big_mul(res, a, b);
      big_dump(res);
    }

    if(teste & 8){
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
    }

    if(teste & 16){
      fflush(stdin);

      printf("\nShift\n");
      printf("Qtos bits? ");
      scanf("%[^\n]%*c", input);      
      while(sscanf(input, " %d ", &d_bits) != 1){
        printf("\nDiga um valor valido de bits: ");

        scanf("%[^\n]%*c", input);
      }

      printf("\nShift left\n");

      printf("a: ");
      big_shl(res, a, d_bits);
      big_dump(res);

      printf("b: ");
      big_shl(res, b, d_bits);
      big_dump(res);

      printf("\nShift right\n");

      printf("a: ");
      big_shr(res, a, d_bits);
      big_dump(res);

      printf("b: ");
      big_shr(res, b, d_bits);
      big_dump(res);
    }

    printf("\n");
  }

  return 0;
}

void big_dump(BigInt a){
  int i;

  for(i = NUM_BYTES - 1; i >= 0; i--)
    printf("%02x ", a[i]);

  printf("\n");
}
