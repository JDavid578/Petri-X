/*****************************************************************************
 *   vistar.c                                 Version 20180714.101818        *
 *                                                                           *
 *   Brief description                                                       *
 *   Copyright (C) 2016-2018    by Ruben Carlo Benante                       *
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

/* ------------------------------------------------------------------------- */
/* includes */

#include "libvistar.h"
#include "vistar.h"

//Define
#define CTRL(x) ((x) & 0x1F)

//Declarações de Funções (Protótipos)
static void vistar_init(void);
static void vistar_loop(void);
static void vistar_cleanup(void);
static void draw_text(void);
void cHELP(void);

//Variáveis Globais Estáticas (Estado do Editor)
static TextBuffer *buffer;
static Cursor cursor;
static bool is_help_visible = false;
static char *clipboard = NULL;
static SEARCH busca = {0};
static char last_search_term[SEARCH_MAX + 1] = "";
static WINDOW *help_win = NULL;


//(função Principal)
int main(void)
{
    vistar_init();
    vistar_loop();
    vistar_cleanup();
    return 0;
}


//Inicialização do Editor
static void vistar_init(void)
{
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(1);
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLUE, -1);
    init_pair(2, COLOR_WHITE, -1);
    init_pair(3, COLOR_RED, -1);
    init_pair(10, COLOR_MAGENTA, -1); // Azul-Roxo para a cruz
    init_pair(11, COLOR_RED, -1);     // Vermelho para os títulos
    init_pair(12, COLOR_YELLOW, -1);  // Amarelo-Laranja para os textos dos comandos
    buffer = buffer_create();
    if(!buffer)
    {
        endwin();
        fprintf(stderr, "Erro fatal: Nao foi possivel criar o buffer de texto.\n");
        exit(EXIT_FAILURE);
    }

    cursor.x = 0;
    cursor.y = 0;
    cursor.top_linha = 0;

    // Inicializa campos para marcação de bloco
    cursor.mark_x = 0;
    cursor.mark_y = 0;
    cursor.select_block = 0;
}

//Texto na tela
static void draw_text(void)
{
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int lines_on_screen = max_y - 1;

    erase();

    /*if (is_help_visible)
    {
        attron(COLOR_PAIR(12));
        mvprintw(0, 0, "[Ajuda] Ctrl+Q para sair | Ctrl+S salvar | Ctrl+O abrir | Ctrl+K comandos");
        attroff(COLOR_PAIR(12));
    }*/

    for (int i = 0; i < lines_on_screen; i++)
    {
        int buffer_line_idx = cursor.top_linha + i;
        if (buffer_line_idx >= buffer->num_lines)
            break;

        char *line = buffer->lines[buffer_line_idx];
        size_t line_len = strlen(line);
        move(is_help_visible ? i + 1 : i, 0);
        char *linha = buffer->lines[buffer_line_idx];
        int len = strlen(linha);

        move(i,0);
        clrtoeol();

        if((int)strspn(linha,"*") == len)
        {
            attron(COLOR_PAIR(3));
            mvprintw(i, 0, "%s", linha);
            attroff(COLOR_PAIR(3));
            continue;
        }
        if(linha[0] == '*')
        {
            attron(COLOR_PAIR(3));
            printw("*");
            attroff(COLOR_PAIR(3));
        }
        else
        {
            printw(" ");
        }

        int start = (linha[0] == '*') ? 1 : 0;
        int end = (linha[len-1] == '*') ? len - 1 : len;
        /* conteudo dentro das bordas */
        attron(COLOR_PAIR(2));
        printw("%.*s", end - start, linha + start);
        attroff(COLOR_PAIR(2));

        if(linha[len - 1] == '*')
        {
            attron(COLOR_PAIR(3));
            printw("*");
            attroff(COLOR_PAIR(3));
        }
        else
        {
            printw(" ");
        }
        continue;

        if (cursor.select_block)
        {
            for (unsigned long j = 0; j < line_len; j++)
            {
                bool is_selected = false;
                int sel_start_y = cursor.y < cursor.mark_y ? cursor.y : cursor.mark_y;
                int sel_end_y = cursor.y > cursor.mark_y ? cursor.y : cursor.mark_y;
                unsigned long sel_start_x = cursor.y < cursor.mark_y ? cursor.x : cursor.mark_x;
                unsigned long sel_end_x = cursor.y > cursor.mark_y ? cursor.x : cursor.mark_x;

                if ((buffer_line_idx > sel_start_y && buffer_line_idx < sel_end_y) ||
                        (buffer_line_idx == sel_start_y && j >= sel_start_x) ||
                        (buffer_line_idx == sel_end_y && j < sel_end_x))
                {
                    is_selected = true;
                }

                if (is_selected) attron(A_REVERSE);
                addch(line[j]);
                if (is_selected) attroff(A_REVERSE);
            }
        }
        else if (busca.total > 0)
        {
            int pos = 0;
            while (pos < (int)line_len)
            {
                char *found = strstr(line + pos, busca.save);
                if (found)
                {
                    int start = found - line;
                    if (start > pos)
                        printw("%.*s", start - pos, line + pos);
                    attron(COLOR_PAIR(3));
                    printw("%s", busca.save);
                    attroff(COLOR_PAIR(3));
                    pos = start + strlen(busca.save);
                }
                else
                {
                    printw("%s", line + pos);
                    break;
                }
            }
        }
        else
        {
            attron(COLOR_PAIR(2));
            printw("%s", line);
            attroff(COLOR_PAIR(2));
        }
    }

    attron(A_REVERSE);
    mvprintw(max_y - 1, 0, "Arquivo: %-20s | Linha: %d, Col: %lu %s",
            buffer->filename[0] ? buffer->filename : "[Sem Nome]",
            cursor.y + 1, cursor.x + 1, buffer->modif ? "(Modificado)" : "");
    int current_x_status = getcurx(stdscr);
    for (int i = current_x_status; i < max_x; i++)
        printw(" ");
    attroff(A_REVERSE);
    move(cursor.y - cursor.top_linha, cursor.x);
    refresh();
}

//Loop Principal do Editor
static void vistar_loop(void)
{
    int ch;
    int max_y, max_x;

    while(1)
    {
        getmaxyx(stdscr, max_y, max_x);
        int lines_on_screen = max_y - 1;
        (void)max_x;

        // --- Lógica de Scroll e Ajuste do Cursor --- <<<< MOVIDO PRA CIMA
        if(cursor.y < cursor.top_linha)
            cursor.top_linha = cursor.y;
        if(cursor.y >= cursor.top_linha + lines_on_screen)
            cursor.top_linha = cursor.y - lines_on_screen + 1;

        unsigned long current_line_len = 0;
        if(cursor.y < buffer->num_lines && buffer->lines[cursor.y] != NULL)
            current_line_len = strlen(buffer->lines[cursor.y]);
        if(cursor.x > current_line_len)
            cursor.x = current_line_len;

        // --- DESENHA A TELA ---
        draw_text();

        // --- LÊ COMANDO DO USUÁRIO ---
        ch = getch();        // 3. PROCESSA O COMANDO DE AJUDA PRIMEIRO (FUNCIONA A QUALQUER MOMENTO)

        getmaxyx(stdscr, max_y, max_x);
        (void)max_x;


        switch(ch)
        {
            // --- Comandos do Sistema ---

            // --- Teclas de Navegação ---
            case 1: // Ctrl+A (Mover para palavra anterior)
                {
                    unsigned long x = cursor.x;
                    int y = cursor.y;

                    // Se já estamos no início absoluto do arquivo, não faz nada
                    if(y == 0 && x == 0)
                        break;

                    // Se estamos no início de uma linha, pulamos para o final da linha anterior
                    if(x == 0)
                    {
                        cursor.y--;
                        cursor.x = strlen(buffer->lines[cursor.y]);
                    }
                    else
                    {
                        char *line = buffer->lines[y];
                        x--; // Começa a busca a partir do caractere à esquerda

                        // 1. Pula para trás sobre quaisquer espaços em branco
                        while(x > 0 && isspace(line[x]))
                            x--;

                        // 2. Pula para trás sobre os caracteres da palavra
                        while(x > 0 && !isspace(line[x]))
                            x--;

                        // 1  no início da palavra. Se paramos no início da linha (x=0),
                        //    não precisamos avançar.
                        if(isspace(line[x]))
                            x++;

                        cursor.x = x;
                    }
                }
                break;

            case 6: // Ctrl+F (Mover para próxima palavra)
                {
                    unsigned long x = cursor.x;
                    int y = cursor.y;

                    // Se já estamos na última linha e no final dela, não fazemos nada
                    if(y >= buffer->num_lines - 1 && x >= strlen(buffer->lines[y]))
                        break;

                    char *line = buffer->lines[y];
                    size_t len = strlen(line);

                    // Se o cursor estiver no final da linha, simplesmente o movemos para o início da próxima
                    if(x >= len)
                    {
                        cursor.y++;
                        cursor.x = 0;
                    }
                    else
                    {
                        // Lógica para encontrar o início da próxima palavra

                        // 1. Avança o 'x' de busca para o final da palavra/bloco de símbolos atual
                        if(isalnum(line[x]))    // Se estamos em uma palavra (letras/números)
                        {
                            while(x < len && isalnum(line[x]))
                                x++;
                        }
                        else     // Se estamos em símbolos ou espaços
                        {
                            while(x < len && !isalnum(line[x]))
                                x++;
                        }

                        // 2. Avança sobre quaisquer espaços em branco seguintes
                        while(x < len && isspace(line[x]))
                            x++;

                        // 3. Define a nova posição do cursor
                        if(x >= len && y < buffer->num_lines - 1)
                        {
                            // Se chegamos ao fim da linha, vamos para o início da próxima
                            cursor.y++;
                            cursor.x = 0;
                        }
                        else
                        {
                            // Se encontramos a próxima palavra na mesma linha, atualizamos o cursor.x
                            cursor.x = x;
                        }
                    }
                }
                break;
            case KEY_PPAGE: // Page Up
                {
                    // Move o cursor para cima pelo número de linhas na tela
                    cursor.y -= lines_on_screen;
                    // Garante que o cursor não passe do início do arquivo
                    if(cursor.y < 0)
                        cursor.y = 0;
                }
                break;

            case KEY_NPAGE: // Page Down
                {
                    // Move o cursor para baixo pelo número de linhas na tela
                    cursor.y += lines_on_screen;
                    // Garante que o cursor não passe do fim do arquivo
                    if(cursor.y >= buffer->num_lines)
                        cursor.y = buffer->num_lines - 1;
                }
                break;

            case KEY_LEFT:
            case 19:
                if(cursor.x > 0)
                    cursor.x--;
                else
                    if(cursor.y > 0)
                    {
                        cursor.y--;
                        cursor.x = strlen(buffer->lines[cursor.y]);
                    }
                break;
            case KEY_RIGHT:
            case 4:
                if(cursor.x < strlen(buffer->lines[cursor.y]))
                    cursor.x++;
                else
                    if(cursor.y < buffer->num_lines - 1)
                    {
                        cursor.y++;
                        cursor.x = 0;
                    }
                break;
            case KEY_UP:
            case 5:
                if(cursor.y > 0)
                {
                    cursor.y--;
                    unsigned long line_len = strlen(buffer->lines[cursor.y]);
                    if(cursor.x > line_len)
                        cursor.x = line_len;
                }
                break;
            case KEY_DOWN:
            case 24:
                if(cursor.y < buffer->num_lines - 1)
                {
                    cursor.y++;
                    unsigned long line_len = strlen(buffer->lines[cursor.y]);
                    if(cursor.x > line_len)
                        cursor.x = line_len;
                }
                break;

                // --- Teclas de Edição --- //
            case 12: // Ctrl+L (Repetir última busca)
                {
                    // 1. Verifica se já existe uma busca anterior para repetir
                    if(last_search_term[0] == '\0')
                    {
                        mvprintw(max_y - 1, 0, "Nenhuma busca anterior para repetir.");
                        clrtoeol();
                        getch();
                        break;
                    }

                    bool found = false;
                    // Converte a posição do cursor para um índice
                    // 2. Inicia a busca a partir do PRÓXIMO caractere
                    for(int y = cursor.y; y < buffer->num_lines; y++)
                    {
                        char *line = buffer->lines[y];
                        // Se estamos na linha inicial, começa a busca um caractere à frente
                        char *search_start_point = (y == cursor.y) ? (line + cursor.x + 1) : line;

                        char *found_pos = strstr(search_start_point, last_search_term);

                        if(found_pos != NULL)
                        {
                            // Palavra encontrada!
                            cursor.y = y;
                            cursor.x = (unsigned long)(found_pos - line);
                            found = true;
                            // Lógica para centralizar a tela
                            if(cursor.y < cursor.top_linha || cursor.y >= cursor.top_linha + lines_on_screen)
                            {
                                cursor.top_linha = cursor.y - (lines_on_screen / 2);
                                if(cursor.top_linha < 0) cursor.top_linha = 0;
                            }
                            break; // Sai do for loop
                        }
                    }

                    // 3. Informa o resultado ao usuário
                    if(found)
                        mvprintw(max_y - 1, 0, "Proxima ocorrencia de '%s' encontrada.", last_search_term);
                    else
                        mvprintw(max_y - 1, 0, "Nao ha mais ocorrencias de '%s'.", last_search_term);
                    clrtoeol();
                    getch();
                }
                break;

            case 25: // Ctrl+Y (Deletar linha atual)
                buffer_apagar_linha_atual(buffer, &cursor, is_help_visible);
                break;
            case 20: // Ctrl+T (Deletar palavra à direita)
                 buffer_apagar_palavra_direita(buffer, &cursor);
                break;

            case 7: // Ctrl+G (Deletar à direita)
            case KEY_DC:
                 buffer_apagar_palavra_direita(buffer, &cursor);
                break;
            case KEY_BACKSPACE:
            case 127:
            case 8:   // Ctrl+H
                buffer_apagar_char(buffer, &cursor, is_help_visible);
                break;
            case '\n':
            case '\r': // Enter
                {
                    buffer_inserir_char(buffer, &cursor, ch);
                }
                break;

                // --- Comandos com Prefixo (Ctrl+K) ---
            case 11: // Ctrl+K
                {
                    int next_cmd_char = getch();

                    switch(next_cmd_char)  // Usando next_cmd_char diretamente para tratar 'S' vs 's' se necessário
                    {
                        // --- Marcadores ---
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            {
                                int marker_index = next_cmd_char - '0';
                                buffer->marker[marker_index].y = cursor.y;
                                buffer->marker[marker_index].x = cursor.x;
                                buffer->marker[marker_index].active = 1;
                                mvprintw(max_y - 1, 0, "Marcador %d definido na linha %d, coluna %lu.",
                                        marker_index, cursor.y + 1, cursor.x + 1);
                                clrtoeol();
                                getch();
                            }
                            break;

                            // --- Comandos de Bloco ---
                        case 'B':
                        case 98:
                            {
                                cursor.mark_y = cursor.y;
                                cursor.mark_x = cursor.x;
                                cursor.select_block = 1;
                                mvprintw(max_y - 1, 0, "Início do bloco marcado.");
                                clrtoeol();
                                getch();
                            }
                            break;
                        case 'k':
                        case 'K':
                        case CTRL('K'): // Marca fim do bloco
                            {
                                if(!cursor.select_block)
                                {
                                    mvprintw(max_y - 1, 0, "Marque o início do bloco antes (Ctrl+KB).");
                                    clrtoeol();
                                    getch();
                                }
                                else
                                {
                                    mvprintw(max_y - 1, 0, "Bloco marcado. Proximo passo: Copiar (^KC) ou Recortar (^KY)");
                                    clrtoeol();
                                    getch(); // Mantemos a pausa para a mensagem ser lida
                                }
                            }
                            break;
                        case 121: // Ctrl+K Y → Recortar bloco
                        case 'Y':
                            {
                                if(!cursor.select_block)
                                {
                                    mvprintw(max_y - 1, 0, "Nenhum bloco sendo selecionado.");
                                    clrtoeol();
                                    getch();
                                    break;
                                }

                                // --- Parte 1: Copia o bloco para o clipboard (Lógica do Ctrl+KC) ---
                                int start_y, end_y;
                                unsigned long start_x, end_x;
                                // Ordena as coordenadas
                                if(cursor.y < cursor.mark_y || (cursor.y == cursor.mark_y && cursor.x < cursor.mark_x))
                                {
                                    start_y = cursor.y;
                                    start_x = cursor.x;
                                    end_y = cursor.mark_y;
                                    end_x = cursor.mark_x;
                                }
                                else
                                {
                                    start_y = cursor.mark_y;
                                    start_x = cursor.mark_x;
                                    end_y = cursor.y;
                                    end_x = cursor.x;
                                }

                                // Calcula tamanho
                                size_t required_size = 0;
                                for(int y = start_y; y <= end_y; y++)
                                {
                                    if(y >= buffer->num_lines) break;
                                    char *line = buffer->lines[y];
                                    size_t line_len = strlen(line);
                                    unsigned long line_copy_start = (y == start_y) ? start_x : 0;
                                    unsigned long line_copy_end = (y == end_y) ? end_x : line_len;
                                    if(line_copy_start > line_len) line_copy_start = line_len;
                                    if(line_copy_end > line_len) line_copy_end = line_len;
                                    if(line_copy_end > line_copy_start) required_size += (line_copy_end - line_copy_start);
                                    if(y < end_y) required_size++;
                                }

                                // Aloca memória e copia
                                free(clipboard);
                                clipboard = malloc(required_size + 1);
                                if(!clipboard) break;
                                size_t clipboard_pos = 0;
                                for(int y = start_y; y <= end_y; y++)
                                {
                                    if(y >= buffer->num_lines) break;
                                    char *line = buffer->lines[y];
                                    size_t line_len = strlen(line);
                                    unsigned long line_copy_start = (y == start_y) ? start_x : 0;
                                    unsigned long line_copy_end = (y == end_y) ? end_x : line_len;
                                    if(line_copy_start > line_len) line_copy_start = line_len;
                                    if(line_copy_end > line_len) line_copy_end = line_len;
                                    if(line_copy_end > line_copy_start)
                                    {
                                        memcpy(clipboard + clipboard_pos, line + line_copy_start, line_copy_end - line_copy_start);
                                        clipboard_pos += (line_copy_end - line_copy_start);
                                    }
                                    if(y < end_y) clipboard[clipboard_pos++] = '\n';
                                }
                                clipboard[clipboard_pos] = '\0';

                                // --- Parte 2: Deleta o bloco do buffer usando a nova função ---
                                buffer_apagar_bloco(buffer, start_y, end_y, start_x, end_x);

                                // Reposiciona o cursor no início da área deletada
                                cursor.y = start_y;
                                cursor.x = start_x;

                                // Finaliza
                                cursor.select_block = 0;
                                mvprintw(max_y - 1, 0, "Bloco recortado.");
                                clrtoeol();
                                getch();
                            }
                            break;

                        case 'c':
                        case 'C':
                        case 3: // Ctrl+C também pode ser usado
                            {
                                if(!cursor.select_block)
                                {
                                    mvprintw(max_y - 1, 0, "Nenhum bloco sendo selecionado. Use Ctrl+KB primeiro depois ctrl+KK.");
                                    clrtoeol();
                                    getch();
                                    break;
                                }

                                // Lógica de cópia
                                int start_y = (cursor.mark_y <= cursor.y) ? cursor.mark_y : cursor.y;
                                int end_y = (cursor.mark_y <= cursor.y) ? cursor.y : cursor.mark_y;
                                unsigned long start_x = (cursor.mark_y <= cursor.y) ? cursor.mark_x : cursor.x;
                                unsigned long end_x = (cursor.mark_y <= cursor.y) ? cursor.x : cursor.mark_x;
                                if(start_y == end_y && start_x > end_x)
                                {
                                    unsigned long temp = start_x;
                                    start_x = end_x;
                                    end_x = temp;
                                }

                                size_t required_size = 0;
                                for(int y = start_y; y <= end_y; y++)
                                {
                                    if(y >= buffer->num_lines) break;
                                    char *line = buffer->lines[y];
                                    size_t line_len = strlen(line);
                                    unsigned long line_copy_start = (y == start_y) ? start_x : 0;
                                    unsigned long line_copy_end = (y == end_y) ? end_x : line_len;
                                    if(line_copy_start > line_len) line_copy_start = line_len;
                                    if(line_copy_end > line_len) line_copy_end = line_len;
                                    if(line_copy_end > line_copy_start) required_size += (line_copy_end - line_copy_start);
                                    if(y < end_y) required_size++;
                                }

                                free(clipboard);
                                clipboard = malloc(required_size + 1);
                                if(!clipboard) break;

                                size_t clipboard_pos = 0;
                                for(int y = start_y; y <= end_y; y++)
                                {
                                    if(y >= buffer->num_lines) break;
                                    char *line = buffer->lines[y];
                                    size_t line_len = strlen(line);
                                    unsigned long line_copy_start = (y == start_y) ? start_x : 0;
                                    unsigned long line_copy_end = (y == end_y) ? end_x : line_len;
                                    if(line_copy_start > line_len) line_copy_start = line_len;
                                    if(line_copy_end > line_len) line_copy_end = line_len;
                                    if(line_copy_end > line_copy_start)
                                    {
                                        size_t chars_to_copy = line_copy_end - line_copy_start;
                                        memcpy(clipboard + clipboard_pos, line + line_copy_start, chars_to_copy);
                                        clipboard_pos += chars_to_copy;
                                    }
                                    if(y < end_y) clipboard[clipboard_pos++] = '\n';
                                }
                                clipboard[clipboard_pos] = '\0';

                                cursor.select_block = 0;
                                mvprintw(max_y - 1, 0, "Bloco copiado (%zu caracteres).", required_size);
                                clrtoeol();
                                getch();
                            }
                            break;

                        case 'v': //: Colar
                        case 'V':
                            {
                                // Primeiro, verifica se há algo no clipboard para colar
                                if(clipboard == NULL || clipboard[0] == '\0')
                                {
                                    mvprintw(max_y - 1, 0, "Area de transferencia esta vazia.");
                                    clrtoeol();
                                    getch();
                                    break;
                                }

                                // inserir no buffer
                                for(int i = 0; clipboard[i] != '\0'; i++)
                                {
                                    char char_para_colar = clipboard[i];

                                    if(char_para_colar == '\n')
                                    {
                                        // Se o caractere for uma nova linha, chama nossa função de ajuda
                                        buffer_inserir_linhaNova(buffer, &cursor);
                                    }
                                    else
                                    {
                                        // Para todos os outros caracteres
                                        buffer_inserir_linhaNova(buffer, &cursor);
                                    }
                                }
                                buffer->modif = 1;
                                mvprintw(max_y - 1, 0, "Bloco colado.");
                                clrtoeol();
                            }
                            break;


                            // --- Comandos de Arquivo ---
                        case 's':
                        case 'S':
                        case 19: // Salvar
                            {
                                if(buffer->filename[0] == '\0')
                                {
                                    echo();
                                    mvprintw(max_y - 1, 0, "Salvar como: ");
                                    clrtoeol();
                                    char temp_filename[FILENAME_MAX_LEN];
                                    getnstr(temp_filename, FILENAME_MAX_LEN - 1);
                                    noecho();
                                    if(strlen(temp_filename) > 0)
                                    {
                                        // Chamando a nova função, passando o estado de 'is_help_visible'
                                        if(buffer_salvar_sem_ajuda(buffer, temp_filename, is_help_visible)) mvprintw(max_y - 1, 0, "Arquivo salvo!");
                                        else mvprintw(max_y - 1, 0, "Erro ao salvar!");
                                    }
                                    else
                                        mvprintw(max_y - 1, 0, "Salvamento cancelado.");
                                }
                                else
                                {
                                    // Chamando a nova função também aqui
                                    if(buffer_salvar_sem_ajuda(buffer, buffer->filename, is_help_visible)) mvprintw(max_y - 1, 0, "Arquivo salvo!");
                                    else mvprintw(max_y - 1, 0, "Erro ao salvar!");
                                }
                                clrtoeol();
                                getch();
                            }
                            break;
                        case 'o':
                        case 'O':
                        case 15: // Abrir
                            {
                                echo();
                                mvprintw(max_y - 1, 0, "Abrir arquivo: ");
                                clrtoeol();
                                char open_filename_buf[FILENAME_MAX_LEN];
                                getnstr(open_filename_buf, FILENAME_MAX_LEN - 1);
                                noecho();
                                if(strlen(open_filename_buf) > 0)
                                {
                                    if(buffer_abrir_file(buffer, open_filename_buf))
                                    {
                                        mvprintw(max_y - 1, 0, "Arquivo carregado!");
                                        cursor.x = 0;
                                        cursor.y = 0;
                                        cursor.top_linha = 0;
                                    }
                                    else
                                        mvprintw(max_y - 1, 0, "Erro ao abrir!");
                                }
                                else
                                    mvprintw(max_y - 1, 0, "Abertura cancelada.");
                                clrtoeol();
                                getch();
                            }
                            break;

                        case 'q':
                        case 'Q':
                        case 17: // Sair
                            {
                                if(buffer->modif)
                                {
                                    mvprintw(max_y - 1, 0, "Arquivo modificado. Sair sem salvar? (s/n)");
                                    clrtoeol();
                                    int ch2 = getch();
                                    if(ch2 == 's' || ch2 == 'S') return;
                                }
                                else
                                    return;
                            }
                            break;

                        case 'n':
                        case 'N':
                        case 14: // Novo
                            {
                                mvprintw(max_y - 1, 0, "Criar novo arquivo? (S/N) ");
                                clrtoeol();
                                if(tolower(getch()) == 's')
                                {
                                    buffer_resetar(buffer);
                                    cursor.x = 0;
                                    cursor.y = 0;
                                    cursor.top_linha = 0;
                                    mvprintw(max_y - 1, 0, "Novo buffer criado.");
                                }
                                else
                                    mvprintw(max_y - 1, 0, "Operação cancelada.");
                                clrtoeol();
                                getch();
                            }
                            break;

                        default:
                            mvprintw(max_y - 1, 0, "Comando Ctrl+K %c nao reconhecido.", next_cmd_char);
                            clrtoeol();
                            getch();
                            break;
                    }
                }
                break; // Fim do case 11

                // --- Comandos com Prefixo (Ctrl+Q) ---
            case 17: // Ctrl+Q
                {
                    int next_char = getch();

                    // Primeiro, verifica se a tecla é um dígito (0-9)
                    if(next_char >= '0' && next_char <= '9')
                    {
                        int marker_index = next_char - '0';
                        if(buffer->marker[marker_index].active)
                        {
                            int target_y = buffer->marker[marker_index].y;
                            unsigned long target_x = buffer->marker[marker_index].x;

                            if(target_y < buffer->num_lines && target_x <= strlen(buffer->lines[target_y]))
                            {
                                cursor.y = target_y;
                                cursor.x = target_x;
                                mvprintw(max_y - 1, 0, "Movendo para o marcador %d.", marker_index);
                            }
                            else
                                mvprintw(max_y - 1, 0, "A posicao do marcador %d nao e mais valida.", marker_index);
                        }
                        else
                            mvprintw(max_y - 1, 0, "Marcador %d nao esta definido.", marker_index);
                        clrtoeol();
                        getch();
                    }
                    else
                    {
                        // Se não for um dígito, trata as outras letras
                        switch(tolower(next_char))
                        {
                            case 6:
                            case 'f': // Ctrl+QF (Buscar)
                                {
                                    memset(&busca, 0, sizeof(busca));
                                    echo();
                                    mvprintw(max_y - 1, 0, "Buscar por: ");
                                    clrtoeol();
                                    refresh();
                                    getnstr(busca.save, SEARCH_MAX);
                                    noecho();

                                    if(strlen(busca.save) > 0)
                                    {
                                        for(int i = 0; i < buffer->num_lines; i++)
                                        {
                                            char *line = buffer->lines[i];
                                            char *found_pos = strstr(line, busca.save);

                                            while(found_pos != NULL)
                                            {
                                                int x = (int)(found_pos - line);
                                                int y = i;

                                                /* Achou a palavra salvou a pos */
                                                busca.pos[busca.total].x_pos = x;
                                                busca.pos[busca.total].y_pos = y;
                                                busca.total++;

                                                /* movendo a busca para a proxima palavra */
                                                found_pos = strstr(found_pos + 1, busca.save);
                                            }
                                        }
                                        if(busca.total > 0)
                                        {
                                            busca.atual = 0;
                                            /* indo para a primeira ocorrencia */
                                            cursor.y = busca.pos[0].y_pos;
                                            cursor.x = busca.pos[0].x_pos;
                                            mvprintw(max_y - 1, 0, "Encontradas %d ocorrencias da palavra (%s)", busca.total, busca.save);
                                        }
                                        else
                                            mvprintw(max_y - 1, 0, "PALAVRA(%s) NAO ENCONTRADA.", busca.save);
                                    }
                                    else
                                        mvprintw(max_y - 1, 0, "BUSCA VAZIA..");
                                    clrtoeol();
                                    getch();
                                }
                                break;

                            case 19:
                            case 's': // Ctrl+QS (Início da linha)
                                cursor.x = 0;
                                break;

                            case 4:
                            case 'd': // Ctrl+QD (Fim da linha)
                                if(cursor.y < buffer->num_lines)
                                    cursor.x = strlen(buffer->lines[cursor.y]);
                                break;

                            case 18:
                            case 'r': // Ctrl+QR (Início do documento)
                                cursor.y = 0;
                                cursor.x = 0;
                                cursor.top_linha = 0;
                                break;

                            case 3:
                            case 'c': // Ctrl+QC (Fim do documento)
                                if(buffer->num_lines > 0)
                                {
                                    cursor.y = buffer->num_lines - 1;
                                    cursor.x = strlen(buffer->lines[cursor.y]);
                                }
                                break;

                            default:
                                mvprintw(max_y - 1, 0, "Comando Ctrl+Q %c nao reconhecido.", next_char);
                                clrtoeol();
                                getch();
                                break;
                        }
                    }
                }
                break; // Fim do case 17

            case 31:
            case '?':
                {
                    int help_pos = 0;
                    if(is_help_visible == 0)
                    {
                        help_pos = inserir_tela_ajuda(buffer, &cursor);
                        is_help_visible = 1;
                    }
                    else
                    {
                        remover_tela_ajuda(buffer, &cursor, help_pos);
                        is_help_visible = 0;
                        if(buffer->num_lines == 0)
                            buffer_inserir_linha(buffer, 0, "");
                    }
                }
                continue;
            case 14:
                {
                    if(busca.total == 0)
                        mvprintw(max_y - 1, 0, "Nenhuma busca ativa...");
                    else
                    {
                        busca.atual = (busca.atual + 1) % busca.total;
                        cursor.y = busca.pos[busca.atual].y_pos;
                        cursor.x = busca.pos[busca.atual].x_pos;
                        mvprintw(max_y - 1, 0, "Ocorrencia[%d] de %d. POS(%d,%d)", busca.atual + 1, busca.total, busca.pos[busca.atual].x_pos, busca.pos[busca.atual].y_pos);
                    }
                    clrtoeol();
                    getch();
                }
                break;
            case 16:
                {
                    if(busca.total == 0)
                        mvprintw(max_y - 1, 0, "Nenhuma busca ativa...");
                    else
                    {
                        busca.atual = 0;
                        cursor.y = busca.pos[0].y_pos;
                        cursor.x = busca.pos[0].x_pos;
                        mvprintw(max_y - 1, 0, "Ocorrencia[0]. POS(%d,%d)", busca.pos[0].x_pos, busca.pos[0].y_pos);
                    }
                    clrtoeol();
                    getch();
                }
                break;

                // --- Inserção de Texto ---
            default:
                if(isprint(ch))
                    buffer_inserir_char(buffer, &cursor, ch);
                break;
        }

        /* protegendo a janela de ajuda */
        if(is_help_visible && cursor.y < HELP_LINES)
        {
            cursor.y = HELP_LINES;
            cursor.x = 0;
        }
        // --- Lógica de Scroll e Ajuste do Cursor ---
        if(cursor.y < cursor.top_linha)
            cursor.top_linha = cursor.y;
        if(cursor.y >= cursor.top_linha + lines_on_screen)
            cursor.top_linha = cursor.y - lines_on_screen + 1;
        if(cursor.y < buffer->num_lines && buffer->lines[cursor.y] != NULL)
            current_line_len = strlen(buffer->lines[cursor.y]);
        if(cursor.x > current_line_len)
            cursor.x = current_line_len;
    }
}

// --- Finalização do Editor ---
static void vistar_cleanup(void)
{
    delwin(help_win);
    buffer_free(buffer);
    endwin();
    free(clipboard);
}
/* ------------------------------------------------------------------------- */
/* vi: set ai et ts=4 sw=4 tw=0 wm=0 fo=croql : C config for Vim modeline    */
/* Template by Dr. Beco <rcb at beco dot cc>       Version 20180716.101436   */
