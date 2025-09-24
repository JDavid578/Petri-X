#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINHA 256

// Estrutura para arcos de entrada (Lugar -> Transição)
typedef struct {
    int lugar;
    int transicao;
    int peso;
} ArcoEntrada;

// Estrutura para arcos de saída (Transição -> Lugar)
typedef struct {
    int transicao;
    int lugar;
    int peso;
} ArcoSaida;

// Variáveis globais da rede
int numLugares, numTransicoes;
int *lugares;                 // vetor de tokens em cada lugar
ArcoEntrada *arcosEntrada;
ArcoSaida *arcosSaida;
int numArcosEntrada, numArcosSaida;

// ----------------------------------------------------------------------
// Função auxiliar: lê próxima linha "válida" (ignora comentários com #)
// ----------------------------------------------------------------------
int lerLinhaValida(FILE *fp, char *linha) {
    while (fgets(linha, MAX_LINHA, fp)) {
        // ignora comentários (#) e linhas vazias
        if (linha[0] == '#' || linha[0] == '\n')
            continue;
        return 1; // linha válida
    }
    return 0; // fim do arquivo
}

// ----------------------------------------------------------------------
// Carregar rede de Petri a partir de um arquivo de configuração
// ----------------------------------------------------------------------
void carregarRede(const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "r");
    if (!fp) {
        printf("Erro: não foi possível abrir o arquivo %s\n", nomeArquivo);
        exit(1);
    }

    char linha[MAX_LINHA];

    // Lê número de lugares e transições
    if (!lerLinhaValida(fp, linha)) { printf("Erro ao ler num lugares/transições\n"); exit(1); }
    sscanf(linha, "%d %d", &numLugares, &numTransicoes);

    // Aloca vetor de lugares
    lugares = (int *) malloc(numLugares * sizeof(int));
    if (!lugares) { printf("Erro de memória.\n"); exit(1); }

    // Lê tokens iniciais
    if (!lerLinhaValida(fp, linha)) { printf("Erro ao ler tokens iniciais\n"); exit(1); }
    char *ptr = linha;
    for (int i = 0; i < numLugares; i++) {
        int valor;
        sscanf(ptr, "%d", &valor);
        lugares[i] = valor;
        while (*ptr != ' ' && *ptr != '\0') ptr++;
        while (*ptr == ' ') ptr++;
    }

    // Lê quantidade de arcos de entrada
    if (!lerLinhaValida(fp, linha)) { printf("Erro ao ler num arcos de entrada\n"); exit(1); }
    sscanf(linha, "%d", &numArcosEntrada);

    arcosEntrada = (ArcoEntrada *) malloc(numArcosEntrada * sizeof(ArcoEntrada));
    if (!arcosEntrada) { printf("Erro de memória.\n"); exit(1); }

    // Lê arcos de entrada
    for (int i = 0; i < numArcosEntrada; i++) {
        if (!lerLinhaValida(fp, linha)) { printf("Erro ao ler arco de entrada\n"); exit(1); }
        sscanf(linha, "%d %d %d", &arcosEntrada[i].lugar,
                                  &arcosEntrada[i].transicao,
                                  &arcosEntrada[i].peso);
    }

    // Lê quantidade de arcos de saída
    if (!lerLinhaValida(fp, linha)) { printf("Erro ao ler num arcos de saída\n"); exit(1); }
    sscanf(linha, "%d", &numArcosSaida);

    arcosSaida = (ArcoSaida *) malloc(numArcosSaida * sizeof(ArcoSaida));
    if (!arcosSaida) { printf("Erro de memória.\n"); exit(1); }

    // Lê arcos de saída
    for (int i = 0; i < numArcosSaida; i++) {
        if (!lerLinhaValida(fp, linha)) { printf("Erro ao ler arco de saída\n"); exit(1); }
        sscanf(linha, "%d %d %d", &arcosSaida[i].transicao,
                                  &arcosSaida[i].lugar,
                                  &arcosSaida[i].peso);
    }

    fclose(fp);
}

// ----------------------------------------------------------------------
// Verifica se uma transição está habilitada (pode disparar)
// ----------------------------------------------------------------------
int transicaoHabilitada(int t) {
    for (int i = 0; i < numArcosEntrada; i++) {
        if (arcosEntrada[i].transicao == t) {
            if (lugares[arcosEntrada[i].lugar] < arcosEntrada[i].peso)
                return 0; // não há tokens suficientes
        }
    }
    return 1; // habilitada
}

// ----------------------------------------------------------------------
// Dispara uma transição (consome e produz tokens)
// ----------------------------------------------------------------------
void dispararTransicao(int t) {
    // Consome tokens
    for (int i = 0; i < numArcosEntrada; i++) {
        if (arcosEntrada[i].transicao == t) {
            lugares[arcosEntrada[i].lugar] -= arcosEntrada[i].peso;
        }
    }
    // Produz tokens
    for (int i = 0; i < numArcosSaida; i++) {
        if (arcosSaida[i].transicao == t) {
            lugares[arcosSaida[i].lugar] += arcosSaida[i].peso;
        }
    }
}

// ----------------------------------------------------------------------
// Simulação principal (gera estatísticas ao final)
// ----------------------------------------------------------------------
void simular(int numIteracoes) {
    double somaTokens[numLugares];
    for (int i = 0; i < numLugares; i++) somaTokens[i] = 0.0;

    for (int passo = 0; passo < numIteracoes; passo++) {
        // acumula tokens
        for (int i = 0; i < numLugares; i++)
            somaTokens[i] += lugares[i];

        int disparou = 0;
        for (int t = 0; t < numTransicoes; t++) {
            if (transicaoHabilitada(t)) {
                dispararTransicao(t);
                disparou = 1;
                break; // dispara apenas a primeira habilitada
            }
        }

        if (!disparou) {
            // rede bloqueada, acumula o estado final até o fim das iterações
            for (int passoRest = passo + 1; passoRest < numIteracoes; passoRest++)
                for (int i = 0; i < numLugares; i++)
                    somaTokens[i] += lugares[i];
            break;
        }
    }

    // cálculo total de tokens médio
    double totalMedio = 0;
    for (int i = 0; i < numLugares; i++) totalMedio += somaTokens[i]/numIteracoes;

    // imprime estatísticas
    printf("\n--- Estatísticas após %d iterações ---\n", numIteracoes);
    for (int i = 0; i < numLugares; i++) {
        double media = somaTokens[i]/numIteracoes;
        double perc = (totalMedio > 0) ? (100.0 * media / totalMedio) : 0.0;
        printf("Lugar P%d: Média = %.2f tokens (%.1f%%)\n", i, media, perc);
    }
    printf("-------------------------------------\n");
}

// ----------------------------------------------------------------------
// Programa principal
// ----------------------------------------------------------------------
int main() {
    char nomeArquivo[100];
    int iteracoes;

    printf("Digite o nome do arquivo da rede de Petri: ");
    scanf("%s", nomeArquivo);

    carregarRede(nomeArquivo);

    printf("Digite o número de iterações: ");
    scanf("%d", &iteracoes);

    simular(iteracoes);

    // libera memória
    free(lugares);
    free(arcosEntrada);
    free(arcosSaida);

    return 0;
}
