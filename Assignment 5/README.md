# Multithread Bank Transactions

## Overview

This program simulates bank transactions on four accounts using multiple threads. It reads transaction data from a specified input file, processes these transactions concurrently using four threads, and ensures data consistency through mutual exclusion using mutex locks.

## Compilation

```bash
gcc assignment-5.c -o assignment-5 -pthread
./assignment-5 <input_file.csv>
```

- `gcc`: the name of the GCC compiler
- `assignment-5.c` the source code file
- `-o assignment-5`: specifies the name of the output executable file
- `pthread`: links the pthreads library, which is necessary for using threads

## Input File Format

The program expects a plain text input file as a command-line argument. The file should be formated as follows:

1. **Header Line**: The first line is expected to be a header (e.g., "ATM, Account, Amount") and will be ignored.
2. **Initial Account Balances (Lines 2-5)**: The next four lines specify the initial balance for each of the four accounts. Each line should follow the format:

```bash
-,<account_number>,<initial_balance>
```

where `<account_number>` is 1, 2, 3, or 4, and `<iniital_balance>` is the starting amount for that account

3. **Transactions (Subsequent Lines):** The remaining liens represent the transactions to be processed. Each line should follow the format:

```bash
<thread_id>,<account_number>,<amount>
```

where:
    - `<thread_id>` is an integer between 1 and 4, indicating which thread is assigned this transaction (in this implementation, all threads process all transactions).
    - `<account_number>` is the target account (1, 2, 3, or 4).
    - `<amount>` is the transaction amount. A positive value indicates a deposit, and a negative value indicates a withdrawal.

## Execution

```bash
./assignment-5 <input_file.csv>
```

## Output

The program will produce the following output:

1. **Starting Balances**: The initial balances of all four accounts will be printed
2. **Transaction Processing Messages**: As each of the four threads processes each transaction, a messsage will be printed indicating:
    - The thread ID
    - The type of transaction (Deposit or Withdraw)
    - The amount of the transaction
    - The target account
    - An `INSUFFICIENT FUNDS` message if a withdrawl would result in a negative balance
    **Note**: The order of these transaction messages may vary between runs due to the concurrent nature of the threads.
3. **Ending balances**: After all threads have finsihed processing all transactions, the final balances of all four accoutns will be printed

### Example output

![Example output](link)

## Resources

- [Multithreading in C](https://www.geeksforgeeks.org/multithreading-in-c/)
- [Multithreading in C\n] (https://www.tutorialspoint.com/multithreading-in-c#:~:text=A%20multithreaded%20program%20contains%20two,in%20support%20for%20multithreaded%20applications.)
- [Pthread APIs](https://www.ibm.com/docs/en/i/7.3?topic=ssw_ibm_i_73/apis/rzah4mst.html)
- [Basics of File Handling in C](https://www.geeksforgeeks.org/basics-file-handling-c/)
- [File Handling in C](https://www.tutorialspoint.com/cprogramming/c_file_io.htm)

## Notes

- The `MAX_TRANSACTIONS`, `MAX_LINE_LEN`, `NUM_ACCOUNTS`, and `NUM_THREADS` constants can be adjusted by modifying the `#define` directives at the beginning of the `assignment-5.c` file.
The program assumes that the input file adheres to the specified format. Incorrectly formatted input may lead to unexpected behavior.