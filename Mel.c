/* ---------------------------------------------------------------------- */
/* Funções de Criação e Liberação de Memória | Vai ser a primeira parte p adicionar*/ 
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
