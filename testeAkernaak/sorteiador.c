#include <stdio.h>
#include "ex8.h" // Inclui a biblioteca que criamos

int main() {
    printf("Simulador de Sorteio de Frases do Jogo Akernaak\n");
    printf("--------------------------------------------------\n\n");
    // Chama a função da nossa biblioteca para preparar o sorteio
    inicializar_sorteio();
    // Usa a função de sorteio da biblioteca em um loop
    for (int i = 1; i <= 20; i++) {
        const char* frase_sorteada = sortear_frase();
        printf("Sorteio %d: %s", i, frase_sorteada);
    }

    printf("\n--------------------------------------------------\n");
    printf("Simulacao concluida.\n");
    return 0;
}
