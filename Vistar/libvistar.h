/*****************************************************************************
 *   libvistar.h                              Version 20180714.101818        *
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

#ifndef LIBVISTAR_H
#define LIBVISTAR_H
#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "vistar.h"
#include <stdbool.h>
// --- Declarações de Funções da Biblioteca ---

// Criação e Liberação do Buffer
TextBuffer *buffer_create(void);      // (buffer_create)
void buffer_free(TextBuffer *buf);    // (buffer_free)

// Inserção e Remoção de Caracteres
int buffer_inserir_char(TextBuffer *buf, Cursor *cursor, int c);                 // (buffer_insert_char)
int buffer_inserir_char(TextBuffer *buf, Cursor *cursor, int c);
int buffer_apagar_char(TextBuffer *buf, Cursor *cursor, bool ajuda_visivel);     // (buffer_delete_char)
void buffer_apagar_char_direita(TextBuffer *buf, Cursor *cursor, bool ajuda_visivel); // (buffer_delete_char_right)

// Operações com Palavras e Linhas
void buffer_apagar_palavra_direita(TextBuffer *buf, Cursor *cursor);     // (buffer_delete_word_right)
void buffer_apagar_linha_atual(TextBuffer *buf, Cursor *cursor, bool ajuda_visivel);
// (buffer_delete_current_line)

int buffer_inserir_linha(TextBuffer *buf, int y, const char *str);       // (buffer_insert_line)
void buffer_inserir_linhaNova(TextBuffer *buf, Cursor *cursor);         // (buffer_insert_newline)
void buffer_apagar_bloco(TextBuffer *buf, int y_inicio, int y_fim,
                         unsigned long x_inicio, unsigned long x_fim);   // (buffer_delete_block)

// Abertura e Salvamento de Arquivos
int buffer_abrir_file(TextBuffer *buf, const char *filename);         // (buffer_open_file)
int buffer_salvar_file(TextBuffer *buf, const char *filename);        // (buffer_save_file)
int buffer_salvar_sem_ajuda(TextBuffer *buf, const char *filename,
                             bool ajuda_visivel);                         // (buffer_save_file_without_help)

// Manipulação da Tela de Ajuda
int inserir_tela_ajuda(TextBuffer *buf, Cursor *cursor);                 // (insert_help_screen)
void remover_tela_ajuda(TextBuffer *buf, Cursor *cursor, int pos_y);     // (remove_Help_screen)
void limpar_ajuda(void);                                                 // (cleanHelp)

// Operações de Interface / Janela
void obter_num_control(int max_y);                                   // (getCntrlNum)
void chamar_ajuda(void);                                                 // (cHELP)
page_st janela_salvamento(int max_y);                                    // (savingWindow)
void mostrar_save(page_st saved, int linha_inicio);               // (printSTsaved)
void limpar_janela(int max_y);                                           // (cleanWindow)
void aplicar_save(page_st saved, TextBuffer *buf, Cursor *cursor); // (putSTsaved)

// Reset do Buffer
void buffer_resetar(TextBuffer *buf);                                    // (buffer_reset)

#endif

/* ------------------------------------------------------------------------- */
/* vi: set ai et ts=4 sw=4 tw=0 wm=0 fo=croql : C config for Vim modeline    */
/* Template by Dr. Beco <rcb at beco dot cc>       Version 20180716.101436   */
