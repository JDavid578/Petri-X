
#ifndef PETRI_H
#define PETRI_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ----------------- Estruturas ----------------- */
typedef struct Lugar {
    int id;
    int tokens;
    int tokens_max;
    struct Lugar *next;
} Lugar;

typedef struct Arco {
    int origem;
    int destino;
    int peso;
    struct Arco *next;
} Arco;

typedef struct Transicao {
    int id;
    Arco *consumidores; // LT
    Arco *produtores;   // TL
    struct Transicao *next;
} Transicao;

typedef struct RedePetri {
    Lugar *lugares;
    Transicao *transicoes;
} RedePetri;

/* ----------------- Funções ----------------- */
RedePetri* criarRede();
Lugar* adicionarLugar(RedePetri *rede, int id, int tokens, int tokens_max);
Transicao* adicionarTransicao(RedePetri *rede, int id);
void adicionarArcoConsumidor(Transicao *t, int lugarId, int peso);
void adicionarArcoProdutor(Transicao *t, int lugarId, int peso);
Lugar* buscarLugar(RedePetri *rede, int id);
Transicao* buscarTransicao(RedePetri *rede, int id);

int pode(Transicao *t, RedePetri *rede);
int quer();

void executarTransicao(Transicao *t, RedePetri *rede);
void executarRede(RedePetri *rede, int iteracoes);

void imprimirTokens(RedePetri *rede);
void liberarRede(RedePetri *rede);

#endif

