#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#include "SudokuValidator.c"

int sudokuLayout[9][9];

int main(int argc, char const *argv[])
{
    size_t file_size = 1024;
    char file_content[file_size];
    int fd = open(argv[1], O_RDONLY);

    if (fd == -1) {
        perror("Error opening file");
        return -1;
    }

    char *file_memory = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);

    if (file_memory == MAP_FAILED) {
        perror("Error at mmap");
        close(fd);
        return -1;
    }
    
    printf("\nmmap content: %s\n", file_memory);
    int k = 0;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            sudokuLayout[i][j] = file_memory[k];
            k++;    
        }
    }

    int columns_validation = column_validator(sudokuLayout);
    int rows_validation = row_validator(sudokuLayout);

    int values[3] = {0, 3, 6};
    int sub_array_validation = 0;

    for (int i = 0; i < 3; i++) {
        sub_array_validation += three_X_three(sudokuLayout, values[i], values[i]);
    }
    
    (columns_validation == 1 && rows_validation == 1 && sub_array_validation == 3) ? printf("Sudoku Válido\n\n") : printf("Sudoku Inválido\n\n");

    pid_t parent = getpid();
    pid_t child = fork();

    if(child < 0) {
        perror("Error al hacer fork");
        return -1;

    } else if (child == 0) { // Hijo
        printf("\nSoy el hijo\n");
        char buffer[8];
        sprintf(buffer, "%d", parent);

        execlp("ps -p %s -lLf", buffer);

    } else { // Padre
        printf("Soy el padre\n");
        pthread_t ptid;
        // pthread_create(ptid, NULL, column_validator, sudokuLayout);
    }

    munmap(file_memory, file_size);
    close(fd);
    return 0;
}