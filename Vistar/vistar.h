/*****************************************************************************
 *   vistar.h                                 Version 20180714.101818        *
 *                                                                           *
 *   Brief description                                                       *
 *   Copyright (C) 2016-2025    by Ruben Carlo Benante                       *
 *****************************************************************************
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 2 of the License.          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program; if not, write to the                           *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 *****************************************************************************
 *   To contact the author, please write to:                                 *
 *   Ruben Carlo Benante                                                     *
 *   Email: rcb@beco.cc                                                      *
 *   Webpage: www.beco.cc                                                    *
 *   Phone: +55 (81) 3184-7555                                               *
 *****************************************************************************/

#ifndef VISTAR_H
#define VISTAR_H

#include "libvistar.h"

// --- Constantes Globais ---
#define MAX_LINES 1000
#define MAX_COLS 1024
#define FILENAME_MAX_LEN 256
#define SEARCH_MAX 64
#define S_LINES 100
#define S_COL 256
#define HELP_LINES 9

// --- Estruturas de Dados ---

typedef struct
{
    int x;
    int y;
    int active;
} EditorMarker;

typedef struct
{
    char **lines;
    int num_lines;
    int max_lines;
    char filename[FILENAME_MAX_LEN];
    int modif;
    EditorMarker marker[10];
} TextBuffer;

typedef struct
{
    unsigned long x;
    int y;
    int top_linha;
    unsigned long mark_x;
    int mark_y;
    int select_block;
} Cursor;

typedef struct line_st
{
    char linha[S_COL];
    int quant_cols;
} line_st;

typedef struct page_st
{
    line_st page[S_LINES];
    int quant_lines;
} page_st;

typedef struct pos_st
{
    int x_pos;
    int y_pos;
} POS;

typedef struct search_st
{
    char save[MAX_COLS]; // (salva[MAX_COLS])
    POS pos[MAX_LINES * 10];  // (posicao[MAX_LINES * 10])
    int total;
    int atual;//indice da ocorrencia atual
} SEARCH;

#endif // VISTAR_H

/* ------------------------------------------------------------------------- */
/* vi: set ai et ts=4 sw=4 tw=0 wm=0 fo=croql : C config for Vim modeline    */
/* Template by Dr. Beco <rcb at beco dot cc>       Version 20180716.101436   */
