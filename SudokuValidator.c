#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int sudokuLayout[8][8];

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
    
    printf("\nmmap content: %c\n", file_memory[0]);
    int k = 0;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            sudokuLayout[i][j] = file_memory[k];
            printf("\nElemento agregado: %c\n", file_memory[k]);
            k++;    
        }
    }

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            printf("\nSudoku[%d][%d]: %c\n", i, j, sudokuLayout[i][j]);
        }
    }

    munmap(file_memory, file_size);
    close(fd);
    return 0;
}
