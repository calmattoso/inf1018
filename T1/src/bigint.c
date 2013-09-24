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
  if(res == a) return;
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
  int i, _val;

  for(i = 0, _val = val; i < 4; i++, _val = _val >> 8)
    res[i] = (unsigned char)(_val & 0xFF);

  /* Caso o val seja *negativo*, o padding será 0xFF */
  padding = 0;
  if(val >> 31)
    padding = ~padding;

  /* Preenchimento rápido do resto do BigInt com padding */
  memset(res + 4, padding, NUM_BYTES - 4);
}

/* res = uval (extensão sem sinal) */
void big_uval(BigInt res, unsigned int uval){
  int i;

  for(i = 0; i < 4; i++, uval = uval >> 8)
    res[i] = (unsigned char)(uval & 0xFF);    
  
  /* Preenchimento rápido do resto do BigInt */
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

    res[i] = (unsigned char)(sum & 0xFF);

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
  BigInt _a, _b;
  int n_bits, n_shifts;

  big_copy(_a, a);
  big_copy(_b, b); /* Para dar suporte à (b = a * b) */

  big_uval(res, 0);  

  /*
      Algoritmo padrão de multiplicação: shifta (uma cópia de) a
    para esquerda a cada bit de b, somando a shiftado à resposta
    quando o bit corrente de b é 1. 
      Os shifts na verdade são feitos apenas quando encontra-se
    um bit 1, acelerando o processo.
  */
  n_shifts = 0;
  for(n_bits = 0; n_bits < NUM_BYTES * 8; n_bits++, n_shifts++){
    if( (_b[ n_bits/8 ] & (1 << (n_bits % 8))) ){
      big_shl(_a, _a, n_shifts);
      big_sum(res, res, _a);

      n_shifts = 0;
    }
  }  
}

/* Operacoes de deslocamento */

/* res = a << n */
void big_shl (BigInt res, BigInt a, int n){
  unsigned char carry, cur;
  int i, d_bytes, d_bits;

  /* Para n negativo, o shift é feito no sentido contrário */ 
  if(n < 0){
    big_shr(res, a, -n);
    return;
  }
  
  /* Limitar em NUM_BYTES */
  d_bytes = ((n / 8) > NUM_BYTES ? NUM_BYTES : (n / 8)); 
  d_bits  = n % 8;

  /* 
    Shift rápido dos bytes -- uso memmove porque
    res e a podem ser o mesmo BigInt. memcpy pode não 
    lidar bem com overlap de memória
   */
  if(d_bytes >= 0){
    memmove(res + d_bytes, a, NUM_BYTES - d_bytes);
    memset(res, 0, d_bytes);
  }

  /* Shift, se necessário, de 1..7 bits em cada byte */
  carry = 0;
  for(i = d_bytes; i < NUM_BYTES && d_bits; i++){
    cur = res[i];

    res[i] = (cur << d_bits) | carry;

    carry = (cur >> (8 - d_bits)); 
  }
}

/* res = a >> n (lógico) */
void big_shr (BigInt res, BigInt a, int n){
  unsigned char carry, cur;
  int i, d_bytes, d_bits;

  /* Para n negativo, o shift é feito no sentido contrário */ 
  if(n < 0){
    big_shl(res, a, -n);
    return;
  }

  /* Limitar em NUM_BYTES */
  d_bytes = ((n / 8) > NUM_BYTES ? NUM_BYTES : (n / 8)); 
  d_bits  = n % 8;

  /* Shift rápido dos bytes */
  if(d_bytes >= 0){
    memmove(res, a + d_bytes, NUM_BYTES - d_bytes);
    memset(res + NUM_BYTES - d_bytes, 0, d_bytes);
  }


  /* Shift, se necessário, de 1..7 bits em cada byte */ 
  carry = 0;
  for(i = NUM_BYTES - d_bytes - 1; i >= 0 && d_bits; i--){
    cur = res[i];

    res[i] = (cur >> d_bits) | carry;

    carry = (cur << (8 - d_bits)); 
  }
}

/* Comparação: retorna -1 (a < b), 0 (a == b), 1 (a > b) */

/* comparação com sinal */
int big_cmp(BigInt a, BigInt b){
  int gt0_a, gt0_b;

  gt0_a = big_is_positive(a);
  gt0_b = big_is_positive(b);

  /* a positivo, b negativo */
  if(gt0_a && !gt0_b)
    return 1;

  /* a negativo, b positivo */
  if(!gt0_a && gt0_b)
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