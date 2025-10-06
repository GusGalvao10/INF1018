/* Nome_do_Aluno Matricula Turma 
Gustavo de Sousa Galvão 2410508 3WA
João Gabriel Gralha Quirino de Souza  2310284 3WA
*/

#include "bigint.h"
#include <string.h>

/* Função auxiliar para copiar um BigInt */
static void big_copy(BigInt dest, BigInt src) {
    int i;
    for (i = 0; i < NUM_BITS/8; i++) {
        dest[i] = src[i];
    }
}

/* Função auxiliar para zerar um BigInt */
static void big_zero(BigInt res) {
    int i;
    for (i = 0; i < NUM_BITS/8; i++) {
        res[i] = 0;
    }
}

/* Função auxiliar para verificar se um BigInt é negativo */
static int big_is_negative(BigInt a) {
    /* Verifica o bit mais significativo (bit 127) */
    return (a[15] & 0x80) != 0;
}

/* Atribuição (com extensão) */
void big_val(BigInt res, long val) {
    int i;
    
    /* Zera o resultado inicialmente */
    big_zero(res);
    
    /* Se o valor é negativo, fazemos a extensão de sinal */
    if (val < 0) {
        /* Preenche com 0xFF para números negativos (extensão de sinal) */
        for (i = 0; i < NUM_BITS/8; i++) {
            res[i] = 0xFF;
        }
    }
    
    /* Copia os bytes do long (little-endian) */
    for (i = 0; i < sizeof(long) && i < NUM_BITS/8; i++) {
        res[i] = (unsigned char)(val >> (i * 8));
    }
}

/* res = -a (complemento a 2) */
void big_comp2(BigInt res, BigInt a) {
    int i;
    unsigned int carry = 1;
    
    /* Inverte todos os bits e soma 1 */
    for (i = 0; i < NUM_BITS/8; i++) {
        unsigned int inverted = (~a[i]) & 0xFF;  /* Garante apenas 8 bits invertidos */
        unsigned int temp = inverted + carry;
        res[i] = (unsigned char)(temp & 0xFF);
        carry = (temp >> 8) & 1;
    }
}

/* res = a + b */
void big_sum(BigInt res, BigInt a, BigInt b) {
    int i;
    int carry = 0;
    
    for (i = 0; i < NUM_BITS/8; i++) {
        int temp = a[i] + b[i] + carry;
        res[i] = (unsigned char)(temp & 0xFF);
        carry = (temp >> 8) & 1;
    }
}

/* res = a - b */
void big_sub(BigInt res, BigInt a, BigInt b) {
    BigInt temp_b;
    
    /* Calcula o complemento a 2 de b */
    big_comp2(temp_b, b);
    
    /* Soma a com -b */
    big_sum(res, a, temp_b);
}

/* res = a * b */
void big_mul(BigInt res, BigInt a, BigInt b) {
    BigInt temp_res;
    BigInt temp_a;
    BigInt temp_b;
    int i, j;
    int a_negative = 0, b_negative = 0;
    
    /* Zera o resultado temporário */
    big_zero(temp_res);
    
    /* Verifica se os operandos são negativos */
    a_negative = big_is_negative(a);
    b_negative = big_is_negative(b);
    
    /* Trabalha com valores absolutos */
    if (a_negative) {
        big_comp2(temp_a, a);
    } else {
        big_copy(temp_a, a);
    }
    
    if (b_negative) {
        big_comp2(temp_b, b);
    } else {
        big_copy(temp_b, b);
    }
    
    /* Multiplicação usando algoritmo de soma repetida otimizado */
    for (i = 0; i < NUM_BITS/8; i++) {
        for (j = 0; j < 8; j++) {
            if (temp_b[i] & (1 << j)) {
                BigInt shifted_a;
                big_shl(shifted_a, temp_a, i * 8 + j);
                big_sum(temp_res, temp_res, shifted_a);
            }
        }
    }
    
    /* Ajusta o sinal do resultado */
    if (a_negative != b_negative) {
        big_comp2(res, temp_res);
    } else {
        big_copy(res, temp_res);
    }
}

/* res = a << n */
void big_shl(BigInt res, BigInt a, int n) {
    int byte_shift, bit_shift;
    int i;
    
    /* Zera o resultado inicialmente */
    big_zero(res);
    
    /* Se n >= 128, o resultado é zero */
    if (n >= NUM_BITS || n < 0) {
        return;
    }
    
    /* Se n == 0, apenas copia */
    if (n == 0) {
        big_copy(res, a);
        return;
    }
    
    byte_shift = n / 8;
    bit_shift = n % 8;
    
    /* Desloca primeiro por bytes completos */
    for (i = NUM_BITS/8 - 1; i >= byte_shift; i--) {
        res[i] = a[i - byte_shift];
    }
    
    /* Depois desloca pelos bits restantes */
    if (bit_shift > 0) {
        int carry = 0;
        for (i = byte_shift; i < NUM_BITS/8; i++) {
            int temp = (res[i] << bit_shift) | carry;
            res[i] = (unsigned char)(temp & 0xFF);
            carry = (temp >> 8) & 0xFF;
        }
    }
}

/* res = a >> n (lógico) */
void big_shr(BigInt res, BigInt a, int n) {
    int byte_shift, bit_shift;
    int i;
    
    /* Zera o resultado inicialmente */
    big_zero(res);
    
    /* Se n >= 128, o resultado é zero */
    if (n >= NUM_BITS || n < 0) {
        return;
    }
    
    /* Se n == 0, apenas copia */
    if (n == 0) {
        big_copy(res, a);
        return;
    }
    
    byte_shift = n / 8;
    bit_shift = n % 8;
    
    /* Desloca primeiro por bytes completos */
    for (i = 0; i < NUM_BITS/8 - byte_shift; i++) {
        res[i] = a[i + byte_shift];
    }
    
    /* Depois desloca pelos bits restantes */
    if (bit_shift > 0) {
        int carry = 0;
        for (i = NUM_BITS/8 - 1; i >= 0; i--) {
            int temp = res[i] | (carry << 8);
            res[i] = (unsigned char)((temp >> bit_shift) & 0xFF);
            carry = temp & ((1 << bit_shift) - 1);
        }
    }
}

/* res = a >> n (aritmético) */
void big_sar(BigInt res, BigInt a, int n) {
    int is_negative;
    
    /* Verifica se o número é negativo */
    is_negative = big_is_negative(a);
    
    /* Faz o deslocamento lógico primeiro */
    big_shr(res, a, n);
    
    /* Se era negativo, preenche com 1s à esquerda */
    if (is_negative && n > 0 && n < NUM_BITS) {
        int byte_pos = (NUM_BITS - 1 - n) / 8;
        int bit_pos = (NUM_BITS - 1 - n) % 8;
        int i;
        
        /* Preenche os bits mais significativos com 1 */
        for (i = byte_pos + 1; i < NUM_BITS/8; i++) {
            res[i] = 0xFF;
        }
        
        /* Preenche parcialmente o byte na posição byte_pos */
        if (bit_pos < 7) {
            unsigned char mask = (unsigned char)(0xFF << (bit_pos + 1));
            res[byte_pos] |= mask;
        }
    }
}