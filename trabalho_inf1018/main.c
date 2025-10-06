/* 
Gustavo de Sousa Galvão 2410508 3WA
João Gabriel Gralha Quirino de Souza  2310284 3WA
*/

#include <stdio.h>
#include <string.h>
#include "bigint.h"

/* Função auxiliar para imprimir um BigInt em hexadecimal */
void print_bigint(const char* label, BigInt a) {
    int i;
    printf("%s: ", label);
    for (i = NUM_BITS/8 - 1; i >= 0; i--) {
        printf("%02X", a[i]);
    }
    printf("\n");
}

/* Função auxiliar para imprimir um BigInt como bytes usando a ordenação little-eldian */
void print_bigint_bytes(const char* label, BigInt a) {
    int i;
    printf("%s (bytes): ", label);
    for (i = 0; i < NUM_BITS/8; i++) {
        printf("0x%02X", a[i]);
        if (i < NUM_BITS/8 - 1) printf(", ");
    }
    printf("\n");
}

/* Função auxiliar para verificar se dois BigInts são iguais */
int big_equals(BigInt a, BigInt b) {
    int i;
    for (i = 0; i < NUM_BITS/8; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}


int main() {
    BigInt a, b, result;
    
    
    
    printf("1. Testando big_val:\n");
    
    big_val(a, 1);
    print_bigint_bytes("big_val(1)", a);
    
    big_val(a, -2);
    print_bigint_bytes("big_val(-2)", a);
    
    big_val(a, 0);
    print_bigint_bytes("big_val(0)", a);
    
    big_val(a, 12345);
    print_bigint_bytes("big_val(12345)", a);
    
    printf("\n");
    
    
    printf("2. Testando big_comp2:\n");
    
    big_val(a, 5);
    big_comp2(result, a);
    print_bigint("5", a);
    print_bigint("-(5)", result);
    
    big_val(a, -10);
    big_comp2(result, a);
    print_bigint("-10", a);
    print_bigint("-(-10)", result);
    
    printf("\n");
    
  
    printf("3. Testando big_sum:\n");
    
    big_val(a, 100);
    big_val(b, 50);
    big_sum(result, a, b);
    print_bigint("100", a);
    print_bigint("50", b);
    print_bigint("100 + 50", result);
    
    big_val(a, -30);
    big_val(b, 20);
    big_sum(result, a, b);
    print_bigint("-30", a);
    print_bigint("20", b);
    print_bigint("-30 + 20", result);
    
    printf("\n");
  
    printf("4. Testando big_sub:\n");
    
    big_val(a, 100);
    big_val(b, 30);
    big_sub(result, a, b);
    print_bigint("100", a);
    print_bigint("30", b);
    print_bigint("100 - 30", result);
    
    printf("\n");
    

    printf("5. Testando big_mul:\n");
    
    big_val(a, 10);
    big_val(b, 20);
    big_mul(result, a, b);
    print_bigint("10", a);
    print_bigint("20", b);
    print_bigint("10 * 20", result);
    
    big_val(a, -5);
    big_val(b, 6);
    big_mul(result, a, b);
    print_bigint("-5", a);
    print_bigint("6", b);
    print_bigint("-5 * 6", result);
    
    printf("\n");
    
 
    printf("6. Testando big_shl:\n");
    
    big_val(a, 1);
    big_shl(result, a, 1);
    print_bigint("1", a);
    print_bigint("1 << 1", result);
    
    big_val(a, 3);
    big_shl(result, a, 2);
    print_bigint("3", a);
    print_bigint("3 << 2", result);
    
    printf("\n");
    
   
    printf("7. Testando big_shr:\n");
    
    big_val(a, 8);
    big_shr(result, a, 1);
    print_bigint("8", a);
    print_bigint("8 >> 1 (lógico)", result);
    
    big_val(a, -8);
    big_shr(result, a, 1);
    print_bigint("-8", a);
    print_bigint("-8 >> 1 (lógico)", result);
    
    printf("\n");
    

    printf("8. Testando big_sar:\n");
    
    big_val(a, 8);
    big_sar(result, a, 1);
    print_bigint("8", a);
    print_bigint("8 >> 1 (aritmético)", result);
    
    big_val(a, -8);
    big_sar(result, a, 1);
    print_bigint("-8", a);
    print_bigint("-8 >> 1 (aritmético)", result);
    
    printf("\n");
    
    
    return 0;
}