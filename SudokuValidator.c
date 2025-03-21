#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdbool.h>
#include <omp.h>

// Variable global
int sudokuLayout[9][9];

// Verificador de columnas
void* column_validator() {
    omp_set_num_threads(9);
    omp_set_nested(true); // Anidación de regiones paralelas

    int *return_value = malloc(sizeof(int));
    *return_value = 0;
    int thread_number = syscall(SYS_gettid);
    printf("Thread que realiza la revisión de columnas: %d\n", thread_number);
    
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < 9; i++) {
        int total = 0;
        thread_number = syscall(SYS_gettid);
        #pragma omp parallel for schedule(dynamic)
        for (int j = 0; j < 9; j++) {
            int number = sudokuLayout[j][i] - '0';
            total += number;
        } 
        
        printf("Thread en ejecución durante revisión de columnas: %d\n", thread_number);

        if (total == 45) {
           *return_value += 1;
        }
    }

    *return_value = (*return_value == 9) ? 1 : 0;

    pthread_exit(return_value);
}

// Verificador de filas no pthread
void* row_validator() {
    omp_set_num_threads(9);
    omp_set_nested(true); // Anidación de regiones paralelas
    int *return_value = malloc(sizeof(int));
    *return_value = 0;
    int thread_number = syscall(SYS_gettid);
    printf("Thread que realiza la revisión de filas: %d\n", thread_number);

    #pragma omp parallel for //schedule(dynamic)
    for (int i = 0; i < 9; i++) {
        int total = 0;
        #pragma omp parallel for schedule(dynamic)
        for (int j = 0; j < 9; j++) {
            int number = sudokuLayout[i][j] - '0';
            total += number;
        } 

        printf("Thread en ejecución durante revisión de filas: %d\n", thread_number);

        if (total == 45) {
           *return_value += 1;
        }
    }

    // printf("Return Row: %d\n", *return_value);

    *return_value = (*return_value == 9) ? 1 : 0;

    return return_value;
}

// Verificador de submatrices
int three_X_three(int row, int column) {
    int total = 0;
    int seen[10] = {0};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int number = sudokuLayout[row + i][column + j] - '0';
            if (number < 1 || number > 9 || seen[number]) {
                return 0;
            }
            seen[number] = 1;
            total += number;
        }
    }

    return (total == 45) ? 1 : 0;
}

int main(int argc, char const *argv[])
{
    omp_set_num_threads(1); // Número de hilos = 1

    size_t file_size = 1024;
    char file_content[file_size];
    int fd = open(argv[1], O_RDONLY);

    if (fd == -1) {
        perror("Error opening file");
        return -1;
    }

    char *file_memory = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0); // Almacenado de sudoku en memoria

    if (file_memory == MAP_FAILED) {
        perror("Error at mmap");
        close(fd);
        return -1;
    }
    
    printf("\nmmap content: %s\n", file_memory); // Mostrar sudoku en memoria
    int k = 0;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            sudokuLayout[i][j] = file_memory[k]; // Almacenar sudoku enb arreglo 2D
            k++;    
        }
    }

    int values[3] = {0, 3, 6};
    int three_validation = 0;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            three_validation += three_X_three(values[i], values[j]); // Verificación de submatrices
        }
    }
    
    if (three_validation == 9) {
        printf("Sudoku Válido\n");

    } else {
        printf("Sudoku Inválido\n");
    }

    pid_t parent = getpid();
    
    pid_t child = fork();
    
    if(child < 0) {
        perror("Error al hacer fork");
        return -1;
        
    } else if (child == 0) { // Hijo
        printf("\nSoy el hijo\n");
        char buffer[10];
        sprintf(buffer, "%d", parent);
        execlp("ps", "ps", "-p",  buffer , "-lLf", NULL);
        
    } else { // Padre
        printf("Soy el padre\n");
        pthread_t ptid;
        int *columns;
        
        if (pthread_create(&ptid, NULL, column_validator, NULL) != 0) {
            perror("Error al crear el hilo");
            return -1;
        }
        
        pthread_join(ptid, (void**)&columns);

        printf("\nProceso que ejecuta el main(): %d\n", parent);

        waitpid(child, NULL, 0);
        
        int *rows = row_validator();
        
        // printf("Rows: %d    Columns: %d\n", *rows, *columns);

        if (*rows == 1 && *columns == 1) {
            printf("\nSudoku Válido\n");
            
        } else {
            printf("\nSudoku Inválido\n");
        }
        
        pid_t child2 = fork();

        if(child2 < 0) {
            perror("Error al hacer el fork 2");
            return -1;
            
        } else if (child2 == 0) { // Segundo hijo
            printf("\nSoy el hijo 2\n");
            char buffer[10];
            sprintf(buffer, "%d", parent);
            execlp("ps", "ps", "-p",  buffer , "-lLf", NULL);

        } else { // Padre de nuevo
            waitpid(child2, NULL, 0);
        }

        free(columns);
        free(rows);
    }

    munmap(file_memory, file_size);
    close(fd);
    return 0;
}