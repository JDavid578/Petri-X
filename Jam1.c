/* ---------------------------------------------------------------------- */
/* Funções do Núcleo da Simulação */
/* ---------------------------------------------------------------------- */

void simular_rede(RedePetri *rede, int max_iteracoes) {
    srand(time(NULL));
    rede->inicio_simulacao = time(NULL);

    for (int i = 0; i < max_iteracoes; i++) {
        atualizar_estatisticas_lugares(rede); // Será adicionado depois

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

        for(int k = 0; k < n_habilitadas; k++) {
            int j = rand() % n_habilitadas;
            Transicao *temp = habilitadas[k];
            habilitadas[k] = habilitadas[j];
            habilitadas[j] = temp;
        }

        for(int k = 0; k < n_habilitadas; k++) {
            if ((double)rand() / RAND_MAX < TRANSITION_PROBABILITY) {
                 executar_transicao(rede, habilitadas[k]->id);
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
    if (!rede->deadlock) atualizar_estatisticas_lugares(rede); // Será adicionado depois
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
