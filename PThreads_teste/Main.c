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

#define DIMENSAO_MATRIZ 10000

int** Alocar_matriz();
void parallel_task(int dimensao_bloco);
void serial_task();
int ehPrimo(int n);

int** matriz;
int numeros_primos = 0;
int* blocos;
pthread_mutex_t mutex;

int main(int argc, char* argv[]) {
    
    clock_t t;
    pthread_t* threads = NULL;

    int num_threads;
    int dimensao_bloco = 10000;

    printf("=== Configuração da busca paralela ===");
    printf("\nDigite o número de threads: ");
    scanf("%d", &num_threads);
    printf("Digite a dimensão do bloco: ");
    scanf("%d", &dimensao_bloco);

    matriz = Alocar_matriz();
    blocos = calloc((DIMENSAO_MATRIZ / dimensao_bloco) * (DIMENSAO_MATRIZ / dimensao_bloco), sizeof(int));
    threads = malloc(num_threads * sizeof(pthread_t));

    if (threads == NULL) {
        perror("Erro ao alocar memória para threads");
        return 1;
    }

    printf("\n== Teste Serial ==\n");

    t = clock();

    serial_task(dimensao_bloco);

    t = clock() - t;

    printf("\nQuantidade De Primos = %d \n", numeros_primos);
    printf("Tempo de execucao: %lf\n", ((double)t) / CLOCKS_PER_SEC);

    numeros_primos = 0;

	printf("\n== Teste Paralelo ==\n");

    pthread_mutex_init(&mutex, NULL);

    // Criar threads
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, parallel_task, dimensao_bloco);
    }

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

    free(threads);
    free(blocos);
    for (int i = 0; i < DIMENSAO_MATRIZ; i++) {
        free(matriz[i]);
    }
    free(matriz);

    return 0;
}

int** Alocar_matriz()
{
    int** v;
    int i;

    if (DIMENSAO_MATRIZ < 1) {
        printf("** Erro: Parametro invalido **\n");
        return (NULL);
    }

    v = calloc(DIMENSAO_MATRIZ, sizeof(int*));
    if (v == NULL) {
        printf("** Erro: Memoria Insuficiente **");
        return (NULL);
    }

    for (i = 0; i < DIMENSAO_MATRIZ; i++) {
        v[i] = calloc(DIMENSAO_MATRIZ, sizeof(int));
        if (v[i] == NULL) {
            printf("** Erro: Memoria Insuficiente **");
            return (NULL);
        }
    }

	// Popular a matriz com numeros aleatorios
    srand(42);
    for (int i = 0; i < DIMENSAO_MATRIZ; i++) {
        for (int j = 0; j < DIMENSAO_MATRIZ; j++) {
            v[i][j] = rand() % 32000;
        }
    }

    return (v);
}

// Funcao que calcula a quantidade de numeros primos em uma matriz m x n
// Essa função deu ruim fml! -->
void parallel_task(int dimensao_bloco) {
    int num_blocos = (DIMENSAO_MATRIZ / dimensao_bloco) * (DIMENSAO_MATRIZ / dimensao_bloco);
    
    for (int i = 0; i < num_blocos; i++) {
		pthread_mutex_lock(&mutex);
        if (blocos[i] == 0) {
            blocos[i] = 1;
            pthread_mutex_unlock(&mutex);
            
            int qtd_blc_linha;
            if (DIMENSAO_MATRIZ % dimensao_bloco != 0) {
                qtd_blc_linha = DIMENSAO_MATRIZ / dimensao_bloco + 1;
            }
            else {
                qtd_blc_linha = DIMENSAO_MATRIZ / dimensao_bloco;
            }

            int linha_ini = (i / qtd_blc_linha) * dimensao_bloco;
            int linha_fim = linha_ini + dimensao_bloco;
            if (linha_fim > DIMENSAO_MATRIZ) linha_fim = DIMENSAO_MATRIZ - 1; // Garantir que não ultrapasse os limites

            int coluna_ini = (i % qtd_blc_linha) * dimensao_bloco;
            int coluna_fim = coluna_ini + dimensao_bloco;
            if (coluna_fim > DIMENSAO_MATRIZ) coluna_fim = DIMENSAO_MATRIZ - 1; // Garantir que não ultrapasse os limites

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

            printf("Bloco %d: %d primos\n", i, contador);
        } else {
			pthread_mutex_unlock(&mutex);
        }
    }
}

void serial_task() {
    for (int i = 0; i < DIMENSAO_MATRIZ; i++) {
        for (int j = 0; j < DIMENSAO_MATRIZ; j++) {
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
