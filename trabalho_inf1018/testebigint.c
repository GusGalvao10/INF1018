/* Jo�o Gabriel Gralha Quirino de Souza 2310284 Turma 3WA
   Gustavo de Sousa Galv�o 2410508 Turma 3WA
*/

#include <stdio.h>
#include <string.h>
#include "bigint.h"

/**
 * @brief Imprime um BigInt em formato hexadecimal (big-endian).
 *
 * Exibe o valor de 128 bits em sequ�ncia de bytes, do mais significativo para o menos significativo.
 *
 * @param label R�tulo descritivo que ser� impresso antes do valor.
 * @param a BigInt a ser impresso.
 */
void print_bigint(const char* label, BigInt a) {
    int i;
    printf("%s: ", label);
    for (i = NUM_BITS / 8 - 1; i >= 0; i--) {
        printf("%02X", a[i]);
    }
    printf("\n");
}

/**
 * @brief Imprime os bytes de um BigInt em ordem little-endian.
 *
 * �til para depura��o detalhada, mostrando os valores byte a byte como armazenados na mem�ria.
 *
 * @param label R�tulo descritivo que ser� impresso antes dos bytes.
 * @param a BigInt a ser impresso.
 */
void print_bigint_bytes(const char* label, BigInt a) {
    int i;
    printf("%s (bytes): ", label);
    for (i = 0; i < NUM_BITS / 8; i++) {
        printf("0x%02X", a[i]);
        if (i < NUM_BITS / 8 - 1) printf(", ");
    }
    printf("\n");
}

/**
 * @brief Compara dois BigInts para verificar se s�o iguais.
 *
 * @param a Primeiro BigInt.
 * @param b Segundo BigInt.
 * @return 1 se forem iguais, 0 caso contr�rio.
 */
int big_equals(BigInt a, BigInt b) {
    int i;
    for (i = 0; i < NUM_BITS / 8; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

/**
 * @brief Cria um BigInt a partir de um array de 16 bytes.
 *
 * @param res BigInt de destino.
 * @param bytes Array de 16 bytes que define o valor desejado.
 */
void create_test_bigint(BigInt res, unsigned char bytes[16]) {
    int i;
    for (i = 0; i < 16; i++) {
        res[i] = bytes[i];
    }
}

/**
 * @brief Fun��o principal de testes da biblioteca BigInt.
 *
 * Executa uma s�rie de testes unit�rios para verificar o correto funcionamento
 * das opera��es implementadas em bigint.c, incluindo:
 * - Atribui��o e extens�o de sinal (`big_val`)
 * - Complemento a 2 (`big_comp2`)
 * - Soma, subtra��o e multiplica��o (`big_sum`, `big_sub`, `big_mul`)
 * - Deslocamentos l�gicos e aritm�ticos (`big_shl`, `big_shr`, `big_sar`)
 *
 * Cada teste imprime os resultados intermedi�rios em formato hexadecimal.
 *
 * @return 0 se todos os testes forem executados corretamente.
 */
int main() {
    BigInt a, b, result;



    /* Teste 1: big_val com valores positivos e negativos */
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

    /* Teste 2: big_comp2 (negacao) */
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

    /* Teste 3: big_sum (soma) */
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

    /* Teste 4: big_sub (subtracao) */
    printf("4. Testando big_sub:\n");

    big_val(a, 100);
    big_val(b, 30);
    big_sub(result, a, b);
    print_bigint("100", a);
    print_bigint("30", b);
    print_bigint("100 - 30", result);

    printf("\n");

    /* Teste 5: big_mul (multiplicacaoo) */
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

    /* Teste 6: big_shl (shift left) */
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

    /* Teste 7: big_shr (shift right logico) */
    printf("7. Testando big_shr:\n");

    big_val(a, 8);
    big_shr(result, a, 1);
    print_bigint("8", a);
    print_bigint("8 >> 1 (logico)", result);

    big_val(a, -8);
    big_shr(result, a, 1);
    print_bigint("-8", a);
    print_bigint("-8 >> 1 (logico)", result);

    printf("\n");

    /* Teste 8: big_sar (shift right aritmetico) */
    printf("8. Testando big_sar:\n");

    big_val(a, 8);
    big_sar(result, a, 1);
    print_bigint("8", a);
    print_bigint("8 >> 1 (aritmetico)", result);

    big_val(a, -8);
    big_sar(result, a, 1);
    print_bigint("-8", a);
    print_bigint("-8 >> 1 (aritmetico)", result);

    printf("\n");


    return 0;
}