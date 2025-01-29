#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#pragma comment(lib,"pthreadVC2.lib")

#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Determina as dimensoes da matriz MxN tal que M > 0 e N > 0;
#define DIMENSAO_MATRIZ_X 20000
#define DIMENSAO_MATRIZ_Y 20000

typedef struct Macrobloco {
	int dim_x;
	int dim_y;
} macrobloco;

int** Alocar_matriz();
void* parallel_task(void * args);
void serial_task();
int ehPrimo(int n);

int** matriz;
int numeros_primos = 0;
int bloco_livre = 0;
pthread_mutex_t mutex;

int main(int argc, char* argv[]) {
    
    clock_t t;
    pthread_t* threads = NULL;
    macrobloco blc;

    int num_threads;
    int opcao;
    
    printf("\n=== Contador de Primos ===");
	printf("\n\n--> Escolha a opcao:");
    printf("\n1. Busca Serial");
    printf("\n2. Busca Paralela");
    printf("\n3. Ambas");
    printf("\nDigite a opcao: ");
    scanf("%d", &opcao);

	printf("\nAlocando matriz %d x %d...\n", DIMENSAO_MATRIZ_X, DIMENSAO_MATRIZ_Y);
    matriz = Alocar_matriz();

    if (matriz == NULL) {
        perror("Erro ao alocar memória para matriz");
        return 1;
    }

    if (opcao == 1 || opcao == 3) {
        printf("\n=== Busca Serial ===\n");

        t = clock();

        serial_task();

        t = clock() - t;

        printf("\nQuantidade De Primos = %d \n", numeros_primos);
        printf("Tempo de execucao: %lf\n", ((double)t) / CLOCKS_PER_SEC);

        numeros_primos = 0;
    }

    if (opcao == 2 || opcao == 3) {
        printf("\n=== Busca Paralela ===\n");

        printf("\n--> Configuracao da busca paralela");
        printf("\nDigite o numero de threads: ");
        scanf("%d", &num_threads);

        printf("Digite a dimensao horizontal do bloco: ");
        scanf("%d", &blc.dim_x);
        printf("Digite a dimensao vertical do bloco: ");
        scanf("%d", &blc.dim_y);

        while (blc.dim_x > DIMENSAO_MATRIZ_X || blc.dim_y > DIMENSAO_MATRIZ_Y || blc.dim_x < 1 || blc.dim_y < 1) {
            printf("** Erro: Dimensao do macrobloco inválido **\n");
            printf("Digite a dimensao horizontal do bloco: ");
            scanf("%d", &blc.dim_x);
            printf("Digite a dimensao vertical do bloco: ");
            scanf("%d", &blc.dim_y);
        }

        // Criar threads
		printf("\nCriando %d threads...\n", num_threads);
        threads = malloc(num_threads * sizeof(pthread_t));

        if (threads == NULL) {
            perror("Erro ao alocar memória para threads");
            return 1;
        }

        pthread_mutex_init(&mutex, NULL);

        for (int i = 0; i < num_threads; i++) {
            pthread_create(&threads[i], NULL, parallel_task, (void *) &blc);
        }

		printf("\nIniciando busca paralela...\n");

        t = clock();

        // Esperar todas as threads terminarem
        for (int i = 0; i < num_threads; i++) {
            if (pthread_join(threads[i], NULL) != 0) {
                perror("Pthread_join falhou!");
                return 1;
            }
        }

        t = clock() - t;

        pthread_mutex_destroy(&mutex);

        printf("\nQuantidade De Primos = %d \n", numeros_primos);
        printf("Tempo de execucao: %lf\n", ((double)t) / CLOCKS_PER_SEC);
    }

    free(threads);
    for (int i = 0; i < DIMENSAO_MATRIZ_Y; i++) {
        free(matriz[i]);
    }
    free(matriz);

    return 0;
}

int** Alocar_matriz()
{
    int** v;
    int i;

    v = calloc(DIMENSAO_MATRIZ_Y, sizeof(int*));
    if (v == NULL) {
        printf("** Erro: Memoria Insuficiente **");
        return (NULL);
    }

    for (i = 0; i < DIMENSAO_MATRIZ_Y; i++) {
        v[i] = calloc(DIMENSAO_MATRIZ_X, sizeof(int));
        if (v[i] == NULL) {
            printf("** Erro: Memoria Insuficiente **");
            return (NULL);
        }
    }

	// Popular a matriz com numeros aleatorios
    srand(42);
    for (int i = 0; i < DIMENSAO_MATRIZ_Y; i++) {
        for (int j = 0; j < DIMENSAO_MATRIZ_X; j++) {
            v[i][j] = rand() % 32000;
        }
    }

    return (v);
}

// Funcao que calcula a quantidade de numeros primos em uma matriz m x n
void * parallel_task(void * args) {
    macrobloco* mb = (macrobloco*) args;
    int num_blocos = (DIMENSAO_MATRIZ_X / mb->dim_x) * (DIMENSAO_MATRIZ_Y / mb->dim_y);
    int bloco_atual;

	while (1) {
		pthread_mutex_lock(&mutex);
		if (bloco_livre >= num_blocos) {
			pthread_mutex_unlock(&mutex);
			break;
		}
        bloco_atual = bloco_livre;
		bloco_livre++;
        pthread_mutex_unlock(&mutex);
            
        int qtd_blc_linha = DIMENSAO_MATRIZ_X / mb->dim_x;;
  
        int linha_ini = (bloco_atual / qtd_blc_linha) * mb->dim_y;
        int linha_fim = linha_ini + mb->dim_y;

        int coluna_ini = (bloco_atual % qtd_blc_linha) * mb->dim_x;
        int coluna_fim = coluna_ini + mb->dim_x;

        int contador = 0;
        // Busca por números primos nesse bloco
        for (int i = linha_ini; i < linha_fim; i++) {
            for (int j = coluna_ini; j < coluna_fim; j++) {
				contador += ehPrimo(matriz[i][j]);
            }
        }

		pthread_mutex_lock(&mutex);
		numeros_primos += contador;
		pthread_mutex_unlock(&mutex);

        // Imprime a quantidade de primos por bloco
        // printf("Bloco %d: %d primos\n", i, contador);
    }

	return NULL;
}

void serial_task() {
    for (int i = 0; i < DIMENSAO_MATRIZ_Y; i++) {
        for (int j = 0; j < DIMENSAO_MATRIZ_X; j++) {
            numeros_primos += ehPrimo(matriz[i][j]);
        }
    }
}

int ehPrimo(int n) {
    if (n < 2) {
        return 0;
    }
    double limite = sqrt(n);
    for (int i = 2; i < limite; i++) {
        if (n % i == 0) {
            return 0;
        }
    }

    return 1;
}
