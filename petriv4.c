/***************************************************************************
 * petriv4.c                                  Version 20240927.150000    *
 * *
 * Simulador de rede de Petri (Funcional e aprimorado)                     *
 * Copyright (C) 2024         by Ruben Carlo Benante & Gemini              *
 ***************************************************************************
 * Baseado nos requisitos do README.md e nos códigos-fonte fornecidos.   *
 * Este programa utiliza listas encadeadas, implementa a lógica de       *
 * disparo não-determinístico, E/S via arquivo ou modo interativo e      *
 * gera estatísticas detalhadas, incluindo desvio padrão.                *
 ***************************************************************************/

#include "petriv4.h"

static int verb = 0;

/* ---------------------------------------------------------------------- */
/* Função Principal */
/* ---------------------------------------------------------------------- */
int main(int argc, char *argv[]) {
    int opt;
    char *arquivo = NULL;
    RedePetri *rede = NULL;
    int max_iteracoes = 1000;

    opterr = 0;
    while ((opt = getopt(argc, argv, "vhVf:")) != EOF) {
        switch (opt) {
            case 'h': help(argv[0]); return EXIT_SUCCESS;
            case 'V': copyr(argv[0]); return EXIT_SUCCESS;
            case 'v': verb++; break;
            case 'f': arquivo = optarg; break;
            case '?':
            default:
                fprintf(stderr, "Opção inválida. Use -h para ajuda.\n");
                return EXIT_FAILURE;
        }
    }

    if (verb) printf("Nível de verbosidade: %d\n", verb);

    if (arquivo != NULL) {
        if (verb) printf("Lendo rede do arquivo: %s\n", arquivo);
        rede = ler_rede_arquivo(arquivo);
    } else {
        if (verb) printf("Lendo rede do modo interativo.\n");
        rede = ler_rede_interativa();
    }

    if (!rede) {
        fprintf(stderr, "Erro: Falha ao criar a rede de Petri.\n");
        return EXIT_FAILURE;
    }

    simular_rede(rede, max_iteracoes);
    imprimir_estatisticas(rede);
    liberar_rede(rede);

    return EXIT_SUCCESS;
}


/* ---------------------------------------------------------------------- */
/* Funções do Núcleo da Simulação */
/* ---------------------------------------------------------------------- */

void simular_rede(RedePetri *rede, int max_iteracoes) {
    srand(time(NULL));
    rede->inicio_simulacao = time(NULL);

    for (int i = 0; i < max_iteracoes; i++) {
        atualizar_estatisticas_lugares(rede);

        Transicao *habilitadas[rede->n_transicoes];
        int n_habilitadas = 0;

        Transicao *trans_atual = rede->transicoes;
        while (trans_atual) {
            rede->transicoes_analisadas++;
            if (transicao_habilitada(rede, trans_atual->id)) {
                habilitadas[n_habilitadas++] = trans_atual;
            }
            trans_atual = trans_atual->prox;
        }

        if (n_habilitadas == 0) {
            rede->deadlock = 1;
            break;
        }

        int alguma_executou = 0;
        for(int k = 0; k < n_habilitadas; k++) {
            int j = rand() % n_habilitadas;
            Transicao *temp = habilitadas[k];
            habilitadas[k] = habilitadas[j];
            habilitadas[j] = temp;
        }
        
        for(int k = 0; k < n_habilitadas; k++) {
            if ((double)rand() / RAND_MAX < TRANSITION_PROBABILITY) {
                 executar_transicao(rede, habilitadas[k]->id);
                 alguma_executou = 1;
                 break;
            }
        }
        
        rede->iteracoes++;

        Lugar *lugar = rede->lugares;
        while (lugar) {
            if (lugar->tokens > MAX_TOKENS) {
                printf("Overflow no lugar L%d! Encerrando.\n", lugar->id);
                rede->fim_simulacao = time(NULL);
                return;
            }
            lugar = lugar->prox;
        }
    }
    // Coleta a última estatística antes de sair
    if (!rede->deadlock) atualizar_estatisticas_lugares(rede);
    rede->fim_simulacao = time(NULL);
}

int transicao_habilitada(RedePetri *rede, int transicao_id) {
    Arco *arco = rede->arcos_LT;
    while (arco) {
        if (arco->destino == transicao_id) {
            Lugar *lugar = encontrar_lugar(rede, arco->origem);
            if (!lugar || lugar->tokens < arco->peso) return 0;
        }
        arco = arco->prox;
    }
    return 1;
}

void executar_transicao(RedePetri *rede, int transicao_id) {
    Arco *arco_lt = rede->arcos_LT;
    while (arco_lt) {
        if (arco_lt->destino == transicao_id) {
            Lugar *lugar = encontrar_lugar(rede, arco_lt->origem);
            if (lugar) lugar->tokens -= arco_lt->peso;
        }
        arco_lt = arco_lt->prox;
    }

    Arco *arco_tl = rede->arcos_TL;
    while (arco_tl) {
        if (arco_tl->origem == transicao_id) {
            Lugar *lugar = encontrar_lugar(rede, arco_tl->destino);
            if (lugar) lugar->tokens += arco_tl->peso;
        }
        arco_tl = arco_tl->prox;
    }
    
    Transicao *t = rede->transicoes;
    while(t) {
        if (t->id == transicao_id) {
            t->execucoes++;
            break;
        }
        t = t->prox;
    }
}


/* ---------------------------------------------------------------------- */
/* Funções de Leitura da Rede (sem alterações) */
/* ---------------------------------------------------------------------- */

RedePetri* ler_rede_arquivo(const char *arquivo) {
    FILE *fp = fopen(arquivo, "r");
    if (!fp) return NULL;
    int n_l, n_t, n_lt, n_tl;
    fscanf(fp, "%d%d%d%d", &n_l, &n_t, &n_lt, &n_tl);
    RedePetri *rede = criar_rede_petri(n_l, n_t);
    if (!rede) { fclose(fp); return NULL; }
    for (int i = 0; i < n_l; i++) { int t; fscanf(fp, "%d", &t); adicionar_lugar(rede, i, t); }
    for (int i = 0; i < n_t; i++) { adicionar_transicao(rede, i); }
    for (int i = 0; i < n_lt; i++) { int l,t,p; fscanf(fp, "%d%d%d",&l,&t,&p); adicionar_arco_LT(rede, l,t,p); }
    for (int i = 0; i < n_tl; i++) { int t,l,p; fscanf(fp, "%d%d%d",&t,&l,&p); adicionar_arco_TL(rede, t,l,p); }
    fclose(fp);
    return rede;
}

RedePetri* ler_rede_interativa() {
    int n_l, n_t, n_lt, n_tl;
    printf("Quantos lugares? (n>0)\n"); scanf("%d", &n_l);
    printf("Quantas transições? (m)\n"); scanf("%d", &n_t);
    printf("Quantos arcos consumidores do tipo LT? (l)\n"); scanf("%d", &n_lt);
    printf("Quantos arcos produtores do tipo TL? (t)\n"); scanf("%d", &n_tl);
    RedePetri *rede = criar_rede_petri(n_l, n_t);
    if (!rede) return NULL;
    printf("Digite quantos tokens em cada lugar, separados por espaco:\n");
    for (int i = 0; i < n_l; i++) { int t; scanf("%d", &t); adicionar_lugar(rede, i, t); }
    for (int i = 0; i < n_t; i++) { adicionar_transicao(rede, i); }
    for (int i = 0; i < n_lt; i++) { int l,t,p; printf("Digite a trinca ALT%d de arco consumidor LT no formato L T R:\n", i); scanf("%d%d%d",&l,&t,&p); adicionar_arco_LT(rede, l,t,p); }
    for (int i = 0; i < n_tl; i++) { int t,l,p; printf("Digite a trinca ATL%d de arco produtor TL no formato T L R:\n", i); scanf("%d%d%d",&t,&l,&p); adicionar_arco_TL(rede, t,l,p); }
    return rede;
}


/* ---------------------------------------------------------------------- */
/* Funções de Estatísticas */
/* ---------------------------------------------------------------------- */

void atualizar_estatisticas_lugares(RedePetri *rede) {
    Lugar *lugar = rede->lugares;
    while(lugar) {
        if (lugar->tokens > lugar->max_tokens) lugar->max_tokens = lugar->tokens;
        if (lugar->tokens < lugar->min_tokens) lugar->min_tokens = lugar->tokens;
        lugar->soma_tokens += lugar->tokens;
        lugar->soma_tokens_quadrado += (long double)lugar->tokens * lugar->tokens;
        lugar->n_amostras++;
        lugar = lugar->prox;
    }
}

void imprimir_estatisticas(RedePetri *rede) {
    double duracao = difftime(rede->fim_simulacao, rede->inicio_simulacao);
    if (duracao < 1) duracao = 1;

    printf("\n=== ESTATÍSTICAS ===\n");
    printf("Status da Simulação: %s\n", rede->deadlock ? "DEADLOCK" : "Limite de iterações atingido");
    printf("Iterações: %lld\n", rede->iteracoes);
    printf("Duração: %.2f segundos\n", duracao);
    printf("Velocidade: %.2f iterações/seg\n", (double)rede->iteracoes / duracao);
    printf("Transições analisadas/seg: %.2f\n", (double)rede->transicoes_analisadas / duracao);

    printf("\n%-5s | %-8s | %-5s | %-5s | %-8s | %-8s\n", "LUGAR", "ATUAL", "MAX", "MIN", "MÉDIA", "DESV.PAD");
    printf("----------------------------------------------------------\n");
    Lugar *lugar = rede->lugares;
    while (lugar) {
        double media = 0;
        double desv_pad = 0;
        if (lugar->n_amostras > 0) {
            media = (double)lugar->soma_tokens / lugar->n_amostras;
            double variancia = (lugar->soma_tokens_quadrado / lugar->n_amostras) - (media * media);
            desv_pad = (variancia > 0) ? sqrt(variancia) : 0;
        }
        printf("L%-4d | %-8d | %-5d | %-5d | %-8.2f | %-8.2f\n",
               lugar->id, lugar->tokens, lugar->max_tokens, lugar->min_tokens, media, desv_pad);
        lugar = lugar->prox;
    }

    long total_execucoes = 0;
    Transicao *trans_count = rede->transicoes;
    while (trans_count) {
        total_execucoes += trans_count->execucoes;
        trans_count = trans_count->prox;
    }

    printf("\n%-12s | %-12s | %-12s\n", "TRANSIÇÃO", "EXECUÇÕES", "% DO TOTAL");
    printf("-------------------------------------------\n");
    Transicao *trans_print = rede->transicoes;
    while (trans_print) {
        double perc = total_execucoes > 0 ? (double)trans_print->execucoes * 100.0 / total_execucoes : 0;
        printf("T%-11d | %-12d | %-11.2f%%\n", trans_print->id, trans_print->execucoes, perc);
        trans_print = trans_print->prox;
    }
}


/* ---------------------------------------------------------------------- */
/* Funções de Criação e Liberação de Memória */
/* ---------------------------------------------------------------------- */

RedePetri* criar_rede_petri(int lugares, int transicoes) {
    RedePetri *rede = (RedePetri*)malloc(sizeof(RedePetri));
    if (!rede) return NULL;
    *rede = (RedePetri){ .n_lugares = lugares, .n_transicoes = transicoes, .deadlock = 0 };
    return rede;
}

void adicionar_lugar(RedePetri *rede, int id, int tokens_iniciais) {
    Lugar *novo = (Lugar*)malloc(sizeof(Lugar));
    if (!novo) return;
    *novo = (Lugar){ .id = id, .tokens = tokens_iniciais, .max_tokens = tokens_iniciais,
                     .min_tokens = tokens_iniciais, .soma_tokens = 0, .soma_tokens_quadrado = 0,
                     .n_amostras = 0, .prox = rede->lugares };
    rede->lugares = novo;
}

void adicionar_transicao(RedePetri *rede, int id) {
    Transicao *nova = (Transicao*)malloc(sizeof(Transicao));
    if (!nova) return;
    *nova = (Transicao){ .id = id, .execucoes = 0, .prox = rede->transicoes };
    rede->transicoes = nova;
}

void adicionar_arco_LT(RedePetri *rede, int l, int t, int p) {
    Arco *novo = (Arco*)malloc(sizeof(Arco));
    if (!novo) return;
    *novo = (Arco){ .origem = l, .destino = t, .peso = p, .prox = rede->arcos_LT };
    rede->arcos_LT = novo;
}

void adicionar_arco_TL(RedePetri *rede, int t, int l, int p) {
    Arco *novo = (Arco*)malloc(sizeof(Arco));
    if (!novo) return;
    *novo = (Arco){ .origem = t, .destino = l, .peso = p, .prox = rede->arcos_TL };
    rede->arcos_TL = novo;
}

Lugar* encontrar_lugar(RedePetri *rede, int id) {
    Lugar *atual = rede->lugares;
    while (atual) {
        if (atual->id == id) return atual;
        atual = atual->prox;
    }
    return NULL;
}

void liberar_rede(RedePetri *rede) {
    if (!rede) return;
    Lugar *l = rede->lugares, *temp_l;
    while(l) { temp_l = l; l = l->prox; free(temp_l); }
    Transicao *t = rede->transicoes, *temp_t;
    while(t) { temp_t = t; t = t->prox; free(temp_t); }
    Arco *a_lt = rede->arcos_LT, *temp_a_lt;
    while(a_lt) { temp_a_lt = a_lt; a_lt = a_lt->prox; free(temp_a_lt); }
    Arco *a_tl = rede->arcos_TL, *temp_a_tl;
    while(a_tl) { temp_a_tl = a_tl; a_tl = a_tl->prox; free(temp_a_tl); }
    free(rede);
}

/* ---------------------------------------------------------------------- */
/* Funções de Ajuda e Copyright (sem alterações) */
/* ---------------------------------------------------------------------- */
void help(const char *prog_name) {
    printf("Simulador de Rede de Petri - Versão %s\n", VERSION);
    printf("Uso: %s [-h|-V|-v] [-f arquivo_da_rede]\n\n", prog_name);
    printf("Opções:\n");
    printf("  -h\t\tMostra esta ajuda.\n");
    printf("  -V\t\tMostra a versão e informações de copyright.\n");
    printf("  -v\t\tAumenta o nível de verbosidade.\n");
    printf("  -f <arquivo>\tLê a rede de Petri a partir do arquivo.\n\n");
    printf("Se -f não for usado, o programa entrará no modo interativo.\n");
}

void copyr(const char *prog_name) {
    printf("%s - Versão %s\n", prog_name, VERSION);
    printf("Copyright (C) 2024 by Ruben Carlo Benante & Gemini\n");
    printf("Este programa é um software livre sob a licença GNU GPL v2.\n");
}
