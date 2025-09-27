/***************************************************************************
 * petriv4.c                                  Version 20240927.120000    *
 * *
 * Simulador de rede de Petri (Funcional)                                *
 * Copyright (C) 2024         by Ruben Carlo Benante & Gemini            *
 ***************************************************************************
 * Baseado nos requisitos do README.md e nos códigos-fonte fornecidos.   *
 * Este programa utiliza listas encadeadas, implementa a lógica de       *
 * disparo não-determinístico e E/S via arquivo ou modo interativo.      *
 ***************************************************************************/

#include "petriv4.h"

static int verb = 0; // Nível de verbosidade

/* ---------------------------------------------------------------------- */
/* Função Principal */
/* ---------------------------------------------------------------------- */
int main(int argc, char *argv[]) {
    int opt;
    char *arquivo = NULL;
    RedePetri *rede = NULL;
    int max_iteracoes = 1000; // Limite padrão de iterações

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

    if (verb) {
        printf("Nível de verbosidade: %d\n", verb);
    }

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
        // --- FASE 1: Sensibilização (Encontrar transições habilitadas) ---
        Transicao *habilitadas[rede->n_transicoes];
        int n_habilitadas = 0;

        Transicao *trans_atual = rede->transicoes;
        while (trans_atual) {
            rede->transicoes_analisadas++;
            if (transicao_habilitada(rede, trans_atual->id)) {
                habilitadas[n_habilitadas] = trans_atual;
                n_habilitadas++;
            }
            trans_atual = trans_atual->prox;
        }

        // --- FASE 2: Escolha e Disparo ---
        if (n_habilitadas == 0) {
            if(verb) printf("Deadlock na iteração %lld\n", rede->iteracoes + 1);
            break; // Deadlock real
        }

        // Tenta disparar uma das habilitadas que "quer"
        int alguma_executou = 0;
        // Para garantir aleatoriedade, embaralhamos a lista de habilitadas
        for(int k = 0; k < n_habilitadas; k++) {
            int j = rand() % n_habilitadas;
            Transicao *temp = habilitadas[k];
            habilitadas[k] = habilitadas[j];
            habilitadas[j] = temp;
        }
        
        for(int k = 0; k < n_habilitadas; k++) {
            // Verifica a condição "quer?"
            if ((double)rand() / RAND_MAX < TRANSITION_PROBABILITY) {
                 executar_transicao(rede, habilitadas[k]->id);
                 alguma_executou = 1;
                 break; // Dispara apenas uma por iteração para simular escolha
            }
        }
        
        atualizar_estatisticas_lugares(rede);
        rede->iteracoes++;

        // Se nenhuma transição habilitada "quis" disparar, a rede não avança neste ciclo,
        // mas não é um deadlock. Tentamos novamente na próxima iteração.
        if (!alguma_executou && verb > 1) {
            printf("Iteração %lld: Transições habilitadas, mas nenhuma quis disparar.\n", rede->iteracoes);
        }

        // Verificar overflow
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

    rede->fim_simulacao = time(NULL);
}

int transicao_habilitada(RedePetri *rede, int transicao_id) {
    Arco *arco = rede->arcos_LT;
    while (arco) {
        if (arco->destino == transicao_id) {
            Lugar *lugar = encontrar_lugar(rede, arco->origem);
            if (!lugar || lugar->tokens < arco->peso) {
                return 0; // Não pode (tokens insuficientes)
            }
        }
        arco = arco->prox;
    }
    return 1; // Pode
}

void executar_transicao(RedePetri *rede, int transicao_id) {
    // Consumir tokens
    Arco *arco_lt = rede->arcos_LT;
    while (arco_lt) {
        if (arco_lt->destino == transicao_id) {
            Lugar *lugar = encontrar_lugar(rede, arco_lt->origem);
            if (lugar) {
                lugar->tokens -= arco_lt->peso;
            }
        }
        arco_lt = arco_lt->prox;
    }

    // Produzir tokens
    Arco *arco_tl = rede->arcos_TL;
    while (arco_tl) {
        if (arco_tl->origem == transicao_id) {
            Lugar *lugar = encontrar_lugar(rede, arco_tl->destino);
            if (lugar) {
                lugar->tokens += arco_tl->peso;
            }
        }
        arco_tl = arco_tl->prox;
    }
    
    // Atualiza contagem de execuções da transição
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
/* Funções de Leitura da Rede */
/* ---------------------------------------------------------------------- */

RedePetri* ler_rede_arquivo(const char *arquivo) {
    FILE *fp = fopen(arquivo, "r");
    if (!fp) return NULL;

    int n_l, n_t, n_lt, n_tl;
    fscanf(fp, "%d", &n_l);
    fscanf(fp, "%d", &n_t);
    fscanf(fp, "%d", &n_lt);
    fscanf(fp, "%d", &n_tl);

    RedePetri *rede = criar_rede_petri(n_l, n_t);
    if (!rede) {
        fclose(fp);
        return NULL;
    }

    for (int i = 0; i < n_l; i++) {
        int tokens;
        fscanf(fp, "%d", &tokens);
        adicionar_lugar(rede, i, tokens);
    }
    for (int i = 0; i < n_t; i++) {
        adicionar_transicao(rede, i);
    }
    for (int i = 0; i < n_lt; i++) {
        int l, t, p;
        fscanf(fp, "%d %d %d", &l, &t, &p);
        adicionar_arco_LT(rede, l, t, p);
    }
    for (int i = 0; i < n_tl; i++) {
        int t, l, p;
        fscanf(fp, "%d %d %d", &t, &l, &p);
        adicionar_arco_TL(rede, t, l, p);
    }

    fclose(fp);
    return rede;
}

RedePetri* ler_rede_interativa() {
    int n_l, n_t, n_lt, n_tl;

    printf("Quantos lugares? (n>0)\n");
    scanf("%d", &n_l);
    printf("Quantas transições? (m)\n");
    scanf("%d", &n_t);
    printf("Quantos arcos consumidores do tipo LT? (l)\n");
    scanf("%d", &n_lt);
    printf("Quantos arcos produtores do tipo TL? (t)\n");
    scanf("%d", &n_tl);

    RedePetri *rede = criar_rede_petri(n_l, n_t);
    if (!rede) return NULL;

    printf("Digite quantos tokens em cada lugar, separados por espaco:\n");
    for (int i = 0; i < n_l; i++) {
        int tokens;
        scanf("%d", &tokens);
        adicionar_lugar(rede, i, tokens);
    }
    for (int i = 0; i < n_t; i++) {
        adicionar_transicao(rede, i);
    }
    for (int i = 0; i < n_lt; i++) {
        int l, t, p;
        printf("Digite a trinca ALT%d de arco consumidor LT no formato L T R:\n", i);
        scanf("%d %d %d", &l, &t, &p);
        adicionar_arco_LT(rede, l, t, p);
    }
    for (int i = 0; i < n_tl; i++) {
        int t, l, p;
        printf("Digite a trinca ATL%d de arco produtor TL no formato T L R:\n", i);
        scanf("%d %d %d", &t, &l, &p);
        adicionar_arco_TL(rede, t, l, p);
    }
    return rede;
}


/* ---------------------------------------------------------------------- */
/* Funções de Estatísticas e Saída */
/* ---------------------------------------------------------------------- */

void atualizar_estatisticas_lugares(RedePetri *rede) {
    Lugar *lugar = rede->lugares;
    while(lugar) {
        if (lugar->tokens > lugar->max_tokens) lugar->max_tokens = lugar->tokens;
        if (lugar->tokens < lugar->min_tokens) lugar->min_tokens = lugar->tokens;
        lugar->soma_tokens_quadrado += (long long)lugar->tokens * lugar->tokens;
        lugar->n_amostras++;
        lugar = lugar->prox;
    }
}

void imprimir_estatisticas(RedePetri *rede) {
    double duracao = difftime(rede->fim_simulacao, rede->inicio_simulacao);
    if (duracao < 1) duracao = 1; // Evitar divisão por zero

    printf("\n=== ESTATÍSTICAS ===\n");
    printf("Iterações: %lld\n", rede->iteracoes);
    printf("Duração: %.2f segundos\n", duracao);
    printf("Velocidade: %.2f iterações/seg\n", (double)rede->iteracoes / duracao);
    printf("Transições analisadas/seg: %.2f\n", (double)rede->transicoes_analisadas / duracao);

    printf("\n=== LUGARES ===\n");
    Lugar *lugar = rede->lugares;
    while (lugar) {
        long long soma_tokens = sqrt(lugar->soma_tokens_quadrado);
        double media = lugar->n_amostras > 0 ? (double)soma_tokens / lugar->n_amostras : 0;
        printf("L%d: atual=%d, max=%d, min=%d, média=%.2f\n",
               lugar->id, lugar->tokens, lugar->max_tokens, lugar->min_tokens, media);
        lugar = lugar->prox;
    }

    printf("\n=== TRANSIÇÕES ===\n");
    Transicao *trans = rede->transicoes;
    while (trans) {
        printf("T%d: %d execuções\n", trans->id, trans->execucoes);
        trans = trans->prox;
    }
}


/* ---------------------------------------------------------------------- */
/* Funções de Criação e Liberação de Memória */
/* ---------------------------------------------------------------------- */

RedePetri* criar_rede_petri(int lugares, int transicoes) {
    RedePetri *rede = (RedePetri*)malloc(sizeof(RedePetri));
    if (!rede) return NULL;

    rede->n_lugares = lugares;
    rede->n_transicoes = transicoes;
    rede->lugares = NULL;
    rede->transicoes = NULL;
    rede->arcos_LT = NULL;
    rede->arcos_TL = NULL;
    rede->iteracoes = 0;
    rede->transicoes_analisadas = 0;

    return rede;
}

void adicionar_lugar(RedePetri *rede, int id, int tokens_iniciais) {
    Lugar *novo = (Lugar*)malloc(sizeof(Lugar));
    if (!novo) return;
    novo->id = id;
    novo->tokens = tokens_iniciais;
    novo->max_tokens = tokens_iniciais;
    novo->min_tokens = tokens_iniciais;
    novo->soma_tokens_quadrado = (long long)tokens_iniciais * tokens_iniciais;
    novo->n_amostras = 1;
    novo->prox = rede->lugares;
    rede->lugares = novo;
}

void adicionar_transicao(RedePetri *rede, int id) {
    Transicao *nova = (Transicao*)malloc(sizeof(Transicao));
    if (!nova) return;
    nova->id = id;
    nova->execucoes = 0;
    nova->prox = rede->transicoes;
    rede->transicoes = nova;
}

void adicionar_arco_LT(RedePetri *rede, int lugar, int transicao, int peso) {
    Arco *novo = (Arco*)malloc(sizeof(Arco));
    if (!novo) return;
    novo->origem = lugar;
    novo->destino = transicao;
    novo->peso = peso;
    novo->prox = rede->arcos_LT;
    rede->arcos_LT = novo;
}

void adicionar_arco_TL(RedePetri *rede, int transicao, int lugar, int peso) {
    Arco *novo = (Arco*)malloc(sizeof(Arco));
    if (!novo) return;
    novo->origem = transicao;
    novo->destino = lugar;
    novo->peso = peso;
    novo->prox = rede->arcos_TL;
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
    
    Lugar *lugar = rede->lugares;
    while (lugar) {
        Lugar *temp = lugar;
        lugar = lugar->prox;
        free(temp);
    }
    
    Transicao *trans = rede->transicoes;
    while (trans) {
        Transicao *temp = trans;
        trans = trans->prox;
        free(temp);
    }
    
    Arco *arco_lt = rede->arcos_LT;
    while (arco_lt) {
        Arco *temp = arco_lt;
        arco_lt = arco_lt->prox;
        free(temp);
    }
    
    Arco *arco_tl = rede->arcos_TL;
    while (arco_tl) {
        Arco *temp = arco_tl;
        arco_tl = arco_tl->prox;
        free(temp);
    }
    
    free(rede);
}


/* ---------------------------------------------------------------------- */
/* Funções de Ajuda e Copyright */
/* ---------------------------------------------------------------------- */

void help(const char *prog_name) {
    printf("Simulador de Rede de Petri - Versão %s\n", VERSION);
    printf("Uso: %s [-h|-V|-v] [-f arquivo_da_rede]\n\n", prog_name);
    printf("Opções:\n");
    printf("  -h\t\tMostra esta ajuda.\n");
    printf("  -V\t\tMostra a versão e informações de copyright.\n");
    printf("  -v\t\tAumenta o nível de verbosidade (pode ser usado múltiplas vezes).\n");
    printf("  -f <arquivo>\tLê a rede de Petri a partir do arquivo especificado.\n\n");
    printf("Se -f não for usado, o programa entrará no modo interativo.\n");
}

void copyr(const char *prog_name) {
    printf("%s - Versão %s\n", prog_name, VERSION);
    printf("Copyright (C) 2024 by Ruben Carlo Benante & Gemini\n");
    printf("Este programa é um software livre sob a licença GNU GPL v2.\n");
}
