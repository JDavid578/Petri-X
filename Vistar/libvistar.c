/*****************************************************************************
 *   libvistar.c                              Version 20180714.101818        *
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

#include "libvistar.h"
#include "vistar.h"
#include <malloc.h>
// --- Funções de Gerenciamento do Buffer ---

TextBuffer *buffer_create(void)
{
    TextBuffer *buf = malloc(sizeof(TextBuffer));
    if(!buf)
    {
        perror("libvistar: Falha ao alocar TextBuffer");
        return NULL;
    }

    buf->max_lines = MAX_LINES;
    buf->num_lines = 1;
    buf->lines = malloc(sizeof(char*) * buf->max_lines);
    if(!buf->lines)
    {
        perror("libvistar: Falha ao alocar buf->lines");
        free(buf);
        return NULL;
    }

    for(int i = 0; i < buf->max_lines; i++)
        buf->lines[i] = NULL;

    buf->lines[0] = calloc(1, 1);
    if(!buf->lines[0])
    {
        perror("libvistar: Falha ao alocar buf->lines[0]");
        free(buf->lines);
        free(buf);
        return NULL;
    }

    buf->modif = 0;
    buf->filename[0] = '\0';

    // Inicialização dos marcadores de posição
    for(int i = 0; i < 10; i++)
    {
        buf->marker[i].active = 0;
        buf->marker[i].x = 0;
        buf->marker[i].y = 0;
    }

    return buf;
}

void free_Buffer(TextBuffer *buf)
{
    if(!buf) return;

    for(int i = 0; i < buf->num_lines; i++)
    {
        free(buf->lines[i]);
        buf->lines[i] = NULL;
    }
    free(buf->lines);
    buf->lines = NULL;

    free(buf);
}

// --- Funções de Edição do Buffer ---

int buffer_inserir_char(TextBuffer *buf, Cursor *cursor, int c)
{
    if(!buf || !cursor) return 0;
    if(cursor->y >= buf->num_lines || cursor->y < 0) return 0;

    char *line = buf->lines[cursor->y];
    int current_len = strlen(line);

    if(current_len + 1 >= MAX_COLS)
        return 0;

    char *new_line = malloc(current_len + 2);
    if(!new_line)
    {
        perror("libvistar: Falha ao alocar new_line em buffer_insert_char");
        return 0;
    }

    memcpy(new_line, line, cursor->x);
    new_line[cursor->x] = (char)c;
    memcpy(new_line + cursor->x + 1, line + cursor->x, current_len - cursor->x + 1);

    free(buf->lines[cursor->y]);
    buf->lines[cursor->y] = new_line;

    cursor->x++;
    buf->modif = 1;

    return 1;
}

int buffer_apagar_char(TextBuffer *buf, Cursor *cursor, bool is_help_visible)   // Deleta à esquerda (Backspace)
{
    if(!buf || !cursor) return 0;
    if(cursor->y >= buf->num_lines || cursor->y < 0) return 0;

    char *current_line = buf->lines[cursor->y];
    int current_len = strlen(current_line);

    if(cursor->x > 0)
    {
        char *new_line = malloc(current_len);
        if(!new_line)
        {
            perror("libvistar: Falhah ao alocar new_line em buffer_delete_char (mesma linha)");
            return 0;
        }
        memcpy(new_line, current_line, cursor->x - 1);
        memcpy(new_line + cursor->x - 1, current_line + cursor->x, current_len - cursor->x + 1);

        free(buf->lines[cursor->y]);
        buf->lines[cursor->y] = new_line;
        cursor->x--;
        buf->modif = 1;
        return 1;
    }
    else
    {
        if(cursor->y == 0)
            return 0;

        char *prev_line = buf->lines[cursor->y - 1];
        int prev_len = strlen(prev_line);
        int current_line_len_for_merge = strlen(current_line);

        char *merged_line = malloc(prev_len + current_line_len_for_merge + 1);
        if(!merged_line)
        {
            perror("libvistar: Falha ao alocar merged_line em buffer_delete_char (merge de linhas)");
            return 0;
        }
        memcpy(merged_line, prev_line, prev_len);
        memcpy(merged_line + prev_len, current_line, current_line_len_for_merge + 1);

        free(buf->lines[cursor->y - 1]);
        buf->lines[cursor->y - 1] = merged_line;

        free(buf->lines[cursor->y]);

        for(int i = cursor->y; i < buf->num_lines - 1; i++)
            buf->lines[i] = buf->lines[i + 1];
        buf->lines[buf->num_lines - 1] = NULL;
        buf->num_lines--;

        cursor->y--;
        cursor->x = prev_len;
        if(is_help_visible && buf->num_lines <= HELP_LINES)
        {
            buffer_inserir_linha(buf, HELP_LINES, "");
            cursor->y = HELP_LINES;
            cursor->x = 0;
        }

        buf->modif = 1;
        return 1;
    }
}

// --- Funções de Arquivo ---

int buffer_abrir_file(TextBuffer *buf, const char *filename)
{
    if(!buf || !filename) return 0;

    FILE *f = fopen(filename, "r");
    if(!f)
        return 0;

    for(int i = 0; i < buf->num_lines; i++)
    {
        free(buf->lines[i]);
        buf->lines[i] = NULL;
    }
    buf->num_lines = 0;

    char line_buffer[MAX_COLS];
    while(fgets(line_buffer, MAX_COLS, f))
    {
        if(buf->num_lines >= buf->max_lines)
        {
            fprintf(stderr, "libvistar: Atingido o limite máximo de linhas (%d) ao abrir o arquivo.\n", buf->max_lines);
            break;
        }

        int len = strlen(line_buffer);
        if(len > 0 && line_buffer[len - 1] == '\n')
        {
            line_buffer[len - 1] = '\0';
            if(len > 1 && line_buffer[len - 2] == '\r')
                line_buffer[len - 2] = '\0';
        }

        buf->lines[buf->num_lines] = strdup(line_buffer);
        if(!buf->lines[buf->num_lines])
        {
            perror("libvistar: Falha ao strdup em buffer_open_file");
            fclose(f);
            return 0;
        }
        buf->num_lines++;
    }
    fclose(f);

    if(buf->num_lines == 0)
    {
        buf->lines[0] = calloc(1, 1);
        if(!buf->lines[0])
        {
            perror("libvistar: Falha ao alocar linha vazia após abrir arquivo");
            return 0;
        }
        buf->num_lines = 1;
    }

    strncpy(buf->filename, filename, FILENAME_MAX_LEN - 1);
    buf->filename[FILENAME_MAX_LEN - 1] = '\0';
    buf->modif = 0;
    return 1;
}

int buffer_salvar_file(TextBuffer *buf, const char *filename)
{
    if(!buf || !filename) return 0;

    FILE *f = fopen(filename, "w");
    if(!f)
        return 0;

    for(int i = 0; i < buf->num_lines; i++)
    {
        if(fprintf(f, "%s\n", buf->lines[i]) < 0)
        {
            perror("libvistar: Erro ao escrever no arquivo em buffer_save_file");
            fclose(f);
            return 0;
        }
    }
    fclose(f);

    strncpy(buf->filename, filename, FILENAME_MAX_LEN - 1);
    buf->filename[FILENAME_MAX_LEN - 1] = '\0';
    buf->modif = 0;
    return 1;
}



// --- Apagar texto a direita ---

void buffer_apagar_char_direita(TextBuffer *buf, Cursor *cursor, bool ajuda_visivel)
{
    if(!buf || !cursor || cursor->y >= buf->num_lines)
    {
        return; // Proteção contra ponteiros nulos
    }

    char *line = buf->lines[cursor->y];
    size_t len = strlen(line);

    // Caso 1: O cursor está antes do final da linha.
    if(cursor->x < len)
    {
        // Usamos memmove para deslocar toda a parte da string que está à direita
        memmove(&line[cursor->x], &line[cursor->x + 1], len - cursor->x);

        // O terminador nulo '\0' também é movido, então a string é encurtada.
        buf->modif = 1;
    }
    // Caso 2: O cursor está EXATAMENTE no final da linha
    else
        if(cursor->x == len && cursor->y < buf->num_lines - 1)
        {

            // Movemos o cursor para o início da próxima linha.
            cursor->y++;
            cursor->x = 0;
            buffer_apagar_char(buf, cursor, ajuda_visivel);
        }
}



// --- deletar palavra a direita --- //

void buffer_apagar_palavra_direita(TextBuffer *buf, Cursor *cursor)
{
    if(!buf || !cursor || cursor->y >= buf->num_lines)
    {
        return; // Validação
    }

    char *line = buf->lines[cursor->y];
    size_t len = strlen(line);
    unsigned long original_x = cursor->x; // Posição inicial do cursor

    if(original_x >= len)
    {
        return; // Cursor já está no final
    }

    unsigned long idx_apos_delecao = original_x;

    // Avança sobre quaisquer espaços em branco a partir da posição do cursor
    while(idx_apos_delecao < len && isspace(line[idx_apos_delecao]))
        idx_apos_delecao++;

    while(idx_apos_delecao < len && !isspace(line[idx_apos_delecao]))
        idx_apos_delecao++;

    if(idx_apos_delecao > original_x)
    {
        // Desloca a parte restante da linha (incluindo o terminador nulo)
        // para a posição original do cursor.
        memmove(&line[original_x], &line[idx_apos_delecao], len - idx_apos_delecao + 1);
        buf->modif = 1;
    }
}


// --- deletar a linha toda --- //

void buffer_apagar_linha_atual(TextBuffer *buf, Cursor *cursor, bool is_help_visible)
{
    if(!buf || !cursor || buf->num_lines == 0 || cursor->y >= buf->num_lines)
    {
        return; // Não há o que deletar
    }

    // 1. Libera a memória da linha que está sendo deletada
    free(buf->lines[cursor->y]);
    buf->lines[cursor->y] = NULL; // Boa prática

    // 2. Se não for a última linha do buffer
    if(cursor->y < buf->num_lines - 1)
    {
        memmove(&buf->lines[cursor->y],
                &buf->lines[cursor->y + 1],
                (buf->num_lines - 1 - cursor->y) * sizeof(char*));
    }

    // 3. Decrementa o número de linhas
    buf->num_lines--;

    if(buf->num_lines > 0)    // Se ainda restam linhas
    {
        buf->lines[buf->num_lines] = NULL; // O antigo último slot
    }


    // 4. Se o buffer ficou completamente vazio (nlines se tornou 0)
    if(buf->num_lines == 0)
    {
        buf->lines[0] = calloc(1, 1); // Adiciona uma nova linha vazia
        if(!buf->lines[0])
        {
            perror("libvistar: buffer_delete_current_line: falha ao alocar linha vazia");
            return;
        }
        buf->num_lines = 1; //Agora temos uma linha novamente
        cursor->y = 0;   // Posiciona o cursor na nova linha vazia
    }
    else
    {
        // 5. Ajusta a posição Y do cursor se ele estava na última linha (que foi removida)
        if(cursor->y >= buf->num_lines)
            cursor->y = buf->num_lines - 1;
    }

    // 6. Ajusta a posição X do cursor para não ficar fora da linha atual (que pode ter mudado)
    if(cursor->y < buf->num_lines && buf->lines[cursor->y] != NULL)
    {
        unsigned long current_line_len = strlen(buf->lines[cursor->y]);
        if(cursor->x > current_line_len)
            cursor->x = current_line_len;
    }
    else
        cursor->x = 0;

    if(is_help_visible && buf->num_lines <= HELP_LINES)
    {
        buffer_inserir_linha(buf, HELP_LINES, "");
        cursor->y = HELP_LINES;
        cursor->x = 0;
    }

    buf->modif = 1; // Marca o buffer como modificado
}

// --- Funções de Auxilio ---
void getCntrlNum(int max_y)
{
    int ch;

    mvprintw(max_y - 2, 0, "Pressione uma tecla...");
    clrtoeol();
    refresh();

    ch = getch();

    if(ch >= 1 && ch <= 26 && ch != '\n' && ch != '\t')
    {
        mvprintw(max_y - 2, 0, "Control Key detected (%d). Proxima tecla: ", ch);
        clrtoeol();
        refresh();
        int next_char = getch();
        mvprintw(max_y - 2, 0, "Control Key detected. Proxima tecla numeracao: %d ", next_char);
        clrtoeol();
        refresh();
        getch();
        mvprintw(max_y - 2, 0, " ");
        clrtoeol();
        refresh();
    }
}
void cHELP()
{
    page_st cHELP;
    cHELP.quant_lines = 0;
    //mvprintf como linhas, cada um sera uma linha
    const char bussula[5][15] =
    {
        {"     (^E)     \n"},
        {"      |      \n"},
        {"(^S)--o--(^D)\n"},
        {"      |      \n"},
        {"     (^X)    \n"},
    };
    strcpy(cHELP.page[0].linha, "Shortcuts \n");
    mvprintw(0, 0, "Shortcuts: ");
    for(int i = 0; i < 5; i++)
        mvprintw(i + 2, 0, "%s", bussula[i]);
    mvprintw(1, 17, "|    MOVE TO     | -> [Next;Previous]WORD == [^F;^A] [Begining;End]LINE = [^QS;^QD]  [Begining;End]FILE = [^QR;^QC]");
    mvprintw(2, 17, "|EDITION CONTROLS| -> DELETE[ Current Line; Right Word; Right Char; Left Char ] == [^Y;^T;^G;^H]");
    mvprintw(3, 17, "|EDITION CONTROLS| -> SEARCH[Word] == [^QF] [Define;Go To]MARKER == [^K(0-9);^Q(0-9)]");
    mvprintw(4, 17, "| BLOCK CONTROLS | -> MarkBlock[Begin;End] == [^KB;^KK]  [Copy;Paste;Cut]MarkedBlock == [^KC;^KV;^KY]");
    mvprintw(5, 17, "| FILE OPERATORS | -> [Save; Open; Create; Exit] = [^KS; ^KO; ^KN; ^KQ]");
    mvprintw(6, 17, "|      HELP      | -> ^?");

}

page_st savingWindow(int max_y)
{
    page_st saved;
    saved.quant_lines = 0;
    int chars_read = 0;

    if(max_y > S_LINES)
        max_y = S_LINES;

    /*mvprintw(max_y-1, 0, "chars_read == %d",chars_read);
      clrtoeol();
      refresh();
      getch();*/
    for(int y = 0; y < max_y - 1; y++)
    {
        memset(saved.page[y].linha, 0, S_COL);
        chars_read = mvinnstr(y, 0, saved.page[y].linha, S_COL - 1);
        if(chars_read != ERR)
        {
            //colocando o null terminator
            saved.page[y].linha[chars_read] = '\0';
            //removendo os espacos em branco valeu chat gpt kkk n tava indo de jeito nenhum
            int last_non_space = chars_read - 1;
            while(last_non_space >= 0 && saved.page[y].linha[last_non_space] == ' ')
                last_non_space--;
            saved.page[y].linha[last_non_space + 1] = '\0';
            saved.page[y].quant_cols = last_non_space + 1;
        }
        else
        {
            saved.page[y].linha[0] = '\0';
            saved.page[y].quant_cols = 0;
        }
        /*mvprintw(max_y-1, 0, "linha[%d] (%d chars): '%.50s...'",y,chars_read,saved.page[y].linha);
          refresh();
          getch();*/
        saved.quant_lines++;
    }
    /*mvprintw(max_y-1, 0, "chars_read == %d",chars_read);
      clrtoeol();
      refresh();
      getch();*/

    return saved;
}

void printSTsaved(page_st saved, int start_line)
{
    int y, x, ch, begin;

    for(y = start_line, begin = 0; y < saved.quant_lines && begin < saved.quant_lines; y++, begin++)
    {
        for(x = 0; x < saved.page[y].quant_cols; x++)
        {
            ch = (int)saved.page[begin].linha[x];
            mvaddch(y, x, ch);
        }
    }
    refresh();
}
void cleanHelp(void)
{
    mvprintw(1, 0, " ");
    mvprintw(2, 0, " ");
    mvprintw(3, 0, " ");
    mvprintw(4, 0, " ");
    mvprintw(5, 0, " ");
    mvprintw(6, 0, " ");
}

void cleanWindow(int max_y)
{
    for(int y = 0; y < max_y - 1; y++)
        mvprintw(y, 0, " ");
}
void putSTsaved(page_st saved, TextBuffer *buf, Cursor *cursor)
{
    int y, x, ch;
    for(y = 0; y < saved.quant_lines ; y++)
    {
        for(x = 0; x < saved.page[y].quant_cols; x++)
        {
            ch = (int)saved.page[y].linha[x];
            buffer_inserir_char(buf, cursor, ch);
        }
        buffer_inserir_char(buf, cursor, '\n');
    }
}

int buffer_inserir_linha(TextBuffer *buf, int y, const char *str)
{
    if(!buf || !str || y < 0 || y > buf->num_lines || buf->num_lines >=  buf->max_lines)
        return 0;
    int len = strlen(str);

    if(len >= MAX_COLS)
        len = MAX_COLS - 1;

    //aloca nova linha
    char *new_line = malloc(MAX_COLS);

    if(!new_line)
        return 0;
    strncpy(new_line, str, len);
    new_line[len] = '\0';

    //desloca linhas para baixo
    for(int i = buf->num_lines; i > y; i--)
        buf->lines[i] = buf->lines[i - 1];
    //insere nova linha
    buf->lines[y] = new_line;
    buf->num_lines++;
    buf->modif = 1;

    return 1;
}
int insert_help_screen(TextBuffer *buf, Cursor *cursor)
{
    if(!buf || !cursor)
        return -1;
    int saved_x = cursor->x;
    int saved_y = cursor->y + HELP_LINES;

    int base_y = 0;
    buffer_inserir_linha(buf, base_y++, "*****************************************************************************************************************************************************");
    buffer_inserir_linha(buf, base_y++, "*                  ------------------    ---------------------------------------------------------------------------------------------------------- *");
    buffer_inserir_linha(buf, base_y++, "*         (^E)     |    MOVE TO     | -> | ^A <Word   ^F >Word   ^QS Bol    ^QD Eol    ^QR Top   ^QC Bottom  PGUP Scroll-up   PGDN Scroll-down    | *");
    buffer_inserir_linha(buf, base_y++, "*          |       |EDITION CONTROLS| -> | ^G Del char   ^H Backspace   ^T Del word   ^Y Del line   ^QF Search   ^K0-9 Set mark   ^Q0-9 Goto mark | *");
    buffer_inserir_linha(buf, base_y++, "*    (^S)--o--(^D) | BLOCK CONTROLS | -> | ^KB Begin  ^KK End  ^KC Copy  ^KV Paste  ^KY Cut                                                       | *");
    buffer_inserir_linha(buf, base_y++, "*          |       | FILE OPERATORS | -> | ^KS Save   ^KO Open   ^KN New    ^KQ Exit                                                              | *");
    buffer_inserir_linha(buf, base_y++, "*         (^X)     |      HELP      | -> | ^? Toggle Help                                                                                         | *");
    buffer_inserir_linha(buf, base_y++, "*                  ------------------    ---------------------------------------------------------------------------------------------------------- *");
    buffer_inserir_linha(buf, base_y++, "*****************************************************************************************************************************************************");


    cursor->x = saved_x;
    cursor->y = saved_y;

    return 0;
}
//--------------------------------------------------------------
/**
 * @brief Salva o conteúdo do buffer em um arquivo, ignorando as linhas de ajuda se estiverem visíveis.
 *
 * @param buf O buffer de texto.
 * @param filename O nome do arquivo para salvar.
 * @param is_help_visible Um booleano que indica se a ajuda está atualmente no buffer.
 * @return int 1 em caso de sucesso, 0 em caso de falha.
 */
int buffer_save_file_without_help(TextBuffer *buf, const char *filename, bool is_help_visible)
{
    if(!buf || !filename) return 0;

    FILE *f = fopen(filename, "w");
    if(!f)
    {
        perror("libvistar: Falha ao abrir arquivo para escrita");
        return 0;
    }

    // --- A LÓGICA PRINCIPAL ESTÁ AQUI ---
    // Determina a partir de qual linha devemos começar a salvar.
    // Se a ajuda estiver visível, pulamos as primeiras HELP_LINES.
    int start_line = 0;
    if(is_help_visible)
        start_line = HELP_LINES;

    // Itera a partir da 'start_line' em vez de 0.
    for(int i = start_line; i < buf->num_lines; i++)
    {
        if(fprintf(f, "%s\n", buf->lines[i]) < 0)
        {
            perror("libvistar: Erro ao escrever no arquivo");
            fclose(f);
            return 0;
        }
    }

    fclose(f);

    // Atualiza o estado do buffer para refletir que foi salvo.
    strncpy(buf->filename, filename, FILENAME_MAX_LEN - 1);
    buf->filename[FILENAME_MAX_LEN - 1] = '\0';
    buf->modif = 0; // Opcional: considerar se salvar com ajuda ainda conta como "salvo"

    return 1;
}
//----------------------------------
void buffer_resetar(TextBuffer *buf)
{
    if(!buf) return;
    for(int i = 0; i < buf->num_lines; i++)
    {
        if(buf->lines[i] != NULL)
        {
            free(buf->lines[i]);
            buf->lines[i] = NULL;
        }
    }
    buf->num_lines = 1;
    buf->lines[0] = calloc(1, 1);
    if(!buf->lines[0])
    {
        perror("libvistar: Falha ao alocar linha em buffer_reset");
        buf->num_lines = 0;
        return;
    }
    buf->modif = 0;
    buf->filename[0] = '\0';
    for(int i = 0; i < 10; i++)
        buf->marker[i].active = 0;
}
void remover_tela_ajuda(TextBuffer *buf, Cursor *cursor, int y_pos)
{
    if(!buf || !cursor)
        return;

    if(y_pos < 0 || y_pos >= buf->num_lines)
        return;
    int saved_x = cursor->x;
    int saved_y = cursor->y - HELP_LINES;

    for(int i = 0; i < HELP_LINES; i++)
    {
        int target_line = y_pos;
        if(target_line >= buf->num_lines)
            break;
        free(buf->lines[target_line]);
        for(int j = target_line; j < buf->num_lines; j++)
            buf->lines[j] = buf->lines[j + 1];
        buf->lines[buf->num_lines - 1] = NULL;
        buf->num_lines--;
    }
    if(cursor->y >= y_pos)
    {
        if(cursor->y >= HELP_LINES)
            cursor->y -= HELP_LINES;
        else
            cursor->y = 0;
        cursor->x = 0;
    }
    buf->modif = 1;
    cursor->x = saved_x;
    cursor->y = saved_y;
}

// Adiciona uma nova linha no buffer na posição do cursor
void buffer_inserir_linhaNova(TextBuffer *buf, Cursor *cursor)
{
    if(!buf || !cursor || cursor->y >= buf->num_lines) return;

    // Se o buffer está cheio, não faz nada.
    if(buf->num_lines >= buf->max_lines) return;

    char *current_line = buf->lines[cursor->y];

    // Pega o resto da linha a partir do cursor
    char *rest_of_line = strdup(current_line + cursor->x);
    if(!rest_of_line) return;  // Falha de memória

    // Tranca a linha atual na posição do cursor
    current_line[cursor->x] = '\0';
    // Opcional: realocar a linha atual para economizar memória
    char *shrunk_line = strdup(current_line);
    if(shrunk_line)
    {
        free(buf->lines[cursor->y]);
        buf->lines[cursor->y] = shrunk_line;
    }

    // Move as linhas abaixo para abrir espaço
    for(int i = buf->num_lines; i > cursor->y + 1; i--)
        buf->lines[i] = buf->lines[i - 1];

    // Insere a nova linha
    buf->lines[cursor->y + 1] = rest_of_line;
    buf->num_lines++;

    // Atualiza a posição do cursor
    cursor->y++;
    cursor->x = 0;
    buf->modif = 1;
}

void buffer_apagar_bloco(TextBuffer *buf, int start_y, int end_y, unsigned long start_x, unsigned long end_x)
{
    if(start_y == end_y)
    {
        //a seleção está em uma única linha
        char *line = buf->lines[start_y];
        memmove(&line[start_x], &line[end_x], strlen(line) - end_x + 1);
    }
    else
    {
        //seleção em múltiplas linhas
        char *start_line = buf->lines[start_y];
        char *end_line = buf->lines[end_y];

        // 1. Tranca o final da linha inicial
        start_line[start_x] = '\0';

        // 2. Pega o final da linha de término
        char *end_line_suffix = strdup(end_line + end_x);

        // 3.início da primeira linha com o final da última linha
        size_t new_len = strlen(start_line) + strlen(end_line_suffix);
        char *merged_line = realloc(start_line, new_len + 1);
        if(merged_line)
        {
            strcat(merged_line, end_line_suffix);
            buf->lines[start_y] = merged_line;
        }
        free(end_line_suffix);

        // 4. Calcula quantas linhas do meio serão deletadas
        int lines_to_delete_count = end_y - start_y;

        // 5. Libera a memória das linhas
        for(int i = 1; i <= lines_to_delete_count; i++)
            free(buf->lines[start_y + i]);

        // 6. Desloca as linhas restantes para cima
        if(end_y + 1 < buf->num_lines)
        {
            memmove(&buf->lines[start_y + 1],
                    &buf->lines[end_y + 1],
                    (buf->num_lines - (end_y + 1)) * sizeof(char*));
        }

        buf->num_lines -= lines_to_delete_count;
    }
    buf->modif = 1;
}
/* ------------------------------------------------------------------------- */
/* vi: set ai et ts=4 sw=4 tw=0 wm=0 fo=croql : C config for Vim modeline    */
/* Template by Dr. Beco <rcb at beco dot cc>       Version 20180716.101436   */
