/*****************************************************************************
 * ex8.c                                    Version 20250925.215000        *
 * *
 * Biblioteca do Akernaak                                                  *
 * Copyright (C) 2016-2018    by Ruben Carlo Benante                         *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include "ex8.h"

static int verb = 0;
static char buffer_frase_formatada[SBUFF];

// MUDANCA: Array agora tem 9 posicoes para incluir a frase 8 (Cat C) no controle
static bool frases_usadas[9] = {false}; 
static int frases_unicas_sorteadas = 0;
static int turno = 0;
static int indice_dica4_variacao;

/* ------------------------------------------------------------------------- */
int main(int argc, char *argv[])
{
    int opt;
    opterr = 0;
    while((opt = getopt(argc, argv, "vhV")) != EOF)
        switch(opt)
        {
            case 'h': help(); break;
            case 'V': copyr(); break;
            case 'v': verb++; break;
            case '?':
            default:
                printf("Type\n\t$man %s\nor\n\t$%s -h\nfor help.\n\n", argv[0], argv[0]);
                return EXIT_FAILURE;
        }

    if(verb)
        printf("Verbose level set at: %d\n", verb);

    ex8_init();
    printf("Simulador de Sorteio de Frases do Jogo Akernaak\n");
    printf("--------------------------------------------------\n\n");

    for (int i = 1; i <= 20; i++) {
        // MUDANCA: Formato de saida melhorado
        printf("Sorteio %d:\nAntes que o livro desapareca de suas maos, voce le:\n", i);
        printf("%s", sortear_frase());
    }

    printf("\n--------------------------------------------------\n");
    printf("Simulacao concluida.\n");
    return EXIT_SUCCESS;
}

/* ------------------------------------------------------------------------- */
const char* sortear_frase(void)
{
    turno++;

    // MUDANCA: Condicao de fim de jogo agora exige 8 frases unicas (A+B+C)
    if (frases_unicas_sorteadas >= 8) {
        return labmodel[9]; // Retorna "Titulo"
    }
    
    int categoria_sorteada = -1; // 0=A, 1=B, 2=C

    if (turno == 1) {
        categoria_sorteada = rand() % 3;
    } else {
        bool a_disponivel = !frases_usadas[0];
        // MUDANCA: Checa se a Cat C (frase 8 no array) ja foi usada uma vez
        bool c_disponivel = !frases_usadas[8]; 
        
        int dicas_b_restantes = 0;
        for (int i = 1; i <= 7; i++) {
            bool is_dica4_valida = (i == indice_dica4_variacao);
            bool is_dica_normal = (i != 4 && i != 5);
            if ((is_dica_normal || is_dica4_valida) && !frases_usadas[i]) {
                dicas_b_restantes++;
            }
        }
        bool b_disponivel = dicas_b_restantes > 0;

        if (a_disponivel && c_disponivel)
            categoria_sorteada = (rand() % 2 == 0) ? 0 : 2;
        else if (a_disponivel && !c_disponivel)
             categoria_sorteada = (rand() % 2 == 0) ? 0 : 1;
        else if (!a_disponivel && c_disponivel)
            categoria_sorteada = (rand() % 2 == 0) ? 2 : 1;
        else if (!a_disponivel && !c_disponivel && b_disponivel)
            categoria_sorteada = (rand() % 3 < 2) ? 1 : 2;
        else // Se A e B acabaram, força o sorteio de C se ele ainda for necessario
            categoria_sorteada = 2;
    }
    
    switch (categoria_sorteada) {
        case 0: // Categoria A
            if (!frases_usadas[0]) {
                frases_usadas[0] = true;
                frases_unicas_sorteadas++;
                snprintf(buffer_frase_formatada, SBUFF, labmodel[0], "biblioteca");
                return buffer_frase_formatada;
            }
            break; 
        
        case 1: // Categoria B
            for (int i = 1; i <= 7; i++) {
                bool is_dica4_valida = (i == indice_dica4_variacao);
                bool is_dica_normal = (i != 4 && i != 5);
                if ((is_dica_normal || is_dica4_valida) && !frases_usadas[i]) {
                    frases_usadas[i] = true;
                    frases_unicas_sorteadas++;
                    return labmodel[i];
                }
            }
            break;

        case 2: // Categoria C
            // MUDANCA: Conta a Cat C como unica na primeira vez que aparece
            if (!frases_usadas[8]) {
                frases_usadas[8] = true;
                frases_unicas_sorteadas++;
            }
            snprintf(buffer_frase_formatada, SBUFF, labmodel[8], (rand() % 100) + 1);
            return buffer_frase_formatada;
    }

    // Fallback de seguranca
    snprintf(buffer_frase_formatada, SBUFF, labmodel[8], (rand() % 100) + 1);
    return buffer_frase_formatada;
}

/* ------------------------------------------------------------------------- */
void ex8_init(void)
{
    srand(time(NULL));
    turno = 0;
    frases_unicas_sorteadas = 0;
    // MUDANCA: Zera o array de 9 posicoes
    for (int i = 0; i < 9; i++) frases_usadas[i] = false;
    indice_dica4_variacao = (rand() % 2 == 0) ? 4 : 5;
}

/* ------------------------------------------------------------------------- */
void help(void)
{
    printf("%s - Simulador de Sorteio de Frases Akernaak\n", "ex8");
    printf("\nUsage: %s [-h|-v|-V]\n", "ex8");
    printf("\nOptions:\n");
    printf("\t-h,  --help\n\t\tShow this help.\n");
    printf("\t-V,  --version\n\t\tShow version and copyright information.\n");
    printf("\t-v,  --verbose\n\t\tSet verbose level (cumulative).\n");
    exit(EXIT_FAILURE);
}

void copyr(void)
{
    printf("%s - Version %s\n", "ex8", VERSION);
    printf("\nCopyright (C) %d %s <%s>, GNU GPL version 2 <http://gnu.org/licenses/gpl.html>.\n\n", 2018, "Ruben Carlo Benante", "rcb@beco.cc");
    exit(EXIT_FAILURE);
}
