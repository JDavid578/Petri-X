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
