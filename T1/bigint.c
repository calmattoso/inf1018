#include <stdio.h>
#include <string.h>
#include "bigint.h"

/* Auxiliares */

/* a > 0 ? */
static int big_is_positive(BigInt a){
  return ((a[NUM_BYTES - 1] >> 7) ^ 1);
}

/* res = a */
static void big_copy(BigInt res, BigInt a){
  memcpy(res, a, NUM_BYTES);
}

/* res = -a */
static void big_minus(BigInt res, BigInt a){
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
  unsigned char padding = 0;
  int i;

  for(i = 0; i < 4; i++)
    res[i] = ((val >> (8 * i)) & 0xFF);

  // Caso o val seja *negativo*, o padding será 0xFF
  padding = 0;
  if(val >> 31)
    padding = ~padding;

  memset(res + 4, padding, NUM_BYTES - 4);
}

/* res = uval (extensão sem sinal) */
void big_uval(BigInt res, unsigned int uval){
  int i;

  for(i = 0; i < 4; i++)
    res[i] = ((uval >> (8 * i)) & 0xFF);    
  
  memset(res + 4, 0, NUM_BYTES - 4);
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

  big_minus(neg_b, b);

  big_sum(res, a, neg_b);
}

/* res = a * b (com sinal) */
void big_mul (BigInt res, BigInt a, BigInt b){
  big_umul(res, a, b);
}

/* res = a * b (sem sinal) */
void big_umul (BigInt res, BigInt a, BigInt b){
  BigInt _a;
  int n;

  big_copy(_a, a);

  big_uval(res, 0);  
  for(n = 0; n < NUM_BYTES * 8; n++){
    if( (b[n/8] & (1 << (n % 8))) )
      big_sum(res, res, _a);

    big_shl(_a, _a, 1);
  }  
}

/* Operacoes de deslocamento */

/* res = a << n */
void big_shl (BigInt res, BigInt a, int n){
  unsigned char carry, cur;
  int i;

  carry = 0;
  for(i = 0; i < NUM_BYTES; i++){
    cur = a[i];

    res[i] = (cur << 1) | carry;

    carry = ((cur >> 7) & 1); 
  }
}

/* res = a >> n (lógico) */
void big_shr (BigInt res, BigInt a, int n){
  unsigned char carry, cur;
  int i;

  carry = 0;
  for(i = NUM_BYTES - 1; i >= 0; i--){
    cur = a[i];

    res[i] = (cur >> 1) | carry;

    carry = ((cur & 1) << 7); 
  }
}

/* Comparação: retorna -1 (a < b), 0 (a == b), 1 (a > b) */

/* comparação com sinal */
int big_cmp(BigInt a, BigInt b){
  int gt0_a, gt0_b;

  gt0_a = big_is_positive(a);
  gt0_b = big_is_positive(b);

  /* a positivo, b negativo */
  if(gt0_a > 0 && gt0_b == 0)
    return 1;

  /* a negativo, b positivo */
  if(gt0_a == 0 && gt0_b > 0)
    return -1;

  /* ambos positivos ou ambos negativos */
  return big_ucmp(a, b);
}

/* comparação sem sinal */
int big_ucmp(BigInt a, BigInt b){
  int i;

  for(i = NUM_BYTES - 1; i >= 0; i--){
    if(a[i] > b[i])
      return 1;
    else if(a[i] < b[i])
      return -1;
  }

  return 0;
}

/* Exibição */
void big_print(BigInt a){
  int i;

  for(i = NUM_BYTES - 1; i >= 0; i--)
    printf("%02x ", a[i]);
  printf("\n");
}