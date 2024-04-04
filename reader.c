/**
 * @file reader.c
 * @brief Programma per copiare il contenuto di un file in un altro utilizzando una FIFO.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BLOCK_DIM 1024 ///< Dimensione del blocco di lettura/scrittura
#define PROGRAM "writer" ///< Nome del programma per la creazione di processi

/**
 * @brief Crea un processo figlio per eseguire un programma specificato con gli argomenti dati.
 * @param argument Array di stringhe degli argomenti del programma.
 * @return 0 se è il processo padre, -1 in caso di errore, non ritorna se è il processo figlio.
 */
int spawn(char program[], char* argument[]) {
    int pid;
    
    pid = fork();
    if (pid < 0)
        return -1;
    if (pid > 0)
        return 0;
    execv(program, argument);
    abort();
}

/**
 * @brief Funzione principale del programma.
 * @param argc Numero di argomenti della riga di comando.
 * @param argv Array di stringhe degli argomenti della riga di comando.
 * @return 1 in caso di successo, altrimenti 0.
 */
int main(int argc, char* argv[]) {
    FILE* file;
    int fifo;
    unsigned char buffer[BLOCK_DIM];
    int n;
    char* arg[3];
    
    // Verifica se il numero corretto di argomenti è stato fornito
    if (argc != 3) {
        printf("Uso: %s file-origine file-destinazione\r\n", argv[0]);
        return 0;
    }
    
    // Alloca memoria per gli argomenti
    arg[0] = (char*)malloc(strlen(PROGRAM) + 1);
    strcpy(arg[0], PROGRAM);
    arg[1] = (char*)malloc(strlen(argv[2]) + 1);
    strcpy(arg[1], argv[2]);
    arg[2] = NULL;
    
    // Crea un processo figlio per eseguire il programma "writer"
    if (spawn(PROGRAM, arg) < 0) {
        printf("Errore creazione processo\r\n");
        free(arg[0]);
        free(arg[1]);
        return 0;
    }
    
    // Apre la FIFO per la scrittura
    fifo = open("my_fifo", O_WRONLY);
    if (fifo < 0) {
        printf("Errore apertura FIFO\r\n");
        free(arg[0]);
        free(arg[1]);
        return 0;
    }
    
    // Apre il file di origine per la lettura in modalità binaria
    file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("Errore apertura file \"%s\"\r\n", argv[1]);
        close(fifo);
        free(arg[0]);
        free(arg[1]);
        return 0;
    }
    
    // Legge dati dal file di origine e li scrive nella FIFO
    while ((n = fread(buffer, 1, sizeof(buffer), file)) > 0)
        write(fifo, buffer, n);
    fclose(file);
    close(fifo);
    free(arg[0]);
    free(arg[1]);
    return 1;
}

