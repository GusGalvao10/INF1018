/* Jo�o Gabriel Gralha Quirino de Souza 2310284 Turma 3WA
   Gustavo de Sousa Galv�o 2410508 Turma 3WA
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
 * @brief Zera o conte�do de um BigInt, atribuindo 0 a todos os bytes.
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
 * @brief Verifica se um BigInt � negativo.
 *
 * O sinal � determinado pelo bit mais significativo (bit 127).
 *
 * @param a BigInt a ser verificado.
 * @return 1 se for negativo, 0 caso contr�rio.
 */
static int big_is_negative(BigInt a) {
    /* Verifica o bit mais significativo (bit 127) */
    return (a[15] & 0x80) != 0;
}

/**
 * @brief Atribui a um BigInt o valor de um long, com extens�o de sinal.
 *
 * Os bytes s�o copiados em ordem little-endian, e o sinal � estendido corretamente.
 *
 * @param res BigInt que armazenar� o resultado.
 * @param val Valor long a ser atribu�do.
 */
void big_val(BigInt res, long val) {
    int i;
    int num_bytes_long = sizeof(long);  /* Quantos bytes tem um long (geralmente 8) */
    int deslocamento_bits;
    unsigned char byte_extraido;

    /* Zera o resultado inicialmente */
    big_zero(res);

    /* Se o valor � negativo, fazemos a extens�o de sinal */
    if (val < 0) {
        /* Preenche com 0xFF para n�meros negativos (extens�o de sinal) */
        for (i = 0; i < NUM_BITS / 8; i++) {
            res[i] = 0xFF;
        }
    }

    /* Copia os bytes do long para o BigInt em little-endian */
    for (i = 0; i < num_bytes_long && i < NUM_BITS/8; i++) {
        /* Calcula quantos bits deslocar para pegar o byte i */
        deslocamento_bits = i * 8;
        
        /* Desloca val à direita para trazer o byte i para a posição menos significativa */
        /* Depois extrai apenas esse byte com o tpcast para unsigned char */
        byte_extraido = (unsigned char)(val >> deslocamento_bits);
        
        /* Armazena o byte extraído na posição i do array */
        res[i] = byte_extraido;
    }
}


/**
 * @brief Calcula o complemento a dois de um BigInt.
 *
 * Inverte todos os bits e adiciona 1.
 *
 * @param res BigInt que armazenar� o resultado.
 * @param a BigInt de entrada.
 */
void big_comp2(BigInt res, BigInt a) {
    int i;
    unsigned int carry = 1;  /* Começamos com carry=1 porque precisamos somar +1 */
    unsigned int byte_invertido;
    unsigned int soma_com_carry;
    unsigned int byte_resultado;
    unsigned int novo_carry;
    
    /* Complemento a 2 = Inverte todos os bits + Soma 1 */
    for (i = 0; i < NUM_BITS/8; i++) {
        /* Passo 1: Inverte os bits do byte atual */
        byte_invertido = (~a[i]) & 0xFF;  // & 0xFF garante apenas 8 bits 
        
        /* Passo 2: Soma o byte invertido com o carry da posição anterior */
        soma_com_carry = byte_invertido + carry;
        
        /* Passo 3: Extrai apenas os 8 bits inferiores para o resultado */
        byte_resultado = soma_com_carry & 0xFF;
        res[i] = (unsigned char)byte_resultado;
        
        /* Passo 4: Verifica se houve overflow (carry para a próxima posição) */
        novo_carry = (soma_com_carry >> 8) & 1;  // Pega o bit 8 (o carry, famoso "vai 1") 
        carry = novo_carry;  // Propaga para a próxima iteração
    }
}

/**
 * @brief Soma dois BigInts (a + b).
 *
 * @param res BigInt que armazenar� o resultado.
 * @param a Primeiro operando.
 * @param b Segundo operando.
 */
void big_sum(BigInt res, BigInt a, BigInt b) {
    int i;
    int carry = 0;

    for (i = 0; i < NUM_BITS / 8; i++) {
        int temp = a[i] + b[i] + carry;     //por isso o carry deve ser inicializado com zero, para usarmos ele a cada nova iteração
        res[i] = (unsigned char)(temp & 0xFF);  // guarda apenas os 8 bits menos significativos
        carry = (temp >> 8) & 1;        // extrai o bit de overflow para a próxima iteração
    }
    /*no fim, o overflow acima de 128 bits é descartado, oque é o correto*/
}

/**
 * @brief Subtrai dois BigInts (a - b).
 *
 * Implementada como a soma de `a` com o complemento a dois de `b`.
 *
 * @param res BigInt que armazenar� o resultado.
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
 * Usa uma varia��o do algoritmo de soma e deslocamento bin�rio.
 * O sinal � tratado separadamente e ajustado ao final.
 *
 * @param res BigInt que armazenar� o resultado.
 * @param a Primeiro operando.
 * @param b Segundo operando.
 */
void big_mul(BigInt res, BigInt a, BigInt b) {
    BigInt temp_res;
    BigInt temp_a;
    BigInt temp_b;
    int i, j;
    int a_negative = 0, b_negative = 0;

    /* Zera o resultado tempor�rio */
    big_zero(temp_res);

    /* Verifica se os operandos s�o negativos */
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

    /* Multiplica��o usando algoritmo de soma repetida otimizado */
    for (i = 0; i < NUM_BITS / 8; i++) {    // Percorre cada byte do multiplicador (b)
        for (j = 0; j < 8; j++) {           // Percorre cada bit dentro do byte atual
            if (temp_b[i] & (1 << j)) {     // Verifica se o bit j do byte i de 'b' é 1
                BigInt shifted_a;
                big_shl(shifted_a, temp_a, i * 8 + j);    //se for desloca "a" para a esquerda (i*8 + j) posiçõe, isso equivale a muktiplicar por 2^(i*8 + j)
                big_sum(temp_res, temp_res, shifted_a);   // Soma esse valor deslocado ao resultado parcial
            }
            
        }
    }

    /* Ajusta o sinal do resultado, se o sinal deles eram diferentes, faz o comp2 da resposta temporária, e caso forem iguais, apenas copia para a resp final*/
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
 * O deslocamento � feito em bits, preservando a ordem little-endian.
 *
 * @param res BigInt que armazenar� o resultado.
 * @param a BigInt a ser deslocado.
 * @param n N�mero de bits a deslocar (0 ? n ? 127).
 */
void big_shl(BigInt res, BigInt a, int n) {
    int byte_shift, bit_shift;
    int i;

    /* Zera o resultado inicialmente */
    big_zero(res);

    /* Se n >= 128 ou negativo, resultado é zero (não há bits para deslocar) */
    if (n >= NUM_BITS || n < 0) {
        return;
    }

    /* Se n == 0, apenas copia */
    if (n == 0) {
        big_copy(res, a);
        return;
    }


    /* Calcula quantos bytes e quantos bits dentro do byte serão deslocados */
    byte_shift = n / 8;
    bit_shift = n % 8;

    /* Desloca primeiro por bytes completos */
    for (i = NUM_BITS / 8 - 1; i >= byte_shift; i--) {
        res[i] = a[i - byte_shift];     // Move o byte da posição (i - byte_shift) para a posição atual, e os bytes menos significativos (menoe que o byte_shift, continuam zerados
    }

    /* Depois desloca pelos bits restantes */
    if (bit_shift > 0) {
        int carry = 0;   // Guardará os bits "que sobram" do deslocamento
        for (i = byte_shift; i < NUM_BITS / 8; i++) {
            int temp = (res[i] << bit_shift) | carry; // Desloca o byte para a esquerda e adiciona os bits carregados do byte anterior
            res[i] = (unsigned char)(temp & 0xFF);    // Mantém apenas os 8 bits menos significativos no byte atual
            carry = (temp >> 8) & 0xFF;     // Extrai os bits que deram overflow pra ser usados no proximo byte
        
        }
    }
}

/**
 * @brief Desloca logicamente um BigInt para a direita (a >> n).
 *
 * Bits deslocados entram como zeros � esquerda.
 *
 * @param res BigInt que armazenar� o resultado.
 * @param a BigInt a ser deslocado.
 * @param n N�mero de bits a deslocar (0 ? n ? 127).
 */
void big_shr(BigInt res, BigInt a, int n) {
    int byte_shift, bit_shift;
    int i;

    /* Zera o resultado inicialmente */
    big_zero(res);

    /* Se n >= 128, o resultado � zero */
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
        int carry = 0;          // Guardará os bits que "sobram" de um byte para o próximo menos significativo
        for (i = NUM_BITS / 8 - 1; i >= 0; i--) {
            int temp = res[i] | (carry << 8);       // Junta o byte atual com os bits que vieram do byte mais significativo anterior
            res[i] = (unsigned char)((temp >> bit_shift) & 0xFF);   // Desloca o byte para a direita e mantém apenas os 8 bits menos significativos
            carry = temp & ((1 << bit_shift) - 1);      // Extrai os bits que “sobraram” que serão passados ao prox byte a direita
        }
    }
}

/**
 * @brief Desloca aritmeticamente um BigInt para a direita (a >> n).
 *
 * Faz a mesma coisa que a funcao anterior mas agora mantendo o sinal original do numero
 *
 * @param res BigInt que armazenar� o resultado.
 * @param a BigInt a ser deslocado.
 * @param n N�mero de bits a deslocar (0 ? n ? 127).
 */
void big_sar(BigInt res, BigInt a, int n) {
    int is_negative;

    /* Verifica se o n�mero � negativo */
    is_negative = big_is_negative(a);

    /* Faz o deslocamento l�gico primeiro */
    big_shr(res, a, n);

    /* Se era negativo, preenche com 1s � esquerda */
    if (is_negative && n > 0 && n < NUM_BITS) {
        int byte_pos = (NUM_BITS - 1 - n) / 8;  // Índice do byte onde o preenchimento parcial começa
        int bit_pos = (NUM_BITS - 1 - n) % 8;   // Posição dentro desse byte onde começa o preenchimento
        int i;

        /* Preenche os bits mais significativos com 1 */
        for (i = byte_pos + 1; i < NUM_BITS / 8; i++) {
            res[i] = 0xFF;  //coloca todos os bits desses bytes com 1
        }

        /* Durante a transicao de sinal, um byte não é invertido completamente, por isso precisamos fazer isso manualmente*/
        if (bit_pos < 7) {
            unsigned char mask = (unsigned char)(0xFF << (bit_pos + 1)); // Cria uma máscara com 1s nas posições mais à esquerda do byte
            res[byte_pos] |= mask;  // Usamos o operador or para ativar esses bits de sinal no byte parcialmente afetado
        }
    }
}