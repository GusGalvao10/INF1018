/* Gustavo de Sousa Galvão (2410508) (3wA) */
/* Joao Gabriel Gralha Quirino de Souza (2310284) (3wA) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gera_codigo.h"

// o macro "emitter" foi escolhido apenas para não precisar escrever tanto a mesma coisa
#define EMIT(b) code[idx++] = (b) 

// única funcao auxiliar. Serve apenas para ficar mais fácil de escrever inteiros em bytes
static void escreve_int(unsigned char code[], int *idx, int valor) {
    code[(*idx)++] = valor & 0xFF;
    code[(*idx)++] = (valor >> 8) & 0xFF;
    code[(*idx)++] = (valor >> 16) & 0xFF;
    code[(*idx)++] = (valor >> 24) & 0xFF;
}

void gera_codigo(FILE *f, unsigned char code[], funcp *entry) {
    int c; // Caractere lido
    int idx = 0; // Seu cursor no vetor code
    
    // Variáveis auxiliares para o parsing (do trataent.c)
    char c0, var0, var1, var2, op;
    int idx0, idx1, idx2; 

    //vetor para guardar o end (idx) onde cmc cada funcao
    int pos_funcoes[50];
    
    // contador pra saber qual funcao eh a atual
    int num_funcao_atual = 0;
    // Loop principal: lê caractere por caractere (igual ao trataent.c)
    while ((c = fgetc(f)) != EOF) {
        
        switch (c) {
            
            // --------------------------------------------------------
            // CASO 'f': function
            // --------------------------------------------------------
            case 'f': { 
                if (fscanf(f, "unction%c", &c0) != 1) break; // Consome o resto da palavra
                
                //salva a pos atual para a funcao corrente
                pos_funcoes[num_funcao_atual] = idx;
                num_funcao_atual++; // inc para a próxima

                // 1. Atualiza o ponto de entrada (endereço da função atual)
                *entry = (funcp) &code[idx];

                // 2. Gera o Prólogo (Assembly)
                // push %rbp
                EMIT(0x55); 
                // mov %rsp, %rbp
                EMIT(0x48); EMIT(0x89); EMIT(0xE5);
                // sub $32, %rsp (Espaço para v0-v4)
                EMIT(0x48); EMIT(0x83); EMIT(0xEC); EMIT(0x20);

                // para casos especificos, é bom proteger melhor o (%edi), para casos
                // como, por exemplo, recursão.
                
                //mov %edi, -32(%rbp)
                EMIT(0x89);
                EMIT(0x7d);
                EMIT(0xE0);
                break;
            }

            // --------------------------------------------------------
            // CASO 'e': end
            // --------------------------------------------------------
            case 'e': { 
                if (fscanf(f, "nd%c", &c0) != 1) break;

                // Gera o Epílogo
                EMIT(0xC9); // leave
                EMIT(0xC3); // ret
                break;
            }

            // --------------------------------------------------------
            // CASO 'r': ret (Retorno Incondicional)
            // --------------------------------------------------------
            case 'r': { 
                // O trataent lê: "et tipo valor" (ex: "et $ 10" ou "et v 0")
                if (fscanf(f, "et %c%d", &var0, &idx0) != 2) break;
                
                // Aqui você decide qual MOV gerar baseado no 'var0'
                if (var0 == '$') {
                    // mov $idx0, %eax
                    EMIT(0xB8);
                    escreve_int(code, &idx, idx0);
                } 
                else if (var0 == 'p') {
                    // mov -32(%rbp),%eax
                    EMIT(0x8b);
                    EMIT(0x45);
                    EMIT(0xe0);
                    
                }
                else if (var0 == 'v') {
                    int offset = (idx0 + 1) * -4;
                    EMIT(0x8b);
                    EMIT(0x45);
                    EMIT(offset & 0xff);
                }

                // Sai da função (pulo para o epílogo ou repete o epílogo)
                EMIT(0xC9); // leave
                EMIT(0xC3); // ret
                break;
            }

            // --------------------------------------------------------
            // CASO 'z': zret (Retorno Condicional)
            // --------------------------------------------------------
            case 'z': {
               if (fscanf(f, "ret %c%d %c%d", &var0, &idx0, &var1, &idx1) != 4) break;
               
               //primeiro, movemos o operando de condicao (var0/idx0) para %ecx
               if (var0 == '$'){
                // mov $constante, %ecx (Opcode B9 + 4 bytes)
                EMIT(0xb9);
                escreve_int(code,&idx,idx0);
               } else if (var0 == 'p'){
                // mov -32(%rbp), %ecx
                EMIT(0x8b);
                EMIT(0x4d);
                EMIT(0xe0);
               } else if (var0 == 'v') {
                // mov -offset(%rbp), %ecx
                // offset = (indice + 1) * -4
                //v0 (-4) v1 (-8) ... v4 (-20)
                int offset = (idx1 + 1) * -4;

                EMIT(0x8b);
                EMIT(0x45);
                EMIT(offset & 0xff); //só o primeiro byte do offset contem o numero
               }

               // cmp $0, %ecx
               EMIT(0x83);
               EMIT(0xF9);
               EMIT(0x00);

               // pulo condicional
               // jne, se nao for zero, pule as instrucoes de retoro
               // que vêm a seguir
               EMIT(0x75);
               
               //guardamos a posicao que devemos escrever o tamanho do pulo
               int pos_do_pulo = idx;
               EMIT(0x00); // inicializamos ela com zero, a principio

               if (var1 == '$'){
                // mov $const, %eax
                EMIT(0xB8);
                escreve_int(code,&idx,idx1);
               } else if (var1 == 'p') {
                // le p0 direto da memoria por seguranca
                // mov -32(%rbp), %eax
                EMIT(0x8b);
                EMIT(0x45);
                EMIT(0xE0);
            } else if (var1 == 'v'){
                // mov -offset(%rbp), %eax
                int offset = (idx1 + 1) * -4;
                EMIT(0x8b);
                EMIT(0x45);
                EMIT(offset & 0xff);
               }

               // finaliza a func
               EMIT(0xC9);
               EMIT(0xc3);

               // calculo do pulo
               // o tamanho do pulo é: (onde estou agora) - (pos logo apos o cmnd de pulo)
               // pos logo apos o comando de pulo = pos_do_pulo + 1
               unsigned char tamanho_do_pulo = idx - (pos_do_pulo + 1);
               
               // voltamos e escrevermos o valor correto agora
               code[pos_do_pulo] = tamanho_do_pulo;
               
               break;
            }

            // --------------------------------------------------------
            // CASO 'v': Atribuição (v0 = ...)
            // --------------------------------------------------------
            case 'v': {
                // Lê quem recebe o valor: "0 ="
                if (fscanf(f, "%d = %c", &idx0, &c0) != 2) break;

                // Verifica se é CALL ou OPERAÇÃO ARITMÉTICA
                if (c0 == 'c') { /* call */
                    int f_id, idx_arg;
                    char tipo_arg;
                   if (fscanf(f, "all %d %c%d", &f_id, &tipo_arg, &idx_arg) != 3) break;
                   
                   if (tipo_arg == '$'){
                    EMIT(0xbf); // mov $const, $edi
                    escreve_int(code, &idx, idx_arg);
                   } else if (tipo_arg == 'p') {
                    //mov -32(%rbp), %edi
                    EMIT(0x8b);
                    EMIT(0x7d);
                    EMIT(0xe0);
                   } else if (tipo_arg == 'v'){
                    // mov -offset(%rbp), %edi
                    EMIT(0x8b);
                    EMIT(0x7d);
                    EMIT(((idx_arg + 1) * -4) & 0xff);
                   }

                   EMIT(0xe8);
                   // calculo do deslocamento
                   // deslocamento = (onde a func que queremos esta) - onde a call termina
                   // a call termina onde a idx atual está + 4 bytes, pois esse é o tamanho da call
                   int pos_alvo = pos_funcoes[f_id];
                   int pos_futura = idx + 4;
                   int deslocamento = pos_alvo - pos_futura;

                   escreve_int(code, &idx, deslocamento);

                   //por fim, pegamos de volta o valor de (%eax) e salvamos na var (v0...)
                   // o resultado da func chamada volta em %eax
                   // precisamos entao mover de %eax para a var local de destino (idx0)

                   //mov %eax, -offset(%rbp)
                   EMIT(0x89);
                   EMIT(0x45);
                   EMIT(((idx0 + 1) * -4) & 0xff);
                }
                else { /* Operação (+, -, *) */
                   // O 'c0' já pegou o primeiro caractere do primeiro operando
                   // Precisamos ler o resto: "ariavel1 op variavel2"
                   var1 = c0; 
                   // Note o formato: lê o resto do primeiro num, o op, e o segundo par
                   if (fscanf(f, "%d %c %c%d", &idx1, &op, &var2, &idx2) != 4) break;
                   //faz as verificacoes do operador 1
                   if (var1 == '$'){
                    EMIT(0xb9); // mov $const, %ecx
                    escreve_int(code,&idx,idx1);
                   } else if (var1 == 'p'){
                    // mov -32(%rbp), %ecx
                    EMIT(0x8b);
                    EMIT(0x4d);
                    EMIT(0xe0);
                   } else if (var1 == 'v'){
                    EMIT(0x8b);
                    EMIT(0x4d); // mov -off(%rbp), %ecx
                    EMIT((((idx1 + 1) * -4)) & 0xff);
                   }

                   // carregar segundo op em %eax

                   if (var2 == '$'){
                    EMIT(0xb8); // mov $const, %eax
                    escreve_int(code,&idx,idx2);
                   } 
                    else if (var2 == 'p'){
                    // mov -32(%rbp), %eax 
                    EMIT(0x8B);
                    EMIT(0x45);
                    EMIT(0xE0);

                   }
                    else if (var2 == 'v'){
                    EMIT(0x8b);
                    EMIT(0x45); // mov -off(%rbp), %eax
                    EMIT((((idx2 + 1) * -4)) & 0xff);
                   }

                   //carregando o valor da operacao em %ecx

                   if (op == '+') {
                    EMIT(0x01);
                    EMIT(0xc1); // add %eax, %ecx
                   } else if (op == '-') {
                    EMIT(0x29);
                    EMIT(0xc1); // sub %eax, %ecx
                   } else if(op == '*'){
                    EMIT(0x0f);
                    EMIT(0xaf);
                    EMIT(0xc8); //imul %eax, %ecx
                   }

                   //salvar resultado (%ecx) no destino (v0, por exemplo)

                   //mov %ecx, -offset(%rbp)
                   EMIT(0x89);
                   EMIT(0x4d); //mov %ecx
                   EMIT(((idx0 + 1) * -4) & 0xff);
                }
                break;
            }
        }
        
        // Consumir quebra de linha e espaços após o comando
        fscanf(f, " ");
    }
}