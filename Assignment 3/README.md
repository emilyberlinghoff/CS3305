# Multithreaded Sudoku Validator

## Overview
This program checks whether a given 9×9 Sudoku solution is valid using multithreading. It creates 27 threads to validate:
- 9 rows
- 9 columns
- 9 subgrids (3×3 blocks)

Each thread verifies that its assigned row, column, or subgrid contains all numbers from 1 to 9 without repetition.

## Features
- Uses multithreading (pthreads) for efficient parallel validation  
- Detects invalid rows, columns, or subgrids and prints their locations  
- Supports text file input with a formatted 9×9 Sudoku grid  
- Outputs whether the Sudoku solution is valid or invalid  

## Installation & Compilation
### Requirements
- GCC compiler with pthread support
- Unix/Linux or Windows (with MinGW)

### Compiling the Program
Run the following command to compile:
```sh
gcc -pthread -o assignment-3 assignment-3.c
```

## Usage
To check a Sudoku file:
```sh
./assignment-3 <sudoku_file.txt>
```
For example:
```sh
./assignment-3 valid-sudoku-1.txt
```

## Input Format
The input file should contain a 9×9 Sudoku grid, where numbers are space-separated. There are many exmaple fiels included in this directory.

### Example (valid-sudoku.txt)
```
5 3 4 6 7 8 9 1 2
6 7 2 1 9 5 3 4 8
1 9 8 3 4 2 5 6 7
8 5 9 7 6 1 4 2 3
4 2 6 8 5 3 7 9 1
7 1 3 9 2 4 8 5 6
9 6 1 5 3 7 2 8 4
2 8 7 4 1 9 6 3 5
3 4 5 2 8 6 1 7 9
```

### Example (invalid-sudoku.txt)
```
5 3 4 6 7 8 9 1 2
6 7 2 1 9 5 3 4 8
1 9 8 3 4 2 5 6 4  # Invalid: '4' appears twice in row 3
8 5 9 7 6 1 4 2 3
4 2 6 8 5 3 7 9 1
7 1 3 9 2 4 8 5 6
9 6 1 5 3 7 2 8 4
2 8 7 4 1 9 6 3 5
3 4 5 2 8 6 1 7 9
```

## Expected Output
### Valid Sudoku
```sh
Thread # 1 (subgrid 1) is valid
Thread # 2 (subgrid 2) is valid
...
Thread # 27 (column 9) is valid
valid-sudoku.txt contains a valid solution
```

### Invalid Sudoku
```sh
Thread # 12 (row 3) is INVALID
Thread # 23 (column 5) is INVALID
Thread # 27 (subgrid 9) is INVALID
invalid-sudoku.txt contains an INVALID solution
```

## How It Works
- 27 threads are created:
  - 9 for rows (`check_row()`)
  - 9 for columns (`check_col()`)
  - 9 for subgrids (`check_subgrid()`)
- Each thread verifies its section independently
- The main thread waits (`pthread_join`) for all threads to finish
- If any validation fails, the solution is reported INVALID

## **Known Issues & Improvements**
- Print order is non-deterministic (due to multithreading)
- Can be optimized using mutexes or semaphores for synchronization

## Citations
This project was inspired by and references the following sources:

- Sudoku Row Validation Reference:
  [Stack Overflow - Sudoku Checker in Python](https://stackoverflow.com/questions/17605898/sudoku-checker-in-python)  
  This source provided guidance on structuring the logic for checking Sudoku rows, ensuring all numbers from 1 to 9 appear without repetition.

- Multithreaded Sudoku Validator Reference:*
  [GitHub - Multithreaded Sudoku Validator](https://github.com/sarmadhashmi/multithreaded-sudoku-validator/blob/master/CSudokuValidator.c)  
  This repository served as a reference for using pthreads to validate Sudoku grids efficiently through multithreading.

These references helped shape the row validation logic and parallel execution approach in this project.
