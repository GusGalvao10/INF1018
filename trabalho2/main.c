#include "gera_codigo.h"

int main(int argc, char *argv[]) {
    FILE *fp;
    funcp funcLBS;
    unsigned char code[300];
    int res;
    ...
    /* Abre o arquivo para leitura */
    fp = fopen(<nomearq>, "r");
    ...
    /* Gera o codigo */
    gera_codigo(fp, code, &funcLBS);
    if (funcLBS == NULL) {
        printf("Erro na geracao\n");
        ...
    }
    /* Chama a função gerada */
    res = (*funcLBS)(<argumento>);
    ...
}