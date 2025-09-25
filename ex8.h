#ifndef EX8_H
#define EX8_H

/**
 * @brief Inicializa o sistema de sorteio.
 * Deve ser chamada uma única vez no início do programa.
 */
void inicializar_sorteio();

/**
 * @brief Realiza o sorteio de uma nova frase.
 * @return Retorna um ponteiro constante para a string da frase sorteada.
 */
const char* sortear_frase();

#endif // EX8_H
