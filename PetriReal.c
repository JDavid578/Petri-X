/***************************************************************************
 *   petri.c                                    Version 20160806.231815      *
 *                                                                         *
 *   Petri Net Simulator                                                   *
 *   Copyright (C) 2016         by Ruben Carlo Benante                     *
 ***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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
 *   Webpage: http://www.beco.cc                                           *
 *   Phone: +55 (81) 3184-7555                                             *
 ***************************************************************************/

/* ---------------------------------------------------------------------- */
/**
 * @file petri.c
 * @ingroup GroupUnique
 * @brief Petri Net Simulator
 * @details This program really do a nice job as a template, and template only!
 * @version 20160806.231815
 * @date 2016-08-06
 * @author Ruben Carlo Benante <<rcb@beco.cc>>
 * @par Webpage
 * <<a href="http://www.beco.cc">www.beco.cc</a>>
 * @copyright (c) 2016 GNU GPL v2
 * @note This program is free software: you can redistribute it
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
 * @todo Now that you have the template, hands on! Programme!
 * @warning Be carefull not to lose your mind in small things.
 * @bug This file right now does nothing usefull
 *
 */

/*
 * Instrucoes para compilar:
 *   $gcc petri.c -o ex11.x -Wall
 *        -Wextra -ansi -pedantic-errors -g -O0 -DDEBUG=1 -DVERSION="0.1.160612.142044"
 *   ou preferencialmente inclua um makefile e use:
 *   $make
 *
 * Modelo de indentacao:
 * Estilo: --style=allman ou -A1
 *
 * Opcoes: -A1 -s4 -k3 -xj -SCNeUpv
 *
 *  * No vi use:
 *      :% !astyle -A1 -s4 -k3 -xj -SCNeUpv
 *  * No linux:
 *      $astlye -A1 -s4 -k3 -xj -SCNeUpv petri.c
 */

/* ---------------------------------------------------------------------- */
/* includes */

#include <stdio.h> /* Standard I/O functions */
#include <stdlib.h> /* Miscellaneous functions (rand, malloc, srand)*/
#include <getopt.h> /* get options from system argc/argv */
#include "PetriReal.h" /* To be created for this template if needed */

/* #include <time.h> */ /* Time and date functions */
/* #include <math.h> */ /* Mathematics functions */
/* #include <string.h> */ /* Strings functions definitions */
/* #include <dlfcn.h> */ /* Dynamic library */
/* #include <malloc.h> */ /* Dynamic memory allocation */
/* #include <unistd.h> */ /* UNIX standard function */
/* #include <limits.h> */ /* Various C limits */
/* #include <ctype.h> */ /* Character functions */
/* #include <errno.h> */ /* Error number codes errno */
/* #include <signal.h> */ /* Signal processing */
/* #include <stdarg.h> */ /* Functions with variable arguments */
/* #include <pthread.h> */ /* Parallel programming with threads */
/* #include <fcntl.h> */ /* File control definitions */
/* #include <termios.h> */ /* Terminal I/O definitions POSIX */
/* #include <sys/stat.h> */ /* File status and information */
/* #include <float.h> */ /* Float constants to help portability */
/* #include <setjmp.h> */ /* Bypass standard function calls and return */
/* #include <stddef.h> */ /* Various types and MACROS */
/* #include <SWI-Prolog.h> */ /* Prolog integration with C */
/* #include <ncurses.h> */ /* Screen handling and optimisation functions */
/* #include <allegro.h> */ /* A game library for graphics, sounds, etc. */
/* #include <libintl.h> */ /* Internationalization / translation */
/* #include <locale.h> */ /* MACROS LC_ for location specific settings */
/* #include "libeco.h" */ /* I/O, Math, Sound, Color, Portable Linux/Windows */

/* ---------------------------------------------------------------------- */
/* definitions */

#ifndef VERSION /* gcc -DVERSION="0.1.160612.142628" */
#define VERSION "20160806.231815" /**< Version Number (string) */
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

/* ---------------------------------------------------------------------- */
/* globals */

/*static int verb = 0;*/ /* < verbose level, global within the file */

/* ---------------------------------------------------------------------- */
/* prototypes */

void help(void); /* print some help */
void copyr(void); /* print version and copyright information */
void ex7_init(void); /* global initialization function */

/* ---------------------------------------------------------------------- */
/**
 * @ingroup GroupUnique
 * @brief This is the main event of the evening
 * @details Ladies and Gentleman... It's tiiiime!
 * Fightiiiiing at the blue corner,
 * he, who has compiled more C code than any other adversary in the history,
 * he, who has developed UNIX and Linux, and is an inspiration to maaany languages
 * and compilers, the GNU C Compiler, GCC!
 * Fightiiiiing at the red corner, the challenger, in his first fight, lacking of any
 * valid experience but angrily, blindly, and no doubtedly, will try to
 * compile this program without errors. He, the student, the apprentice,
 * the developer, beco!!
 *
 * @param[in] argc Argument counter
 * @param[in] argv Argument strings (argument values)
 *
 * @retval 0 If succeed (EXIT_SUCCESS).
 * @retval 1 Or another error code if failed.
 *
 * @par Example
 * @code
 *    $./ex11.x -h
 *    $./ex11.x -f petrinet1.txt
 *    $./ex11.x < petrinet1.txt
 * @endcode
 *
 * @warning   Be carefull with...
 * @bug There is a bug with...
 * @todo Need to do...
 * @note You can read more about it at <<a href="http://www.beco.cc">www.beco.cc</a>>
 * @author Ruben Carlo Benante
 * @version 20160806.231815
 * @date 2016-08-06
 *
 */
int main(int argc, char *argv[])
{
    int opt; /* return from getopt() */
    char *arquivo = NULL; /* arquivo de entrada */
    RedePetri *rede = NULL;
    int max_iteracoes = 1000;

    IFDEBUG("Starting optarg loop...");

    /* getopt() configured options:
     *        -h  help
     *        -V  version
     *        -v  verbose
     *        -f  file
     */
    opterr = 0;
    while((opt = getopt(argc, argv, "vhVf:")) != EOF)
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
            case 'f':
                arquivo = optarg;
                break;
            case '?':
            default:
                printf("Type\n\t$man %s\nor\n\t$%s -h\nfor help.\n\n", argv[0], argv[0]);
                return EXIT_FAILURE;
        }

    if(verb)
        printf("Verbose level set at: %d\n", verb);

    ex7_init(); /* initialization function */

    /* ...and we are done */
    /* Write your code here... */

    if(arquivo != NULL)
    {
        rede = ler_rede_arquivo(arquivo);
    }
    else
    {
        rede = ler_rede_interativa();
    }

    if(!rede)
    {
        printf("Erro ao criar rede\n");
        return EXIT_FAILURE;
    }

    simular_rede(rede, max_iteracoes);
    imprimir_estatisticas(rede);
    liberar_rede(rede);

    return EXIT_SUCCESS;
}

/* Write your functions here... */

RedePetri* criar_rede_petri(int lugares, int transicoes)
{
    RedePetri *rede = (RedePetri*)malloc(sizeof(RedePetri));
    if (!rede) return NULL;

    rede->num_lugares = lugares;
    rede->num_transicoes = transicoes;
    rede->lugares = NULL;
    rede->transicoes = NULL;
    rede->arcos_LT = NULL;
    rede->arcos_TL = NULL;
    rede->iteracoes = 0;
    rede->transicoes_analisadas = 0;

    return rede;
}

void adicionar_lugar(RedePetri *rede, int id, int tokens_iniciais)
{
    Lugar *novo = (Lugar*)malloc(sizeof(Lugar));
    if (!novo) return;

    novo->id = id;
    novo->tokens = tokens_iniciais;
    novo->max_tokens = tokens_iniciais;
    novo->min_tokens = tokens_iniciais;
    novo->soma_tokens = tokens_iniciais;
    novo->num_amostras = 1;
    novo->prox = rede->lugares;
    rede->lugares = novo;
}

void adicionar_transicao(RedePetri *rede, int id)
{
    Transicao *nova = (Transicao*)malloc(sizeof(Transicao));
    if (!nova) return;

    nova->id = id;
    nova->habilitada = 0;
    nova->execucoes = 0;
    nova->prox = rede->transicoes;
    rede->transicoes = nova;
}

void adicionar_arco_LT(RedePetri *rede, int lugar, int transicao, int peso)
{
    Arco *novo = (Arco*)malloc(sizeof(Arco));
    if (!novo) return;

    novo->origem = lugar;
    novo->destino = transicao;
    novo->peso = peso;
    novo->prox = rede->arcos_LT;
    rede->arcos_LT = novo;
}

void adicionar_arco_TL(RedePetri *rede, int transicao, int lugar, int peso)
{
    Arco *novo = (Arco*)malloc(sizeof(Arco));
    if (!novo) return;

    novo->origem = transicao;
    novo->destino = lugar;
    novo->peso = peso;
    novo->prox = rede->arcos_TL;
    rede->arcos_TL = novo;
}

Lugar* encontrar_lugar(RedePetri *rede, int id)
{
    Lugar *atual = rede->lugares;
    while (atual)
    {
        if (atual->id == id) return atual;
        atual = atual->prox;
    }
    return NULL;
}

Transicao* encontrar_transicao(RedePetri *rede, int id)
{
    Transicao *atual = rede->transicoes;
    while (atual)
    {
        if (atual->id == id) return atual;
        atual = atual->prox;
    }
    return NULL;
}

int transicao_habilitada(RedePetri *rede, int transicao_id)
{
    Arco *arco = rede->arcos_LT;

    while (arco)
    {
        if (arco->destino == transicao_id)
        {
            Lugar *lugar = encontrar_lugar(rede, arco->origem);
            if (!lugar || lugar->tokens < arco->peso)
            {
                return 0;
            }
        }
        arco = arco->prox;
    }
    return 1;
}

void executar_transicao(RedePetri *rede, int transicao_id)
{
    Arco *arco;
    Lugar *lugar;
    Transicao *trans = encontrar_transicao(rede, transicao_id);

    if (!trans) return;

    /* Consumir tokens */
    arco = rede->arcos_LT;
    while (arco)
    {
        if (arco->destino == transicao_id)
        {
            lugar = encontrar_lugar(rede, arco->origem);
            if (lugar)
            {
                lugar->tokens -= arco->peso;
                if (lugar->tokens < lugar->min_tokens)
                {
                    lugar->min_tokens = lugar->tokens;
                }
                lugar->soma_tokens += lugar->tokens;
                lugar->num_amostras++;
            }
        }
        arco = arco->prox;
    }

    /* Produzir tokens */
    arco = rede->arcos_TL;

    while (arco)
    {
        if (arco->origem == transicao_id)
        {
            lugar = encontrar_lugar(rede, arco->destino);
            if (lugar)
            {
                lugar->tokens += arco->peso;
                if (lugar->tokens > lugar->max_tokens)
                {
                    lugar->max_tokens = lugar->tokens;
                }
                lugar->soma_tokens += lugar->tokens;
                lugar->num_amostras++;
            }
        }
        arco = arco->prox;
    }

    trans->execucoes++;
}

void simular_rede(RedePetri *rede, int max_iteracoes)
{
    srand(time(NULL));
    rede->inicio_simulacao = time(NULL);

    printf("Iniciando simulação...\n");

    for (int i = 0; i < max_iteracoes; i++)
    {
        rede->iteracoes++;
        int alguma_executou = 0;

        Transicao *trans = rede->transicoes;
        while (trans)
        {
            rede->transicoes_analisadas++;

            if (transicao_habilitada(rede, trans->id))
            {
                if ((rand() % 100) < (TRANSITION_PROBABILITY * 100))
                {
                    executar_transicao(rede, trans->id);
                    alguma_executou = 1;
                }
            }
            trans = trans->prox;
        }

        if (!alguma_executou)
        {
            printf("Deadlock na iteração %d\n", i+1);
            break;
        }

        /* Verificar overflow */
        Lugar *lugar = rede->lugares;
        while (lugar)
        {
            if (lugar->tokens > MAX_TOKENS)
            {
                printf("Overflow no lugar L%d\n", lugar->id);
                return;
            }
            lugar = lugar->prox;
        }
    }

    rede->fim_simulacao = time(NULL);
}

void imprimir_estatisticas(RedePetri *rede)
{
    double duracao = difftime(rede->fim_simulacao, rede->inicio_simulacao);

    printf("\n=== ESTATÍSTICAS ===\n");
    printf("Iterações: %lld\n", rede->iteracoes);
    printf("Duração: %.2f segundos\n", duracao);

    if (duracao > 0)
    {
        printf("Velocidade: %.2f iterações/seg\n", rede->iteracoes / duracao);
        printf("Transições/seg: %.2f\n", rede->transicoes_analisadas / duracao);
    }

    printf("\n=== LUGARES ===\n");
    Lugar *lugar = rede->lugares;
    while (lugar)
    {
        double media = (double)lugar->soma_tokens / lugar->num_amostras;
        printf("L%d: atual=%d, max=%d, min=%d, média=%.2f\n",
               lugar->id, lugar->tokens, lugar->max_tokens,
               lugar->min_tokens, media);
        lugar = lugar->prox;
    }

    printf("\n=== TRANSIÇÕES ===\n");
    Transicao *trans = rede->transicoes;
    while (trans)
    {
        printf("T%d: %d execuções\n", trans->id, trans->execucoes);
        trans = trans->prox;
    }
}

void liberar_rede(RedePetri *rede)
{
    if (!rede) return;

    Lugar *lugar = rede->lugares;
    while (lugar)
    {
        Lugar *temp = lugar;
        lugar = lugar->prox;
        free(temp);
    }

    Transicao *trans = rede->transicoes;
    while (trans)
    {
        Transicao *temp = trans;
        trans = trans->prox;
        free(temp);
    }

    Arco *arco = rede->arcos_LT;
    while (arco)
    {
        Arco *temp = arco;
        arco = arco->prox;
        free(temp);
    }

    arco = rede->arcos_TL;
    while (arco)
    {
        Arco *temp = arco;
        arco = arco->prox;
        free(temp);
    }

    free(rede);
}

RedePetri* ler_rede_arquivo(const char *arquivo)
{
    FILE *fp = fopen(arquivo, "r");
    if (!fp) return NULL;

    int num_lugares, num_transicoes, num_arcos_LT, num_arcos_TL;

    fscanf(fp, "%d %d %d %d", &num_lugares, &num_transicoes, &num_arcos_LT, &num_arcos_TL);

    RedePetri *rede = criar_rede_petri(num_lugares, num_transicoes);
    if (!rede)
    {
        fclose(fp);
        return NULL;
    }

    /* Tokens iniciais */
    for (int i = 0; i < num_lugares; i++)
    {
        int tokens;
        fscanf(fp, "%d", &tokens);
        adicionar_lugar(rede, i, tokens);
    }

    /* Transições */
    for (int i = 0; i < num_transicoes; i++)
    {
        adicionar_transicao(rede, i);
    }

    /* Arcos LT */
    for (int i = 0; i < num_arcos_LT; i++)
    {
        int lugar, transicao, peso;
        fscanf(fp, "%d %d %d", &lugar, &transicao, &peso);
        adicionar_arco_LT(rede, lugar, transicao, peso);
    }

    /* Arcos TL */
    for (int i = 0; i < num_arcos_TL; i++)
    {
        int transicao, lugar, peso;
        fscanf(fp, "%d %d %d", &transicao, &lugar, &peso);
        adicionar_arco_TL(rede, transicao, lugar, peso);
    }

    fclose(fp);
    return rede;
}

RedePetri* ler_rede_interativa()
{
    int num_lugares, num_transicoes, num_arcos_LT, num_arcos_TL;

    printf("Quantos lugares? (n>0): ");
    scanf("%d", &num_lugares);
    printf("Quantas transições? (m): ");
    scanf("%d", &num_transicoes);
    printf("Quantos arcos consumidores do tipo LT? (l): ");
    scanf("%d", &num_arcos_LT);
    printf("Quantos arcos produtores do tipo TL? (t): ");
    scanf("%d", &num_arcos_TL);

    RedePetri *rede = criar_rede_petri(num_lugares, num_transicoes);
    if (!rede) return NULL;

    printf("Digite quantos tokens em cada lugar, separados por espaco: ");
    for (int i = 0; i < num_lugares; i++)
    {
        int tokens;
        scanf("%d", &tokens);
        adicionar_lugar(rede, i, tokens);
    }

    for (int i = 0; i < num_transicoes; i++)
    {
        adicionar_transicao(rede, i);
    }

    for (int i = 0; i < num_arcos_LT; i++)
    {
        int lugar, transicao, peso;
        printf("Digite a trinca ALT%d de arco consumidor LT no formato L T R: ", i);
        scanf("%d %d %d", &lugar, &transicao, &peso);
        adicionar_arco_LT(rede, lugar, transicao, peso);
    }

    for (int i = 0; i < num_arcos_TL; i++)
    {
        int transicao, lugar, peso;
        printf("Digite a trinca ATL%d de arco produtor TL no formato T L R: ", i);
        scanf("%d %d %d", &transicao, &lugar, &peso);
        adicionar_arco_TL(rede, transicao, lugar, peso);
    }

    return rede;
}

/* ---------------------------------------------------------------------- */
/**
 * @ingroup GroupUnique
 * @brief Prints help information and exit
 * @details Prints help information (usually called by opt -h)
 * @return Void
 * @author Ruben Carlo Benante
 * @version 20160806.231815
 * @date 2016-08-06
 *
 */
void help(void)
{
    IFDEBUG("help()");
    printf("%s - %s\n", "ex11", "Petri Net Simulator");
    printf("\nUsage: %s [-h|-v|-f filename]\n", "ex11.x");
    printf("\nOptions:\n");
    printf("\t-h,  --help\n\t\tShow this help.\n");
    printf("\t-V,  --version\n\t\tShow version and copyright information.\n");
    printf("\t-v,  --verbose\n\t\tSet verbose level (cumulative).\n");
    printf("\t-f,  --file filename\n\t\tRead Petri Net from file.\n");
    /* add more options here */
    printf("\nExit status:\n\t0 if ok.\n\t1 some error occurred.\n");
    printf("\nTodo:\n\tLong options not implemented yet.\n");
    printf("\nAuthor:\n\tWritten by %s <%s>\n\n", "Ruben Carlo Benante", "rcb@beco.cc");
    exit(EXIT_FAILURE);
}

/* ---------------------------------------------------------------------- */
/**
 * @ingroup GroupUnique
 * @brief Prints version and copyright information and exit
 * @details Prints version and copyright information (usually called by opt -V)
 * @return Void
 * @author Ruben Carlo Benante
 * @version 20160806.231815
 * @date 2016-08-06
 *
 */
void copyr(void)
{
    IFDEBUG("copyr()");
    printf("%s - Version %s\n", "ex11", VERSION);
    printf("\nCopyright (C) %d %s <%s>, GNU GPL version 2 <http://gnu.org/licenses/gpl.html>. This  is  free  software: you are free to change and redistribute it. There is NO WARRANTY, to the extent permitted by law. USE IT AS IT IS. The author takes no responsability to any damage this software may inflige in your data.\n\n", 2016, "Ruben Carlo Benante", "rcb@beco.cc");
    if(verb > 3) printf("copyr(): Verbose: %d\n", verb); /* -vvvv */
    exit(EXIT_FAILURE);
}

/* ---------------------------------------------------------------------- */
/**
 * @ingroup GroupUnique
 * @brief This function initializes some operations before start
 * @details Details to be written in
 * multiple lines
 *
 * @pre You need to call foo() before calling this function
 *
 * @param[in] i Input parameter that does bla
 * @param[out] o Parameter that outputs ble
 * @param[in,out] z The @a z variable is used as input and output
 *
 * @retval 0 Returned when bli
 * @retval 1 Error code: function returned blo
 *
 * @par Example
 * @code
 *    if(x == funcexample(i, o, z))
 *       printf("And that is it\n");
 * @endcode
 *
 * @return Void
 *
 * @warning Be carefull with blu
 * @todo Need to implement it. Its empty now. This doxygen tags are overwhelming.
 * Mandatory tags are: ingroup, brief, details, param, return, author and date.
 * The others are optional.
 *
 * @deprecated This function will be deactivated in version +11
 * @see help()
 * @see copyr()
 * @bug There is a bug with x greater than y
 * @note You can read more about it at <<a href="http://www.beco.cc">www.beco.cc</a>>
 *
 * @author Ruben Carlo Benante
 * @version 20160806.231815
 * @date 2016-08-06
 * @copyright Use this tag only if not the same as the whole file
 *
 */
void ex7_init(void)
{
    IFDEBUG("ex7_init()");
    /* initialization */
    return;
}

/* ---------------------------------------------------------------------------- */
/* vi: set ai cin et ts=4 sw=4 tw=0 wm=0 fo=croqltn : C config for Vim modeline */
/* Template by Dr. Beco <rcb at beco dot cc>  Version 20160714.153029           */
