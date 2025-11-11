/***************************************************************************
 * petriv4.h                                  Version 20240927.150000    *
 * *
 * Simulador de rede de Petri (Funcional e aprimorado)                     *
 * Copyright (C) 2024         by Ruben Carlo Benante & Gemini              *
 ***************************************************************************
 * Baseado nos requisitos do README.md e nos códigos-fonte fornecidos.   *
 * Este programa utiliza listas encadeadas, implementa a lógica de       *
 * disparo não-determinístico, E/S via arquivo ou modo interativo e      *
 * gera estatísticas detalhadas, incluindo desvio padrão.                *
 ***************************************************************************/

#ifndef _PETRIV4_H
#define _PETRIV4_H

/* ---------------------------------------------------------------------- */
/* Includes */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h> // Para getopt()
#include <math.h>   // Para sqrt()

/* ---------------------------------------------------------------------- */
/* Definições */

#ifndef VERSION
#define VERSION "20240927.150000"
#endif

#define MAX_TOKENS 1000000
#define TRANSITION_PROBABILITY 0.55 // Probabilidade de 55% (quer?)

/* ---------------------------------------------------------------------- */
/* Estruturas de Dados */

typedef struct Arco {
    int origem;
    int destino;
    int peso;
    struct Arco *prox;
} Arco;

typedef struct Lugar {
    int id;
    int tokens;
    int max_tokens;
    int min_tokens;
    long long soma_tokens;          // Acumulador para a média
    long double soma_tokens_quadrado; // Acumulador para o desvio padrão
    long n_amostras;
    struct Lugar *prox;
} Lugar;

typedef struct Transicao {
    int id;
    int execucoes;
    struct Transicao *prox;
} Transicao;

typedef struct RedePetri {
    int n_lugares;
    int n_transicoes;
    Lugar *lugares;
    Transicao *transicoes;
    Arco *arcos_LT; // Arcos Consumidores (Lugar -> Transição)
    Arco *arcos_TL; // Arcos Produtores (Trasição -> Lugar)
    long long iteracoes;
    time_t inicio_simulacao;
    time_t fim_simulacao;
    long long transicoes_analisadas;
    int deadlock; // Flag para indicar o status final
} RedePetri;


/* ---------------------------------------------------------------------- */
/* Protótipos das Funções */

// Funções do menu e ajuda
void help(const char *prog_name);
void copyr(const char *prog_name);

// Funções para criação e manipulação da rede
RedePetri* criar_rede_petri(int lugares, int transicoes);
void adicionar_lugar(RedePetri *rede, int id, int tokens_iniciais);
void adicionar_transicao(RedePetri *rede, int id);
void adicionar_arco_LT(RedePetri *rede, int lugar, int transicao, int peso);
void adicionar_arco_TL(RedePetri *rede, int transicao, int lugar, int peso);
void liberar_rede(RedePetri *rede);

// Funções para leitura da rede
RedePetri* ler_rede_arquivo(const char *arquivo);
RedePetri* ler_rede_interativa();

// Funções do núcleo da simulação
void simular_rede(RedePetri *rede, int max_iteracoes);
int transicao_habilitada(RedePetri *rede, int transicao_id);
void executar_transicao(RedePetri *rede, int transicao_id);
Lugar* encontrar_lugar(RedePetri *rede, int id);

// Funções de estatísticas
void imprimir_estatisticas(RedePetri *rede);
void atualizar_estatisticas_lugares(RedePetri *rede);

#endif /* _PETRIV4_H */
