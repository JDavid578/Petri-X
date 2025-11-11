#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINHA 256

typedef struct {
    int lugar;
    int transicao;
    int peso;
} ArcoEntrada;

typedef struct {
    int transicao;
    int lugar;
    int peso;
} ArcoSaida;

int numLugares, numTransicoes;
int *lugares;
ArcoEntrada *arcosEntrada;
ArcoSaida *arcosSaida;
int numArcosEntrada, numArcosSaida;

void carregarRede(FILE *fp) {
    if (!fp) {
        printf("Erro: entrada inválida.\n");
        exit(1);
    }

    // Lê número de lugares, transições, arcos LT, arcos TL
    if (fscanf(fp, "%d", &numLugares) != 1) { printf("Erro ao ler num lugares\n"); exit(1); }
    if (fscanf(fp, "%d", &numTransicoes) != 1) { printf("Erro ao ler num transições\n"); exit(1); }
    if (fscanf(fp, "%d", &numArcosEntrada) != 1) { printf("Erro ao ler num arcos LT\n"); exit(1); }
    if (fscanf(fp, "%d", &numArcosSaida) != 1) { printf("Erro ao ler num arcos TL\n"); exit(1); }

    // Aloca vetor de lugares
    lugares = (int *) malloc(numLugares * sizeof(int));
    if (!lugares) { printf("Erro de memória.\n"); exit(1); }

    // Lê tokens iniciais
    for (int i = 0; i < numLugares; i++) {
        if (fscanf(fp, "%d", &lugares[i]) != 1) {
            printf("Erro ao ler tokens iniciais\n");
            exit(1);
        }
    }

    // Aloca arcos de entrada
    arcosEntrada = (ArcoEntrada *) malloc(numArcosEntrada * sizeof(ArcoEntrada));
    if (!arcosEntrada) { printf("Erro de memória.\n"); exit(1); }

    // Lê arcos LT (Lugar -> Transição)
    for (int i = 0; i < numArcosEntrada; i++) {
        if (fscanf(fp, "%d %d %d", &arcosEntrada[i].lugar,
                                     &arcosEntrada[i].transicao,
                                     &arcosEntrada[i].peso) != 3) {
            printf("Erro ao ler arco LT\n");
            exit(1);
        }
    }

    // Aloca arcos de saída
    arcosSaida = (ArcoSaida *) malloc(numArcosSaida * sizeof(ArcoSaida));
    if (!arcosSaida) { printf("Erro de memória.\n"); exit(1); }

    // Lê arcos TL (Transição -> Lugar)
    for (int i = 0; i < numArcosSaida; i++) {
        if (fscanf(fp, "%d %d %d", &arcosSaida[i].transicao,
                                     &arcosSaida[i].lugar,
                                     &arcosSaida[i].peso) != 3) {
            printf("Erro ao ler arco TL\n");
            exit(1);
        }
    }
}

int transicaoHabilitada(int t) {
    for (int i = 0; i < numArcosEntrada; i++) {
        if (arcosEntrada[i].transicao == t) {
            if (lugares[arcosEntrada[i].lugar] < arcosEntrada[i].peso)
                return 0;
        }
    }
    return 1;
}

void dispararTransicao(int t) {
    for (int i = 0; i < numArcosEntrada; i++) {
        if (arcosEntrada[i].transicao == t) {
            lugares[arcosEntrada[i].lugar] -= arcosEntrada[i].peso;
        }
    }
    for (int i = 0; i < numArcosSaida; i++) {
        if (arcosSaida[i].transicao == t) {
            lugares[arcosSaida[i].lugar] += arcosSaida[i].peso;
        }
    }
}

void simular(int numIteracoes) {
    double somaTokens[numLugares];
    for (int i = 0; i < numLugares; i++) somaTokens[i] = 0.0;

    for (int passo = 0; passo < numIteracoes; passo++) {
        for (int i = 0; i < numLugares; i++)
            somaTokens[i] += lugares[i];

        int disparou = 0;
        for (int t = 0; t < numTransicoes; t++) {
            if (transicaoHabilitada(t)) {
                dispararTransicao(t);
                disparou = 1;
                break;
            }
        }

        if (!disparou) {
            for (int passoRest = passo + 1; passoRest < numIteracoes; passoRest++)
                for (int i = 0; i < numLugares; i++)
                    somaTokens[i] += lugares[i];
            break;
        }
    }

    double totalMedio = 0;
    for (int i = 0; i < numLugares; i++) totalMedio += somaTokens[i]/numIteracoes;

    printf("\n--- Estatísticas após %d iterações ---\n", numIteracoes);
    for (int i = 0; i < numLugares; i++) {
        double media = somaTokens[i]/numIteracoes;
        double perc = (totalMedio > 0) ? (100.0 * media / totalMedio) : 0.0;
        printf("Lugar P%d: Média = %.2f tokens (%.1f%%)\n", i, media, perc);
    }
    printf("-------------------------------------\n");
}

int main(int argc, char *argv[]) {
    FILE *fp = NULL;
    int iteracoes;

    if (argc == 1) {
        // leitura de stdin
        fp = stdin;
    } else if (argc == 3 && strcmp(argv[1], "-f") == 0) {
        // leitura de arquivo
        fp = fopen(argv[2], "r");
        if (!fp) {
            printf("Erro ao abrir o arquivo %s\n", argv[2]);
            return 1;
        }
    } else {
        printf("Uso: %s [-f arquivo] < entrada.txt\n", argv[0]);
        return 1;
    }

    carregarRede(fp);
    if (fp != stdin) fclose(fp);

    printf("Digite o número de iterações: ");
    scanf("%d", &iteracoes);

    simular(iteracoes);

    free(lugares);
    free(arcosEntrada);
    free(arcosSaida);

    return 0;
}
