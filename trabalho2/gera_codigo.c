/* Gustavo de Sousa Galvão (Sua Matrícula) (Sua Turma) */
/* Nome do Dupla (Matrícula) (Turma) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gera_codigo.h"

void gera_codigo(FILE *f, unsigned char code[], funcp *entry) {
    char linha[256];  // Buffer para ler cada linha do arquivo
    int idx = 0;      // Cursor: indica a próxima posição livre no vetor code[]
    
    // Variáveis auxiliares para leitura (usaremos no sscanf depois)
    char comando[32];
    
    // Loop principal: lê o arquivo linha por linha até o fim
    while (fgets(linha, 256, f) != NULL) {
        
        // Tenta ler a primeira palavra da linha para saber o comando
        // O " %s" com espaço antes ignora espaços em branco iniciais
        if (sscanf(linha, " %s", comando) != 1) {
            continue; // Linha vazia ou inválida, pula
        }

        // ---------------------------------------------------
        // CASO 1: Início de Função
        // ---------------------------------------------------
        if (strcmp(comando, "function") == 0) {
            // O enunciado diz: retornar o endereço da ÚLTIMA função lida.
            // Então, toda vez que achamos uma "function", atualizamos o entry.
            // O endereço é a posição atual de code, convertida para ponteiro de função.
            *entry = (funcp) &code[idx];

            //prologo: push %rbp
            code[idx++] = 0x55;

            //prologo: movq %rsp, %rbp
            code[idx++] = 0x48;
            code[idx++] = 0x89;
            code[idx++] = 0xe5;

            //subq $32, %rsp
            
            code[idx++] = 0x48;
            code[idx++] = 0x83;
            code[idx++] = 0xec;
            code[idx++] = 0x20;

            
            continue;
        }

        // ---------------------------------------------------
        // CASO 2: Fim de Função
        // ---------------------------------------------------
        if (strcmp(comando, "end") == 0) {
            //epilogo
            //leaveq
            code[idx++] = 0xc9;
            
            //ret
            code[idx++] = 0xc3;
            
            continue;
        }

        // ---------------------------------------------------
        // CASO 3: Retorno de Constante (ret $10)
        // ---------------------------------------------------
        if (strcmp(comando, "ret") == 0) {
            int constante;
            // Tenta ler "ret $número"
            // Note o '$' no formato do sscanf para ignorar o caractere $
            if (sscanf(linha, " ret $%d", &constante) == 1) {
                // AQUI VOCÊ VAI INSERIR O CÓDIGO PARA: mov $constante, %eax
                // code[idx++] = ...
                
                // Depois do mov, precisamos do 'ret'?
                // Não necessariamente, pois o 'end' já vai colocar o 'ret'.
                // Mas geralmente colocamos um jump para o epílogo ou o próprio epílogo aqui.
                // Dica: Por enquanto, assuma que o ret sai da função direto.
                // code[idx++] = 0xC3; // ret
            }
            continue;
        }

        // Outros comandos virão aqui (atribuições, chamadas, etc)...
    }
}