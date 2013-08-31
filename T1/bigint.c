#include <stdio.h>
#include "bigint.h"

/* Auxiliares */

static void negative(BigInt res, BigInt a){
  BigInt one;
  int i;

  for(i = 0; i < NUM_BYTES; i++)
    res[i] = ~a[i];

  big_uval(one, 1);

  big_sum(res, res, one);
}

/* Atribuição */

/* res = val (extensão com sinal) */
void big_val(BigInt res, int val){
  int i;
  unsigned char padding = 0;

  for(i = 0; i < 4; i++)
    res[i] = ((val >> (8 * i)) & 0xFF);

  // Caso o val seja *negativo*, o padding será 0xFF
  if(val >> 31)
    padding = ~padding;

  for(i = 4; i < NUM_BYTES; i++)
    res[i] = padding;
}

/* res = uval (extensão sem sinal) */
void big_uval(BigInt res, unsigned int uval){
  int i;

  for(i = 0; i < 4; i++)
    res[i] = ((uval >> (8 * i)) & 0xFF);
    
  for(i = 4; i < NUM_BYTES; i++)
    res[i] = 0;
}

/* Operacoes aritméticas */

/* res = a + b */
void big_sum (BigInt res, BigInt a, BigInt b){
  int i;
  unsigned short sum, carry;

  carry = 0;
  for(i = 0; i < NUM_BYTES; i++){
    sum = (unsigned short)a[i] + (unsigned short)b[i] + carry;

    res[i] = (sum & 0xFF);

    carry = ((sum >> 8) & 0xFF);
  }
}

/* res = a - b */
void big_sub (BigInt res, BigInt a, BigInt b){
  BigInt neg_b;

  negative(neg_b, b);

  big_sum(res, a, neg_b);
}

/* res = a * b (com sinal) */
void big_mul (BigInt res, BigInt a, BigInt b);

/* res = a * b (sem sinal) */
void big_umul (BigInt res, BigInt a, BigInt b);

/* Operacoes de deslocamento */

/* res = a << n */
void big_shl (BigInt res, BigInt a, int n);

/* res = a >> n (lógico) */
void big_shr (BigInt res, BigInt a, int n);

/* Comparação: retorna -1 (a < b), 0 (a == b), 1 (a > b) */

/* comparação com sinal */
int big_cmp(BigInt a, BigInt b);

/* comparação sem sinal */
int big_ucmp(BigInt a, BigInt b);

/* Exibição */
void big_print(BigInt a){
  int i;

  for(i = NUM_BYTES - 1; i >= 0; i--)
    printf("%02x ", a[i]);
  printf("\n");
}