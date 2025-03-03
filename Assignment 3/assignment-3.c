#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define SIZE 9

int sudoku[SIZE][SIZE];  // Global 9x9 Sudoku grid
int valid[SIZE * 3] = {0};  // Array to store validation results

typedef struct {
    int row;
    int col;
    int index;
} parameters;

// Function to check if a row is valid
// https://stackoverflow.com/questions/17605898/sudoku-checker-in-python
void* check_row(void* param) {
    parameters* p = (parameters*)param;
    bool seen[SIZE + 1] = {false}; 

    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[p->row][i];
        if (seen[num]) {
            valid[p->index] = 0;
            printf("Thread # %d (row %d) is INVALID\n", p->index + 1, p->row + 1);
            pthread_exit(NULL);
        }
        seen[num] = true;
    }
    valid[p->index] = 1;
    printf("Thread # %d (row %d) is valid\n", p->index + 1, p->row + 1);
    pthread_exit(NULL);
}

// Function to check if a column is valid
void* check_col(void* param) {
    parameters* p = (parameters*)param;
    bool seen[SIZE + 1] = {false};

    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[i][p->col];
        if (seen[num]) {
            valid[p->index] = 0;
            printf("Thread # %d (column %d) is INVALID\n", p->index + 1, p->col + 1);
            pthread_exit(NULL);
        }
        seen[num] = true;
    }
    valid[p->index] = 1;
    printf("Thread # %d (column %d) is valid\n", p->index + 1, p->col + 1);
    pthread_exit(NULL);
}

// Function to check if a 3x3 subgrid is valid
void* check_subgrid(void* param) {
    parameters* p = (parameters*)param;
    bool seen[SIZE + 1] = {false};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int num = sudoku[p->row + i][p->col + j];
            if (seen[num]) {
                valid[p->index] = 0;
                printf("Thread # %d (subgrid %d) is INVALID\n", p->index + 1, p->index - 18 + 1);
                pthread_exit(NULL);
            }
            seen[num] = true;
        }
    }
    valid[p->index] = 1;
    printf("Thread # %d (subgrid %d) is valid\n", p->index + 1, p->index - 18 + 1);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <sudoku_file.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Read Sudoku grid from file
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            fscanf(file, "%d", &sudoku[i][j]);
    
    fclose(file);

    pthread_t threads[SIZE * 3];  
    parameters data[SIZE * 3];  
    int thread_count = 0;

    // Create threads for rows and columns
    // https://github.com/sarmadhashmi/multithreaded-sudoku-validator/blob/master/CSudokuValidator.c
    for (int i = 0; i < SIZE; i++) {
        data[thread_count] = (parameters){.row = i, .col = 0, .index = thread_count};
        pthread_create(&threads[thread_count], NULL, check_row, &data[thread_count]);
        thread_count++;

        data[thread_count] = (parameters){.row = 0, .col = i, .index = thread_count};
        pthread_create(&threads[thread_count], NULL, check_col, &data[thread_count]);
        thread_count++;
    }

    // Create threads for subgrids
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3) {
            data[thread_count] = (parameters){.row = i, .col = j, .index = thread_count};
            pthread_create(&threads[thread_count], NULL, check_subgrid, &data[thread_count]);
            thread_count++;
        }
    }

    // Join all threads
    for (int i = 0; i < SIZE * 3; i++)
        pthread_join(threads[i], NULL);

    // Check final validity
    bool is_valid = true;
    for (int i = 0; i < SIZE * 3; i++) {
        if (valid[i] == 0) {
            is_valid = false;
        }
    }

    if (is_valid)
        printf("%s contains a valid solution\n", argv[1]);
    else
        printf("%s contains an INVALID solution\n", argv[1]);

    return EXIT_SUCCESS;
}
