/* main.c - Arquivo de teste para o gera_codigo */
#include <stdio.h>
#include <stdlib.h>
#include "gera_codigo.h" 

int main(int argc, char *argv[]) {
    FILE *fp;
    funcp funcLBS = NULL;
    unsigned char code[1000]; // Um buffer seguro
    int res;
    int argumento_p0 = 0; //valor padrao 0 caso não tenha digitado nada

    /* Verifica se passou o nome do arquivo */
    if (argc < 2) {
        printf("Erro: Voce esqueceu de passar o arquivo!\n");
        printf("Uso: ./gera teste.lbs\n");
        return 1;
    }
    /* se usuario digitou um terceiro item, converte para número */
    if (argc >= 3){
        argumento_p0 = atoi(argv[2]);
    }

    /* Abre o arquivo para leitura */
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    /* Gera o codigo */
    // Passamos o arquivo, o vetor de buffer e o endereço do ponteiro da função
    gera_codigo(fp, code, &funcLBS);
    
    // Fechamos o arquivo pois já lemos tudo
    fclose(fp);

    /* Verifica se deu certo */
    if (funcLBS == NULL) {
        printf("Erro grave: A funcao gera_codigo nao retornou endereco valido!\n");
        return 1;
    }

    printf("Rodando a funcao com p0 = %d\n", argumento_p0);

    /* Chama a função gerada */
    // Como seu código atual (ret $42) ignora o parâmetro, podemos passar qualquer coisa (ex: 0)
    res = (*funcLBS)(argumento_p0); 

    printf("Retorno da funcao LBS: %d\n", res);

    return 0;
}