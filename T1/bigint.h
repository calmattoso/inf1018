#ifndef BIGINT_H
#define BIGINT_H

#define NUM_BYTES 16
typedef unsigned char BigInt[NUM_BYTES];

/* Atribuição */

/* res = val (extensão com sinal) */
void big_val(BigInt res, int val);

/* res = uval (extensão sem sinal) */
void big_uval(BigInt res, unsigned int uval);

/* Operacoes aritméticas */

/* res = a + b */
void big_sum (BigInt res, BigInt a, BigInt b);

/* res = a - b */
void big_sub (BigInt res, BigInt a, BigInt b);

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
void big_print(BigInt a);

#endif