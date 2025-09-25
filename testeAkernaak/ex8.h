/*****************************************************************************
 * exN.h                                    Version 20180714.101818        *
 * *
 * Biblioteca do Akernaak                                                  *
 * Copyright (C) 2016-2018    by Ruben Carlo Benante                         *
 *****************************************************************************
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License.          *
 * *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 * GNU General Public License for more details.                            *
 * *
 * You should have received a copy of the GNU General Public License       *
 * along with this program; if not, write to the                           *
 * Free Software Foundation, Inc.,                                         *
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 *****************************************************************************/

#ifndef _EX8_H
#define _EX8_H

/* ------------------------------------------------------------------------- */
/* includes */
#include <stdio.h> /* Standard I/O functions */

/* ------------------------------------------------------------------------- */
/* definitions */

#ifndef VERSION /* gcc -DVERSION="0.1.160612.142628" */
#define VERSION "20250925.204500" /**< Version Number (string) */
#endif

/* Debug */
#ifndef DEBUG /* gcc -DDEBUG=1 */
#define DEBUG 0 /**< Activate/deactivate debug mode */
#endif

#if DEBUG==0
#define NDEBUG
#endif

/** @brief Debug message if DEBUG on */
#define IFDEBUG(M) if(DEBUG) fprintf(stderr, "[DEBUG file:%s line:%d]: " M "\n", __FILE__, __LINE__); else {;}

/* limits */
#define SBUFF 256 /**< String buffer */
#define FRASES 10 /**< Total de frases no array labmodel */

/* ------------------------------------------------------------------------- */
/* globals */

/**
 * @brief Matriz com as frases do jogo Akernaak.
 */
const char labmodel[FRASES][SBUFF] =
{
    {"-Ha' uma passagem secreta da arca para %s.\n"},
    {"-...porque os magos detestam vinho.\n"},
    {"-'akon' e' a palavra das direcoes.\n"},
    {"-...o pergaminho que esta' dentro do diamante tem o segredo.\n"},
    {"-a musica acalma as feras.\n"},
    {"(dicionario) nazg: /Næʒgi/ sm (ling. neg., duendes) 1-anel; 2-anik.\n"},
    {"-'kadon' e' a magia que compra a onisciencia.\n"},
    {"-'anik' e' a ultima magia para o aprendiz de mago...\n"},
    {"-O numero magico e': %d\n"},
    {"Titulo: Alfarrabio de Nyok.\n"}
};

/* ------------------------------------------------------------------------- */
/* prototypes */

void help(void); /**< Prints help information and exit */
void copyr(void); /**< Prints copyright information and exit */
void ex8_init(void); /**< Initializes some operations before start */
const char* sortear_frase(void); /**< Realiza o sorteio de uma frase conforme as regras */

#endif /* NOT def _EXN_H */
