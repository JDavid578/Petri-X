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

TextBuffer *buffer_create(void);
void buffer_free(TextBuffer *buf);
int buffer_insert_char(TextBuffer *buf, Cursor *cursor, int c);
int buffer_delete_char(TextBuffer *buf, Cursor *cursor, bool is_help_visible);
int buffer_open_file(TextBuffer *buf, const char *filename);
int buffer_save_file(TextBuffer *buf, const char *filename);
void buffer_delete_char_right(TextBuffer *buf, Cursor *cursor, bool is_help_visible);
void buffer_delete_word_right(TextBuffer *buf, Cursor *cursor);
void buffer_delete_current_line(TextBuffer *buf, Cursor *cursor, bool is_help_visible);
void getCntrlNum(int max_y);
void cHELP();
page_st savingWindow(int max_y);
void printSTsaved(page_st saved, int start_line);
void cleanHelp(void);
void cleanWindow(int max_y);
void putSTsaved(page_st saved, TextBuffer *buf, Cursor *cursor);
int buffer_insert_line(TextBuffer *buf, int y, const char *str);
int insert_help_screen(TextBuffer *buf, Cursor *cursor);
void buffer_reset(TextBuffer *buf);
void remove_Help_screen(TextBuffer *buf, Cursor *cursor, int y_pos);
void buffer_insert_newline(TextBuffer *buf, Cursor *cursor);
int buffer_save_file_without_help(TextBuffer *buf, const char *filename, bool is_help_visible);
void buffer_delete_block(TextBuffer *buf, int start_y, int end_y, unsigned long start_x, unsigned long end_x);
#endif

/* ------------------------------------------------------------------------- */
/* vi: set ai et ts=4 sw=4 tw=0 wm=0 fo=croql : C config for Vim modeline    */
/* Template by Dr. Beco <rcb at beco dot cc>       Version 20180716.101436   */
