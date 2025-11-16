#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ncurses.h>

#define MAX_ARQ 256
#define MAX_LINHA 512

char **listar_arquivos(int *count) {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (!d) return NULL;

    char **lista = malloc(sizeof(char*) * MAX_ARQ);
    int i = 0;

    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || 
            strcmp(dir->d_name, "..") == 0)
            continue;

        if (dir->d_type == DT_REG) {
            lista[i] = strdup(dir->d_name);
            i++;
        }
    }

    closedir(d);
    *count = i;
    return lista;
}

void mostrar_arquivo(const char *nome) {
    clear();
    FILE *f = fopen(nome, "r");

    if (!f) {
        mvprintw(0, 0, "Erro abrindo arquivo!");
        return;
    }

    char linha[MAX_LINHA];
    int y = 0;

    mvprintw(y++, 0, "Arquivo: %s", nome);
    mvprintw(y++, 0, "----------------------------------------");

    while (fgets(linha, sizeof(linha), f) != NULL && y < LINES-1) {
        mvprintw(y++, 0, "%s", linha);
    }

    fclose(f);

    mvprintw(LINES-2, 0, "[Q] Voltar");
    refresh();

    int ch;
    while ((ch = getch())) {
        if (ch == 'q' || ch == 'Q')
            break;
    }
}

int menu_arquivos(char **arquivos, int total) {
    int highlight = 0;
    int c;

    keypad(stdscr, TRUE);

    while (1) {
        clear();
        mvprintw(0, 0, "Selecione um arquivo e pressione ENTER:");
        mvprintw(1, 0, "(Use ↑ ↓ para navegar, Q para sair)");

        for (int i = 0; i < total; i++) {
            if (i == highlight)
                attron(A_REVERSE);

            mvprintw(i + 3, 0, "%s", arquivos[i]);

            if (i == highlight)
                attroff(A_REVERSE);
        }

        c = getch();

        switch (c) {
            case KEY_UP:
                highlight--;
                if (highlight < 0) highlight = total - 1;
                break;

            case KEY_DOWN:
                highlight++;
                if (highlight >= total) highlight = 0;
                break;

            case 10: // ENTER
                return highlight;

            case 'q':
            case 'Q':
                return -1;
        }
    }
}

int main() {
    int total;
    char **arquivos = listar_arquivos(&total);

    if (total == 0) {
        printf("Nenhum arquivo encontrado na pasta.\n");
        return 0;
    }

    initscr();
    noecho();
    curs_set(0);

    while (1) {
        int opcao = menu_arquivos(arquivos, total);
        if (opcao == -1)
            break;

        mostrar_arquivo(arquivos[opcao]);
    }

    endwin();
    return 0;
}
