#include <stdio.h>

int column_validator(int sudoku[9][9]) {
    int return_value = 0;
    
    for (int i = 0; i < 9; i++) {
        int total = 0;

        for (int j = 0; j < 9; j++) {
            int number = sudoku[j][i] - '0';
            total += number;
        } 

        if (total == 45) {
           return_value += 1;
        }
    }

    return (return_value == 9) ? 1 : 0;
}

int row_validator(int sudoku[9][9]) {
    int return_value = 0;
    
    for (int i = 0; i < 9; i++) {
        int total = 0;

        for (int j = 0; j < 9; j++) {
            int number = sudoku[i][j] - '0';
            total += number;
        } 

        if (total == 45) {
           return_value += 1;
        }
    }

    return (return_value == 9) ? 1 : 0;
}

int three_X_three(int sudoku[9][9], int row, int column) {
    int total = 0;
    int seen[10] = {0};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int number = sudoku[row + i][column + j] - '0';
            if (number < 1 || number > 9 || seen[number]) {
                return 0;
            }
            seen[number] = 1;
            total += number;
        }
    }

    return (total == 45) ? 1 : 0;
}