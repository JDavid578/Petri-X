#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "ex8.h" // Inclui a própria interface

// --- Definição das Frases e Variáveis de Estado (privadas ao módulo) ---

static const char* frases[9];
static char buffer_frase_formatada[256];
static bool frases_usadas[7] = {false};
static bool categoria_C_ja_saiu = false;
static int turno = 0;
static int frases_unicas_sorteadas = 0;

// Implementação das funções
void inicializar_sorteio() {
    srand(time(NULL));
    
    frases[0] = "-Ha' uma passagem secreta da arca para %s.\n";
    frases[1] = "-...porque os magos detestam vinho.\n";
    frases[2] = "-'akon' e' a palavra das direcoes.\n";
    frases[3] = "-...o pergaminho que esta' dentro do diamante tem o segredo.\n";
    
    if (rand() % 2 == 0) {
        frases[4] = "-a musica acalma as feras.\n";
    } else {
        frases[4] = "-(dicionario) nazg: /Næʒgi/ sm (ling. neg., duendes) 1-anel; 2-anik.\n";
    }
    
    frases[5] = "-'kadon' e' a magia que compra a onisciencia.\n";
    frases[6] = "-'anik' e' a ultima magia para o aprendiz de mago...\n";
    frases[7] = "-O numero magico e': %d\n";
    frases[8] = "Titulo: Alfarrabio de Nyok.\n";

    turno = 0;
}

const char* sortear_frase() {
    turno++;

    if (frases_unicas_sorteadas >= 7) {
        return frases[8];
    }
    
    int categoria_sorteada = -1; // 0=A, 1=B, 2=C

    if (turno == 1) {
        categoria_sorteada = rand() % 3;
    } else {
        bool a_disponivel = !frases_usadas[0];
        bool c_disponivel = !categoria_C_ja_saiu;
        
        int dicas_b_restantes = 0;
        for (int i = 1; i <= 6; i++) {
            if (!frases_usadas[i]) dicas_b_restantes++;
        }
        bool b_disponivel = dicas_b_restantes > 0;

        if (a_disponivel && c_disponivel) {
            categoria_sorteada = (rand() % 2 == 0) ? 0 : 2;
        } 
        else if (a_disponivel && !c_disponivel) {
             categoria_sorteada = (rand() % 2 == 0) ? 0 : 1;
        }
        else if (!a_disponivel && c_disponivel) {
            categoria_sorteada = (rand() % 2 == 0) ? 2 : 1;
        }
        else if (!a_disponivel && !c_disponivel && b_disponivel) {
            categoria_sorteada = (rand() % 3 < 2) ? 1 : 2;
        }
    }
    
    int dicas_b_restantes = 0;
    for (int i = 1; i <= 6; i++) if (!frases_usadas[i]) dicas_b_restantes++;
    if(categoria_sorteada == 1 && dicas_b_restantes == 0){
        if(!frases_usadas[0]) categoria_sorteada = 0;
        else categoria_sorteada = 2;
    }

    switch (categoria_sorteada) {
        case 0:
            if (!frases_usadas[0]) {
                frases_usadas[0] = true;
                frases_unicas_sorteadas++;
                snprintf(buffer_frase_formatada, sizeof(buffer_frase_formatada), frases[0], "biblioteca");
                return buffer_frase_formatada;
            }
        case 1:
            for (int i = 1; i <= 6; i++) {
                if (!frases_usadas[i]) {
                    frases_usadas[i] = true;
                    frases_unicas_sorteadas++;
                    return frases[i];
                }
            }
        case 2:
            categoria_C_ja_saiu = true;
            int numero_magico = (rand() % 100) + 1;
            snprintf(buffer_frase_formatada, sizeof(buffer_frase_formatada), frases[7], numero_magico);
            return buffer_frase_formatada;
        default:
            return "Erro: Nenhuma frase pode ser sorteada.\n";
    }
}
