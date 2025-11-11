/* ---------------------------------------------------------------------- */
/* Funções de Leitura da Rede */
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
