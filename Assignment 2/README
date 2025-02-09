# Multiplication Using Inter-Process Communication in C

This project demonstrates how to multiply two 4-digit integers by splitting them into two 2-digit parts and performing the multiplication using inter-process communication (IPC) with pipes. A child process is created via `fork()`, and the actual multiplication operations are offloaded to the child, with results sent back to the parent process for final assembly.

## Overview

The multiplication is done using the standard “grade-school” algorithm:

\[
a \times b = (a_1 \times b_1) \times 10000 + (a_1 \times b_2 + a_2 \times b_1) \times 100 + (a_2 \times b_2)
\]

Where:
- For a 4-digit number `a`, the digits are split into `a1` (the first two digits) and `a2` (the last two digits).
- For a 4-digit number `b`, the digits are split into `b1` (the first two digits) and `b2` (the last two digits).

The parent process sends the following multiplication tasks to the child process:
1. Multiply `a1` and `b1` (result scaled by 10000).
2. Multiply `a1` with `b2` and `a2` with `b1` (sum the two results and scale by 100).
3. Multiply `a2` and `b2`.

The results are combined in the parent process to produce the final multiplication result.

## Prerequisites

- **Operating System:** UNIX-based (Linux, macOS, etc.)  
  *The program uses POSIX system calls (`fork()`, `pipe()`, etc.).*
- **Compiler:** GCC (or any standard C compiler)
- **Knowledge:** Basic understanding of C programming and inter-process communication

## Building the Project

To compile the program, use the following command in your terminal:

```bash
gcc -o assignment-2 assignment-2.c
```

## Running the Program

The program requires exactly two command-line arguments, each a 4-digit integer. For example:

```bash
./assignment-2 1234 5678
```

### Expected output:
```bash
Your integers are 1234 5678
Parent (PID 26580): created child (PID 26581) 

### 
# Calculating X 
### 
Parent (PID 26580): Sending 12 to child 
Parent (PID 26580): Sending 56 to child 
Child (PID 26581): Received 12 from parent 
Child (PID 26581): Received 56 from parent 
Child (PID 26581): Sending 672 to parent 
Parent (PID 26580): Received 672 from child 

### 
# Calculating Y 
### 
Parent (PID 26580): Sending 12 to child 
Parent (PID 26580): Sending 78 to child 
Child (PID 26581): Received 12 from parent 
Child (PID 26581): Received 78 from parent 
Child (PID 26581): Sending 936 to parent 
Parent (PID 26580): Received 936 from child 
Parent (PID 26580): Sending 34 to child 
Parent (PID 26580): Sending 56 to child 
Child (PID 26581): Received 34 from parent 
Child (PID 26581): Received 56 from parent 
Child (PID 26581): Sending 1904 to parent 
Parent (PID 26580): Received 1904 from child 

### 
# Calculating Z 
### 
Parent (PID 26580): Sending 34 to child 
Parent (PID 26580): Sending 78 to child 
Child (PID 26581): Received 34 from parent 
Child (PID 26581): Received 78 from parent 
Child (PID 26581): Sending 2652 to parent 
Parent (PID 26580): Received 2652 from child 

1234*5678 == 6720000 + 284000 + 2652 == 7006652
```

If the provided numbers are not valid 4-digit integers, the program prints an error message and exits.

## How It Works

1. **Input Validation and Splitting:**
- The program checks for the correct number of command-line arguments.
- It converts the string arguments into integers and verifies that they are 4-digit numbers.
- Each number is split into two parts:
  - `a1 = a / 100` and `a2 = a % 100`
  - `b1 = b / 100` and `b2 = b % 100`
2. **Setting Up Inter-Process Communication:**
- Two pipes are created:
  - **pipe1:** For sending data from the parent to the child.
  - **pipe2:** For sending the multiplication results from the child back to the parent.
3. **Process Forking and Multiplication:**
- The program forks a child process.
- The child process enters a loop where it:
  - Reads two integers from the parent.
  - Multiplies them.
  - Sends the result back through the pipe.
- The parent process coordinates by sending the appropriate pairs for the multiplications:
  - **Step 1:** Send `a1` and `b1`, then receive the result and scale it by 10000.
  - **Step 2:** Send `a1` and `b2` and then `a2` and `b1`, sum the two results, and scale by 100.
  - **Step 3:** Send `a2` and `b2` and receive the result.
4. **Combining the Results:**
- The parent process combines the scaled results from the three steps to produce the final multiplication result.
- The final result and a breakdown of the calculation are printed to the screen.
  ```bash
  X = (a1 * b1) * 10^4
  Y = (a1 * b2 + a2 * b1) * 10^2
  Z = (a2 * b2)
  Result = X + Y + Z
  ```

## Error Handling

- **Argument Validation:**
  The program checks that exactly two arguments are provided and that they represent valid 4-digit numbers.
- **Pipe and Fork Errors:**
  If a pipe cannot be created or if the fork fails, an error message is printed using perror() and the program exits.

## Acknowledgments

- **Reference Material:**
  The implementation is inspired by examples found on GeeksforGeeks and Wyzant, and by the relevant UNIX manual pages.
  
  [GeeksForGeeks](https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/)
  [Wyzant](https://www.wyzant.com/resources/answers/859625/write-a-c-program-where-two-child-processes-are-created-using-fork)
  [ChatGPT](https://chatgpt.com/share/67a90f19-adc4-8000-8fe5-d1a6d1ba62c3)
