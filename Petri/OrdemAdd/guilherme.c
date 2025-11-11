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
