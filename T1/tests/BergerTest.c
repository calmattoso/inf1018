/* 
	Author: Guilherme Berger
    Find @: https://github.com/gberger42
*/

#include <stdio.h>
#include <math.h>
#include "..\src\bigint.h"

#define true 1
#define false 0

void big_dump(BigInt b, int newline){
	int i = NUM_BYTES - 1;
	printf("0x");
	for(; i>=0; i--){
		printf("%02x", b[i]);
	}
	newline ? printf("\n") : 0;
}

void test_uval(unsigned int ui){
	BigInt res;

	printf("%-12u -> ", ui);
	big_uval(res, ui);
	big_dump(res, true);
}

void test_val(int si){
	BigInt res;

	printf("%-12d -> ", si);
	big_val(res, si);
	big_dump(res, true);
}

void test_sum_sub(int a, int b){
	BigInt res, ba, bb, bb_neg;
	long l = a+b;

	printf("0x%08x -> ", l); 
	big_val(ba, a);
	big_val(bb, b);
	big_val(bb_neg, -b);

	big_sum(res, ba, bb);
	big_dump(res, false);

	printf(" == ");

	big_sub(res, ba, bb_neg);
	big_dump(res, true);
}

void test_umul(unsigned long long a, unsigned long long b){
	BigInt res, ba, bb;
	unsigned long long l = a*b;

	printf("0x%08x ->", l);
	big_uval(ba, a);
	big_uval(bb, b);

	big_umul(res, ba, bb);
	big_dump(res, true);
}

void test_mul(int a, int b){
	BigInt res, ba, bb;
	long long l = a*b;

	printf("0x%08x ->", l);
	big_val(ba, a);
	big_val(bb, b);

	big_mul(res, ba, bb);
	big_dump(res, true);
}

void test_shl(int a, int n){
	BigInt res, ba;

	printf("0x%09x -> ", a<<n);

	big_val(ba, a);
	big_shl(res, ba, n);
	big_dump(res, true);
}

void test_shr(int a, int n){
	BigInt res, ba;

	printf("0x%08x -> ", a>>n);

	big_val(ba, a);
	big_shr(res, ba, n);
	big_dump(res, true);
}

int ucmp(unsigned int a, unsigned int b){
	return a < b ? -1 : (a == b ? 0 : 1);
}

void test_ucmp(unsigned int a, unsigned int b){
	BigInt ba, bb;
	int res;

	big_uval(ba, a);
	big_uval(bb, b);
	printf("%d -> %d\n", ucmp(a, b), big_ucmp(ba, bb));
}

int cmp(int a, int b){
	return a < b ? -1 : (a == b ? 0 : 1);
}

void test_cmp(int a, int b){
	BigInt ba, bb;
	int res;

	big_val(ba, a);
	big_val(bb, b);
	printf("%d -> %d\n", cmp(a, b), big_cmp(ba, bb));
}

void big_max(BigInt b){
	int i;
	for(i=0; i<NUM_BYTES; i++){
		b[i] = 0xff;
	}
}

int main(void){ 	
	BigInt b;
	BigInt one;

	printf("\n== big_val ==\n");
	test_val(2147483647);
	test_val(65536);
	test_val(0);
	test_val(-6283);
	test_val(-65537);
	test_val(-2147483648);

	printf("\n== big_uval ==\n");
	test_uval(4294967295);
	test_uval(16777215);
	test_uval(65535);
	test_uval(255);
	test_uval(0);

	printf("\n== big_sum e big_sub ==\n");
	test_sum_sub(-2000, -5000);
	test_sum_sub(-2000, 3000);
	test_sum_sub(1000, 0);
	test_sum_sub(0, 1000);
	test_sum_sub(5, -5);
	test_sum_sub(20000, -19999);
	test_sum_sub(65536, 65536);
	test_sum_sub(2000000000, 2000000000);
	test_sum_sub(-1000000000, -2000000000);
	test_sum_sub(100000, -200000);

	printf("\n== big_umul ==\n");
	test_umul(1000, 0);
	test_umul(50, 50);
	test_umul(62, 68);
	test_umul(256, 256);
	test_umul(50000, 50000);

	printf("\n== big_mul ==\n");
	test_mul(1000, 0);
	test_mul(256, 256);
	test_mul(50000, 50000);
	test_mul(256, -256);
	test_mul(-256, 256);
	test_mul(50000, -50000);
	test_mul(-50000, 50000);

	printf("\n== big_shl ==\n");
	test_shl(0xffff, 1);
	test_shl(0xffff, 2);
	test_shl(0xffff, 3);
	test_shl(0xffff, 4);
	test_shl(0xffff, 5);
	test_shl(0xffff, 6);
	test_shl(0xffff, 7);
	test_shl(0xffff, 8);
	test_shl(0xabcdef, 9);
	test_shl(0xabcdef, 10);
	test_shl(0xabcdef, 11);
	test_shl(0xabcdef, 12);
	test_shl(0xabcdef, 13);
	test_shl(0xabcdef, 14);
	test_shl(0xabcdef, 15);
	test_shl(0xabcdef, 16);
	test_shl(0xabcdef, 104);

	printf("\n== big_shr ==\n");
	test_shr(0xffff, 1);
	test_shr(0xffff, 2);
	test_shr(0xffff, 3);
	test_shr(0xffff, 4);
	test_shr(0xffff, 5);
	test_shr(0xffff, 6);
	test_shr(0xffff, 7);
	test_shr(0xffff, 8);
	test_shr(0xabcdef, 9);
	test_shr(0xabcdef, 10);
	test_shr(0xabcdef, 11);
	test_shr(0xabcdef, 12);
	test_shr(0xabcdef, 13);
	test_shr(0xabcdef, 14);
	test_shr(0xabcdef, 15);
	test_shr(0xabcdef, 16);

	printf("\n== big_ucmp ==\n");
	test_ucmp(1000, 0);
	test_ucmp(1000000, 0);
	test_ucmp(1000000000, 0);
	test_ucmp(0, 1000);
	test_ucmp(0, 1000000);
	test_ucmp(0, 1000000000);
	test_ucmp(1000, 1000);
	test_ucmp(1000000, 1000000);
	test_ucmp(1000000000, 1000000000);

	printf("\n== big_cmp ==\n");
	test_cmp(1000000, 0);
	test_cmp(1000000, -1000000);
	test_cmp(0, -1000000);
	test_cmp(0, 1000000);
	test_cmp(-1000000, 1000000);
	test_cmp(-1000000, 0);
	test_cmp(0, 0);
	test_cmp(1000000, 1000000);
	test_cmp(-1000000, -1000000);

	printf("\n== overflow ==\n");
	big_max(b);
	big_uval(one, 1);
	big_dump(b, false);
	printf(" + ");
	big_dump(one, false);
	big_sum(b, b, one);
	printf(" = ");
	big_dump(b, true);

	big_dump(b, false);
	printf(" - ");
	big_dump(one, false);
	big_sub(b, b, one);
	printf(" = ");	
	big_dump(b, true);

	return 1;
}