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

int** Alocar_matriz(int m, int n);
int ehPrimo(int n);

int** matrix;

int main(int argc, char* argv[]) {
	pthread_t thread;

	matrix = Alocar_matriz(ALTURA, LARGURA);


	
	return 0;
}

int** Alocar_matriz(int m, int n)
{
	int** v; 
	int i; 
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

	return (v);
}

int ehPrimo(int n) {
	for (int i = 0; i < sqrt(n); i++) {
		if (n % i == 0) {
			return 1;
		}
	}
	return 0;
}