#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#pragma comment(lib,"pthreadVC2.lib")
#define HAVE_STRUCT_TIMESPEC

#define LARGURA 10000
#define ALTURA 10000
#define PRIME_NUMBERS 0

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int** Alocar_matriz(int m, int n);
int ehPrimo(int n);

int** matrix;
int qtd_numeros_primos = 0;


int main(int argc, char* argv[]) {

    clock_t t;
    
    t = clock();

    pthread_t thread;

    matrix = Alocar_matriz(ALTURA, LARGURA);

    qtd_de_n_primos();

    t = clock() - t;

    printf("Quantidade De Primos = %d \n", qtd_numeros_primos);
    printf("Tempo de execucao: %lf", ((double)t) / ((CLOCKS_PER_SEC / 1000)));

    return 0;
}

int** Alocar_matriz(int m, int n)
{
    int** v;
    int i;
    int qtdEhPrimos = 0;

    if (m < 1 || n < 1) {
        printf("** Erro: Parametro invalido **\n");
        return (NULL);
    }

    v = calloc(m, sizeof(int*));
    if (v == NULL) {
        printf("** Erro: Memoria Insuficiente **");
        return (NULL);
    }

    for (i = 0; i < m; i++) {
        v[i] = calloc(n, sizeof(int));
        if (v[i] == NULL) {
            printf("** Erro: Memoria Insuficiente **");
            return (NULL);
        }
    }

    srand(time(NULL));

    /*
    coloca qualquer numero dentro do srand() pra seed fixa
    */

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            v[i][j] = rand() % 32000;
            //printf("%i ", v[i][j]);
        }
        //printf("\n");
    }
    

    return (v);
}

int qtd_de_n_primos() {
    
    for (int i = 0; i < ALTURA; i++) {
        for (int j = 0; j < LARGURA; j++) {
            qtd_numeros_primos += ehPrimo(matrix[i][j]);
        }
    }
}

int ehPrimo(int n) {

    if (n < 2) {
        return 0;
    }
    for (int i = 2; i < sqrt(n); i++) {
        if (n % i == 0) {
            return 0;
        }
    }

    return 1;
}
