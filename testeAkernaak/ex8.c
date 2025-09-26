/*****************************************************************************
 * exN.c                                    Version 20180714.101818        *
 * *
 * Biblioteca do Akernaak                                                  *
 * Copyright (C) 2016-2018    by Ruben Carlo Benante                         *
 *****************************************************************************/

/* ------------------------------------------------------------------------- */
/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include "ex8.h"

/* ------------------------------------------------------------------------- */
/* Variaveis de estado do sorteio (globais estaticas) */

static int verb = 0; /**< verbose level, global within the file */

// Buffer para formatar frases com %s ou %d
static char buffer_frase_formatada[SBUFF];

// Controle de frases unicas (indices 0 a 7 do array labmodel)
static bool frases_usadas[8] = {false};

// Controle para a primeira aparicao da Categoria C (numero magico)
static bool categoria_C_ja_saiu = false;

// Contadores de estado do jogo
static int turno = 0;
static int frases_unicas_sorteadas = 0;

// Indice da variacao da dica 4 a ser usada no jogo (4 ou 5)
static int indice_dica4_variacao;


/* ------------------------------------------------------------------------- */
/**
 * @brief Ponto de entrada principal do programa.
 */
int main(int argc, char *argv[])
{
    int opt; /* return from getopt() */

    /* getopt() configured options */
    opterr = 0;
    while((opt = getopt(argc, argv, "vhV")) != EOF)
        switch(opt)
        {
            case 'h':
                help();
                break;
            case 'V':
                copyr();
                break;
            case 'v':
                verb++;
                break;
            case '?':
            default:
                printf("Type\n\t$man %s\nor\n\t$%s -h\nfor help.\n\n", argv[0], argv[0]);
                return EXIT_FAILURE;
        }

    if(verb)
        printf("Verbose level set at: %d\n", verb);

    /* --- INICIO DA LOGICA DA ATIVIDADE --- */
    
    // 1. Inicializa o sistema de sorteio
    exN_init();
    
    printf("Simulador de Sorteio de Frases do Jogo Akernaak\n");
    printf("--------------------------------------------------\n\n");
    
    // 2. Realiza 20 sorteios para demonstrar o funcionamento
    for (int i = 1; i <= 20; i++) {
        printf("Sorteio %d: %s", i, sortear_frase());
    }

    printf("\n--------------------------------------------------\n");
    printf("Simulacao concluida.\n");
    
    /* --- FIM DA LOGICA DA ATIVIDADE --- */

    return EXIT_SUCCESS;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief Realiza o sorteio de uma frase seguindo as regras do jogo.
 */
const char* sortear_frase(void)
{
    turno++;

    // CORRECAO 1: A condicao de fim de jogo agora e' >= 7 (1 Cat A + 6 Cat B)
    if (frases_unicas_sorteadas >= 7) {
        return labmodel[9]; // Retorna "Titulo"
    }
    
    int categoria_sorteada = -1; // 0=A, 1=B, 2=C

    // Logica do primeiro turno (33% de chance para A, B ou C)
    if (turno == 1) {
        categoria_sorteada = rand() % 3;
    } else {
        // Logica dos turnos seguintes
        bool a_disponivel = !frases_usadas[0];
        bool c_disponivel = !categoria_C_ja_saiu;
        
        int dicas_b_restantes = 0;
        for (int i = 1; i <= 7; i++) {
            if (i != 4 && i != 5) {
                if (!frases_usadas[i]) dicas_b_restantes++;
            } else if (i == indice_dica4_variacao) {
                if (!frases_usadas[i]) dicas_b_restantes++;
            }
        }
        bool b_disponivel = dicas_b_restantes > 0;

        if (!b_disponivel && !a_disponivel) {
            // Se nao ha mais frases unicas, a condicao de >= 7 no inicio da funcao devera resolver.
            // Mas, por seguranca, forcamos a categoria C.
            categoria_sorteada = 2;
        }
        else if (a_disponivel && c_disponivel) { // Prioridade: sortear A e C
            categoria_sorteada = (rand() % 2 == 0) ? 0 : 2; // 0=A, 2=C
        } 
        else if (a_disponivel && !c_disponivel) { // Falta A e talvez B
             categoria_sorteada = (rand() % 2 == 0) ? 0 : 1; // 0=A, 1=B
        }
        else if (!a_disponivel && c_disponivel) { // Falta C e talvez B
            categoria_sorteada = (rand() % 2 == 0) ? 2 : 1; // 2=C, 1=B
        }
        else if (!a_disponivel && !c_disponivel && b_disponivel) { // A e C ja sairam, faltam dicas B
            categoria_sorteada = (rand() % 3 < 2) ? 1 : 2; // 66% B (1), 33% C (2)
        }
    }
    
    // Processamento do sorteio
    switch (categoria_sorteada) {
        case 0: // Categoria A (passagem secreta)
            if (!frases_usadas[0]) {
                frases_usadas[0] = true;
                frases_unicas_sorteadas++;
                snprintf(buffer_frase_formatada, SBUFF, labmodel[0], "biblioteca");
                return buffer_frase_formatada;
            }
            // Se A ja foi usada, tenta sortear B como alternativa
            // (intencionalmente sem break para cair no proximo case)
            // NOVO: A logica acima de selecao de categoria ja cuida disso.

            // CORRECAO 2: Adicionado 'break' para logica correta
            break; 
        
        case 1: // Categoria B (dicas)
            for (int i = 1; i <= 7; i++) {
                // Checa a variacao correta da dica 4
                bool is_dica4_valida = (i == indice_dica4_variacao);
                // Checa se e uma dica normal (nao e 4 nem 5)
                bool is_dica_normal = (i != 4 && i != 5);

                if ((is_dica_normal || is_dica4_valida) && !frases_usadas[i]) {
                    frases_usadas[i] = true;
                    frases_unicas_sorteadas++;
                    return labmodel[i];
                }
            }
            // CORRECAO 2: Adicionado 'break'
            break;

        case 2: // Categoria C (numero magico)
            categoria_C_ja_saiu = true;
            snprintf(buffer_frase_formatada, SBUFF, labmodel[8], (rand() % 100) + 1);
            return buffer_frase_formatada;
            // Nao precisa de break pois ha um return
    }
    
    // Se chegou ate aqui, algo deu errado. Tenta sortear uma frase magica como ultimo recurso.
    // Isso evita a mensagem de erro.
    categoria_C_ja_saiu = true;
    snprintf(buffer_frase_formatada, SBUFF, labmodel[8], (rand() % 100) + 1);
    return buffer_frase_formatada;
}



/* ------------------------------------------------------------------------- */
/**
 * @brief Inicializa as variaveis de estado para o sorteio.
 */
void ex8_init(void)
{
    IFDEBUG("ex8_init()");
    srand(time(NULL));

    // Zera os contadores e flags de estado
    turno = 0;
    frases_unicas_sorteadas = 0;
    categoria_C_ja_saiu = false;
    for (int i = 0; i < 8; i++) {
        frases_usadas[i] = false;
    }
    
    // Sorteia qual variacao da "dica 4" sera usada (indice 4 ou 5 do array)
    indice_dica4_variacao = (rand() % 2 == 0) ? 4 : 5;
    
    return;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief Imprime informacoes de ajuda e sai do programa.
 */
void help(void)
{
    IFDEBUG("help()");
    printf("%s - Simulador de Sorteio de Frases Akernaak\n", "exN");
    printf("\nUsage: %s [-h|-v|-V]\n", "exN");
    printf("\nOptions:\n");
    printf("\t-h,  --help\n\t\tShow this help.\n");
    printf("\t-V,  --version\n\t\tShow version and copyright information.\n");
    printf("\t-v,  --verbose\n\t\tSet verbose level (cumulative).\n");
    printf("\nExit status:\n\t0 if ok.\n\t1 some error occurred.\n");
    printf("\nAuthor:\n\tWritten by %s <%s>\n\n", "Ruben Carlo Benante", "rcb@beco.cc");
    exit(EXIT_FAILURE);
}

/* ------------------------------------------------------------------------- */
/**
 * @brief Imprime a versao, copyright e sai do programa.
 */
void copyr(void)
{
    IFDEBUG("copyr()");
    printf("%s - Version %s\n", "exN", VERSION);
    printf("\nCopyright (C) %d %s <%s>, GNU GPL version 2 <http://gnu.org/licenses/gpl.html>.\n\n", 2018, "Ruben Carlo Benante", "rcb@beco.cc");
    exit(EXIT_FAILURE);
}
