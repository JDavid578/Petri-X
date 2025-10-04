
/* ---------------------------------------------------------------------- */
/* Função Principal */
/* ---------------------------------------------------------------------- */

static int verb = 0;

int main(int argc, char *argv[]) {
    int opt;
    char *arquivo = NULL;
    RedePetri *rede = NULL;
    int max_iteracoes = 1000;

    opterr = 0;
    while ((opt = getopt(argc, argv, "vhVf:")) != EOF) {
        switch (opt) {
            case 'h': help(argv[0]); return EXIT_SUCCESS;
            case 'V': copyr(argv[0]); return EXIT_SUCCESS;
            case 'v': verb++; break;
            case 'f': arquivo = optarg; break;
            case '?':
            default:
                fprintf(stderr, "Opção inválida. Use -h para ajuda.\n");
                return EXIT_FAILURE;
        }
    }

    if (verb) printf("Nível de verbosidade: %d\n", verb);

    if (arquivo != NULL) {
        if (verb) printf("Lendo rede do arquivo: %s\n", arquivo);
        rede = ler_rede_arquivo(arquivo);
    } else {
        if (verb) printf("Lendo rede do modo interativo.\n");
        rede = ler_rede_interativa();
    }

    if (!rede) {
        fprintf(stderr, "Erro: Falha ao criar a rede de Petri.\n");
        return EXIT_FAILURE;
    }

    simular_rede(rede, max_iteracoes);
    imprimir_estatisticas(rede);
    liberar_rede(rede); // Será adicionado depois

    return EXIT_SUCCESS;
}
