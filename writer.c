/**
 * @file writer.c
 * @brief Programma per leggere i dati da una FIFO e scriverli su un file.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BLOCK_DIM 1024 ///< Dimensione del blocco di lettura/scrittura

/**
 * @brief Funzione principale del programma.
 * @param argc Numero di argomenti della riga di comando.
 * @param argv Array di stringhe degli argomenti della riga di comando.
 * @return 1 in caso di successo, altrimenti 0.
 */
int main(int argc, char* argv[]) {
    FILE* file;
    unsigned char buffer[BLOCK_DIM];
    int n;
    int fifo;
    
    // Verifica se il numero corretto di argomenti è stato fornito
    if (argc != 2) {
        printf("Uso: %s file\r\n", argv[0]);
        return 0;
    }

    // Apre la FIFO per la lettura
    fifo = open("my_fifo", O_RDONLY);
    if (fifo < 0) {
        printf("Errore apertura FIFO\r\n");
        return 0;
    }

    // Apre il file di destinazione per la scrittura in modalità binaria
    file = fopen(argv[1], "wb");
    if (file == NULL) {
        printf("Errore apertura file \"%s\"\r\n", argv[1]);
        close(fifo);
        return 0;
    }

    // Legge dati dalla FIFO e li scrive nel file di destinazione
    while ((n = read(fifo, buffer, sizeof(buffer))) > 0)
        fwrite(buffer, 1, n, file);

    // Chiude la FIFO e il file di destinazione
    close(fifo);
    fclose(file);
    return 1;
}

