/* João Gabriel Gralha Quirino de Souza 2310284 Turma 3WA
   Gustavo de Sousa Galvão 2410508 Turma 3WA
*/

#include "bigint.h"
#include <string.h>

/**
 * @brief Copia o valor de um BigInt para outro.
 *
 * @param dest BigInt de destino.
 * @param src BigInt de origem.
 */
static void big_copy(BigInt dest, BigInt src) {
    int i;
    for (i = 0; i < NUM_BITS / 8; i++) {
        dest[i] = src[i];
    }
}

/**
 * @brief Zera o conteúdo de um BigInt, atribuindo 0 a todos os bytes.
 *
 * @param res BigInt a ser zerado.
 */
static void big_zero(BigInt res) {
    int i;
    for (i = 0; i < NUM_BITS / 8; i++) {
        res[i] = 0;
    }
}

/**
 * @brief Verifica se um BigInt é negativo.
 *
 * O sinal é determinado pelo bit mais significativo (bit 127).
 *
 * @param a BigInt a ser verificado.
 * @return 1 se for negativo, 0 caso contrário.
 */
static int big_is_negative(BigInt a) {
    /* Verifica o bit mais significativo (bit 127) */
    return (a[15] & 0x80) != 0;
}

/**
 * @brief Atribui a um BigInt o valor de um long, com extensão de sinal.
 *
 * Os bytes são copiados em ordem little-endian, e o sinal é estendido corretamente.
 *
 * @param res BigInt que armazenará o resultado.
 * @param val Valor long a ser atribuído.
 */
void big_val(BigInt res, long val) {
    int i;

    /* Zera o resultado inicialmente */
    big_zero(res);

    /* Se o valor é negativo, fazemos a extensão de sinal */
    if (val < 0) {
        /* Preenche com 0xFF para números negativos (extensão de sinal) */
        for (i = 0; i < NUM_BITS / 8; i++) {
            res[i] = 0xFF;
        }
    }

    /* Copia os bytes do long (little-endian) */
    for (i = 0; i < sizeof(long) && i < NUM_BITS / 8; i++) {
        res[i] = (unsigned char)(val >> (i * 8));
    }
}

/**
 * @brief Calcula o complemento a dois de um BigInt.
 *
 * Inverte todos os bits e adiciona 1.
 *
 * @param res BigInt que armazenará o resultado.
 * @param a BigInt de entrada.
 */
void big_comp2(BigInt res, BigInt a) {
    int i;
    int carry = 1;

    /* Inverte todos os bits e soma 1 */
    for (i = 0; i < NUM_BITS / 8; i++) {
        int temp = (~a[i]) + carry;
        res[i] = (unsigned char)(temp & 0xFF);
        carry = (temp >> 8) & 1;
    }
}

/**
 * @brief Soma dois BigInts (a + b).
 *
 * @param res BigInt que armazenará o resultado.
 * @param a Primeiro operando.
 * @param b Segundo operando.
 */
void big_sum(BigInt res, BigInt a, BigInt b) {
    int i;
    int carry = 0;

    for (i = 0; i < NUM_BITS / 8; i++) {
        int temp = a[i] + b[i] + carry;
        res[i] = (unsigned char)(temp & 0xFF);
        carry = (temp >> 8) & 1;
    }
}

/**
 * @brief Subtrai dois BigInts (a - b).
 *
 * Implementada como a soma de `a` com o complemento a dois de `b`.
 *
 * @param res BigInt que armazenará o resultado.
 * @param a Minuendo.
 * @param b Subtraendo.
 */
void big_sub(BigInt res, BigInt a, BigInt b) {
    BigInt temp_b;

    /* Calcula o complemento a 2 de b */
    big_comp2(temp_b, b);

    /* Soma a com -b */
    big_sum(res, a, temp_b);
}

/**
 * @brief Multiplica dois BigInts (a * b).
 *
 * Usa uma variação do algoritmo de soma e deslocamento binário.
 * O sinal é tratado separadamente e ajustado ao final.
 *
 * @param res BigInt que armazenará o resultado.
 * @param a Primeiro operando.
 * @param b Segundo operando.
 */
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
    }
    else {
        big_copy(temp_a, a);
    }

    if (b_negative) {
        big_comp2(temp_b, b);
    }
    else {
        big_copy(temp_b, b);
    }

    /* Multiplicação usando algoritmo de soma repetida otimizado */
    for (i = 0; i < NUM_BITS / 8; i++) {
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
    }
    else {
        big_copy(res, temp_res);
    }
}

/**
 * @brief Desloca um BigInt para a esquerda (a << n).
 *
 * O deslocamento é feito em bits, preservando a ordem little-endian.
 *
 * @param res BigInt que armazenará o resultado.
 * @param a BigInt a ser deslocado.
 * @param n Número de bits a deslocar (0 ? n ? 127).
 */
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
    for (i = NUM_BITS / 8 - 1; i >= byte_shift; i--) {
        res[i] = a[i - byte_shift];
    }

    /* Depois desloca pelos bits restantes */
    if (bit_shift > 0) {
        int carry = 0;
        for (i = byte_shift; i < NUM_BITS / 8; i++) {
            int temp = (res[i] << bit_shift) | carry;
            res[i] = (unsigned char)(temp & 0xFF);
            carry = (temp >> 8) & 0xFF;
        }
    }
}

/**
 * @brief Desloca logicamente um BigInt para a direita (a >> n).
 *
 * Bits deslocados entram como zeros à esquerda.
 *
 * @param res BigInt que armazenará o resultado.
 * @param a BigInt a ser deslocado.
 * @param n Número de bits a deslocar (0 ? n ? 127).
 */
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
    for (i = 0; i < NUM_BITS / 8 - byte_shift; i++) {
        res[i] = a[i + byte_shift];
    }

    /* Depois desloca pelos bits restantes */
    if (bit_shift > 0) {
        int carry = 0;
        for (i = NUM_BITS / 8 - 1; i >= 0; i--) {
            int temp = res[i] | (carry << 8);
            res[i] = (unsigned char)((temp >> bit_shift) & 0xFF);
            carry = temp & ((1 << bit_shift) - 1);
        }
    }
}

/**
 * @brief Desloca aritmeticamente um BigInt para a direita (a >> n).
 *
 * Preserva o bit de sinal em caso de número negativo.
 *
 * @param res BigInt que armazenará o resultado.
 * @param a BigInt a ser deslocado.
 * @param n Número de bits a deslocar (0 ? n ? 127).
 */
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
        for (i = byte_pos + 1; i < NUM_BITS / 8; i++) {
            res[i] = 0xFF;
        }

        /* Preenche parcialmente o byte na posição byte_pos */
        if (bit_pos < 7) {
            unsigned char mask = (unsigned char)(0xFF << (bit_pos + 1));
            res[byte_pos] |= mask;
        }
    }
}