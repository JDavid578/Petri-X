#include "petrij.h"

/* ----------------- Implementação ----------------- */

RedePetri* criarRede() {
    RedePetri *rede = (RedePetri*)malloc(sizeof(RedePetri));
    rede->lugares = NULL;
    rede->transicoes = NULL;
    srand(time(NULL));
    return rede;
}

Lugar* adicionarLugar(RedePetri *rede, int id, int tokens, int tokens_max) {
    Lugar *novo = (Lugar*)malloc(sizeof(Lugar));
    novo->id = id;
    novo->tokens = tokens;
    novo->tokens_max = tokens_max;
    novo->next = rede->lugares;
    rede->lugares = novo;
    return novo;
}

Transicao* adicionarTransicao(RedePetri *rede, int id) {
    Transicao *t = (Transicao*)malloc(sizeof(Transicao));
    t->id = id;
    t->consumidores = NULL;
    t->produtores = NULL;
    t->next = rede->transicoes;
    rede->transicoes = t;
    return t;
}

void adicionarArcoConsumidor(Transicao *t, int lugarId, int peso) {
    Arco *a = (Arco*)malloc(sizeof(Arco));
    a->origem = lugarId;
    a->peso = peso;
    a->next = t->consumidores;
    t->consumidores = a;
}

void adicionarArcoProdutor(Transicao *t, int lugarId, int peso) {
    Arco *a = (Arco*)malloc(sizeof(Arco));
    a->destino = lugarId;
    a->peso = peso;
    a->next = t->produtores;
    t->produtores = a;
}

Lugar* buscarLugar(RedePetri *rede, int id) {
    Lugar *l = rede->lugares;
    while(l) {
        if(l->id == id) return l;
        l = l->next;
    }
    return NULL;
}

Transicao* buscarTransicao(RedePetri *rede, int id) {
    Transicao *t = rede->transicoes;
    while(t) {
        if(t->id == id) return t;
        t = t->next;
    }
    return NULL;
}

int pode(Transicao *t, RedePetri *rede) {
    Arco *a = t->consumidores;
    while(a) {
        Lugar *l = buscarLugar(rede, a->origem);
        if(!l || l->tokens < a->peso) return 0;
        a = a->next;
    }
    return 1;
}

int quer() {
    return rand() % 100 < 55; // 55% chance
}

void executarTransicao(Transicao *t, RedePetri *rede) {
    if(pode(t, rede) && quer()) {
        Arco *a = t->consumidores;
        while(a) {
            Lugar *l = buscarLugar(rede, a->origem);
            l->tokens -= a->peso;
            a = a->next;
        }
        a = t->produtores;
        while(a) {
            Lugar *l = buscarLugar(rede, a->destino);
            if(l) l->tokens += a->peso;
            a = a->next;
        }
    }
}

void executarRede(RedePetri *rede, int iteracoes) {
    for(int i=0; i<iteracoes; i++) {
        Transicao *t = rede->transicoes;
        while(t) {
            executarTransicao(t, rede);
            t = t->next;
        }
    }
}

void imprimirTokens(RedePetri *rede) {
    Lugar *l = rede->lugares;
    printf("Tokens nos lugares:\n");
    while(l) {
        printf("Lugar %d: %d\n", l->id, l->tokens);
        l = l->next;
    }
}

void liberarRede(RedePetri *rede) {
    Lugar *l = rede->lugares;
    while(l) { Lugar *tmp=l; l=l->next; free(tmp); }
    Transicao *t = rede->transicoes;
    while(t) {
        Arco *a = t->consumidores;
        while(a) { Arco *tmp=a; a=a->next; free(tmp); }
        a = t->produtores;
        while(a) { Arco *tmp=a; a=a->next; free(tmp); }
        Transicao *tmpT = t;
        t = t->next;
        free(tmpT);
    }
    free(rede);
}

/* ----------------- Main incluso ----------------- */
int main() {
    RedePetri *rede = criarRede();

    // Exemplo simples: 2 lugares e 1 transição
    adicionarLugar(rede, 0, 5, 1000);
    adicionarLugar(rede, 1, 0, 1000);
    Transicao *t = adicionarTransicao(rede, 0);
    adicionarArcoConsumidor(t, 0, 2);
    adicionarArcoProdutor(t, 1, 2);

    printf("Antes da execução:\n");
    imprimirTokens(rede);

    executarRede(rede, 10);

    printf("Depois da execução:\n");
    imprimirTokens(rede);

    liberarRede(rede);
    return 0;
}

