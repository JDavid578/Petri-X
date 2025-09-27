/***************************************************************************
 *   ex7.h                                    Version 20160806.231942      *
 *                                                                         *
 *   Simulador de rede de Petri                                            *
 *   Copyright (C) 2016         by Ruben Carlo Benante                     *
 ***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.        *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 *   To contact the author, please write to:                               *
 *   Ruben Carlo Benante                                                   *
 *   Email: rcb@beco.cc                                                    *
 *   Webpage: www.beco.cc                                                  *
 *   Phone: +55 (81) 3184-7555                                             *
 ***************************************************************************/

/* ---------------------------------------------------------------------- */
/**
 * \file petri.h
 * \ingroup GroupUnique
 * \brief Petri Net Simulator - header file
 * \details This program really do a nice job as a template, and template only!
 * \version 20160806.231942
 * \date 2016-08-06
 * \author Ruben Carlo Benante <<rcb@beco.cc>>
 * \par Webpage
 * <<a href="www.beco.cc">www.beco.cc</a>>
 * \copyright (c) 2016 GNU GPL v2
 * \note This program is free software: you can redistribute it
 * and/or modify it under the terms of the
 * GNU General Public License as published by
 * the Free Software Foundation version 2 of the License.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.
 * If not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA. 02111-1307, USA.
 * Or read it online at <<http://www.gnu.org/licenses/>>.
 *
 *
 * \todo Now that you have the template, hands on! Programme!
 * \warning Be carefull not to lose your mind in small things.
 * \bug This file right now does nothing usefull
 *
 */

#ifndef _PETRI_H
#define _PETRI_H

/* ---------------------------------------------------------------------- */
/* includes */

 #include <stdio.h>  /* Standard I/O functions */
 #include <stdlib.h>  /* Miscellaneous functions (rand, malloc, srand)*/

#include <time.h>  /* Time and date functions */
#include <string.h>  /* Strings functions definitions */
#include <unistd.h>  /* UNIX standard function */
/* #include "libeco.h" */ /* I/O, Math, Sound, Color, Portable Linux/Windows */
/* #include "ex7.h" */ /* To be created for this template if needed */

/* ---------------------------------------------------------------------- */
/* definitions */

#ifndef VERSION /* gcc -DVERSION="0.1.160612.142628" */
#define VERSION "20160806.231942" /**< Version Number (string) */
#endif

/* Debug */
#ifndef DEBUG /* gcc -DDEBUG=1 */
#define DEBUG 0 /**< Activate/deactivate debug mode */
#endif

#if DEBUG==0
#define NDEBUG
#endif
/* #include <assert.h> */ /* Verify assumptions with assert. Turn off with #define NDEBUG */

/** @brief Debug message if DEBUG on */
#define IFDEBUG(M) if(DEBUG) fprintf(stderr, "[DEBUG file:%s line:%d]: " M "\n", __FILE__, __LINE__); else {;}

/* limits */
#define SBUFF 256 /**< String buffer */
#define MAX_TOKENS 1000000
#define TRANSITION_PROBABILITY 0.55

/* ---------------------------------------------------------------------- */
/* structures */

typedef struct arco {
    int origem;
    int destino;
    int peso;
    struct arco *prox;
} Arco;

typedef struct lugar {
    int id;
    int tokens;
    int max_tokens;
    int min_tokens;
    long long soma_tokens;
    int n_amostras;
    struct lugar *prox;
} Lugar;

typedef struct transicao {
    int id;
    int habilitada;
    int execucoes;
    struct transicao *prox;
} Transicao;

typedef struct rede_petri {
    int n_lugares;
    int n_transicoes;
    Lugar *lugares;
    Transicao *transicoes;
    Arco *arcos_LT;
    Arco *arcos_TL;
    long long iteracoes;
    time_t inicio_simulacao;
    time_t fim_simulacao;
    long long transicoes_analisadas;
} RedePetri;

/* ---------------------------------------------------------------------- */
/* globals */

static int verb = 0; /*< verbose level, global within the file */

/* ---------------------------------------------------------------------- */
/* prototypes */

void help(void); /**< Prints help information and exit */
void copyr(void); /**< Prints copyright information and exit */
void ex7_init(void);  /**< Initializes some operations before start */
void ex7_update(void); /**< Updates values during program cycle */

RedePetri* criar_rede_petri(int lugares, int transicoes);
void adicionar_lugar(RedePetri *rede, int id, int tokens_iniciais);
void adicionar_transicao(RedePetri *rede, int id);
void adicionar_arco_LT(RedePetri *rede, int lugar, int transicao, int peso);
void adicionar_arco_TL(RedePetri *rede, int transicao, int lugar, int peso);
int transicao_habilitada(RedePetri *rede, int transicao_id);
void executar_transicao(RedePetri *rede, int transicao_id);
void simular_rede(RedePetri *rede, int max_iteracoes);
void imprimir_estatisticas(RedePetri *rede);
void liberar_rede(RedePetri *rede);
RedePetri* ler_rede_arquivo(const char *arquivo);
RedePetri* ler_rede_interativa();

#endif /* NOT def _PETRI_H */

/* ---------------------------------------------------------------------- */
/* vi: set ai et ts=4 sw=4 tw=0 wm=0 fo=croql : C config for Vim modeline */
/* Template by Dr. Beco <rcb at beco dot cc> Version 20160615.020326      */
