#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_TRANSACTIONS 1000 // Define the maximum number of transactions that can be stored for each thread
#define MAX_LINE_LEN 256 // Define the maximum length of a line in the input file
#define NUM_ACCOUNTS 4 // Define the number of accounts
#define NUM_THREADS 4 // Define the number of threads that can be created

// Structure to represent a single transaction
typedef struct {
    int thread_id; // ID of the thread that processed this transaction
    int account; // Account number (1-based index)
    int amount; // Amount to deposit or withdraw (positive for deposit, negative for withdrawal)
} Transaction;

// Structure to hold data for each thread, including its transactions (not directly used in this revised version for processing)
typedef struct {
    int thread_id; // ID of the thread (1-based index)
    int transaction_count; // Number of transactions for this thread
    Transaction transactions[MAX_TRANSACTIONS]; // Array to hold transactions for this thread
} ThreadData;

// Shared structure to hold all transactions
typedef struct {
    int count;
    Transaction *transactions;
} TransactionList;

TransactionList all_transactions; // Global variable to hold all transactions
int accounts[NUM_ACCOUNTS]; // Array to hold the balances of each account
pthread_mutex_t account_locks[NUM_ACCOUNTS]; // Mutex locks for each account to ensure thread safety

// Function to process transactions for a thread
void *process_transactions(void *arg) {
    int thread_id = *(int *)arg; // Get the thread ID
    for (int i = 0; i < all_transactions.count; i++) { // Iterate through each transaction for this thread
        Transaction *t = &all_transactions.transactions[i]; // Get a pointer to the current transaction
        int acc_index = t->account - 1; // Convert 1-based account number to 0-based index

        pthread_mutex_lock(&account_locks[acc_index]); // Acquire the mutex lock for the account involved in the transaction. Used https://www.geeksforgeeks.org/multithreading-in-c/ for reference

        if (accounts[acc_index] + t->amount < 0) { // Check if the transaction would result in a negative balance
            printf("Thread %d: Withdraw $%d from Account %d - INSUFFICIENT FUNDS\n",
                thread_id, -t->amount, t->account);
        } else {
            accounts[acc_index] += t->amount; // If sufficient funcs or it's a deposit, update the account balance
            printf("Thread %d: %s $%d %s Account %d\n",
                   thread_id,
                   t->amount > 0 ? "Deposit" : "Withdraw",
                   abs(t->amount),
                   t->amount > 0 ? "into" : "from",
                   t->account);
        }

        pthread_mutex_unlock(&account_locks[acc_index]); // Release the mutex lock for the account
    }
    return NULL; // Return NULL to indicate that the thread has finished processing
}

int main(int argc, char *argv[]) {
    if (argc != 2) { // Check if the correct number of arguments is provided
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return EXIT_FAILURE; // Exit if the number of arguments is incorrect
    }

    FILE *file = fopen(argv[1], "r"); // Used https://www.geeksforgeeks.org/basics-file-handling-c/ and https://www.tutorialspoint.com/cprogramming/c_file_io.htm for reference
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    char line[MAX_LINE_LEN]; // Buffer to hold each line read from the file

    fgets(line, sizeof(line), file); // Read the first line (header) and ignore it

    // Read account balances. // Used https://www.geeksforgeeks.org/basics-file-handling-c/ and https://www.tutorialspoint.com/cprogramming/c_file_io.htm for reference
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        if (fgets(line, sizeof(line), file) == NULL) { // Check if the line was read successfully
            fprintf(stderr, "Error reading account balances\n");
            fclose(file);
            return EXIT_FAILURE;
        }
        int acc_id, balance; // Variables to hold account ID and balance
        if (sscanf(line, "-,%d,%d", &acc_id, &balance) == 2 && acc_id >= 1 && acc_id <= NUM_ACCOUNTS) { // Parse the line to get account ID and balance
            accounts[acc_id - 1] = balance;
        } else {
            fprintf(stderr, "Invalid balance line: %s", line);
        }
    }

    // Print starting balances
    printf("Balance:\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Account %d: $%d\n", i + 1, accounts[i]);
    }
    printf("\n");

    // Initialize the all_transactions list
    all_transactions.count = 0;
    all_transactions.transactions = NULL;
    int transaction_index = 0;

    // Read transactions and store them in the all_transactions list. // Used https://www.geeksforgeeks.org/basics-file-handling-c/ and https://www.tutorialspoint.com/cprogramming/c_file_io.htm for reference
    while (fgets(line, sizeof(line), file) != NULL) {
        int thread_id, account, amount; // Variables to hold thread ID, account number, and transaction amount
        if (sscanf(line, "%d,%d,%d", &thread_id, &account, &amount) == 3) { // Parse the line to get thread ID, account number, and amount
            if (thread_id >= 1 && thread_id <= NUM_THREADS && account >= 1 && account <= NUM_ACCOUNTS) { // Check if thread ID and account number are valid
                all_transactions.count++;
                all_transactions.transactions = (Transaction *)realloc(all_transactions.transactions, all_transactions.count * sizeof(Transaction));
                if (all_transactions.transactions == NULL) {
                    perror("Error reallocating memory for transactions");
                    fclose(file);
                    return EXIT_FAILURE;
                }
                all_transactions.transactions[all_transactions.count - 1].thread_id = thread_id;
                all_transactions.transactions[all_transactions.count - 1].account = account;
                all_transactions.transactions[all_transactions.count - 1].amount = amount;
            } else {
                fprintf(stderr, "Invalid transaction data: %s", line);
            }
        } else {
            fprintf(stderr, "Malformed transaction line: %s", line);
        }
    }

    fclose(file);

    // Init mutexes. Used https://www.geeksforgeeks.org/multithreading-in-c/, https://www.tutorialspoint.com/multithreading-in-c#:~:text=A%20multithreaded%20program%20contains%20two,in%20support%20for%20multithreaded%20applications, and https://www.ibm.com/docs/en/i/7.3?topic=ssw_ibm_i_73/apis/rzah4mst.html for reference
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_init(&account_locks[i], NULL);
    }

    // Create threads. Used https://www.geeksforgeeks.org/multithreading-in-c/, https://www.tutorialspoint.com/multithreading-in-c#:~:text=A%20multithreaded%20program%20contains%20two,in%20support%20for%20multithreaded%20applications, and https://www.ibm.com/docs/en/i/7.3?topic=ssw_ibm_i_73/apis/rzah4mst.html for reference
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1; // 1-based for printing
        if (pthread_create(&threads[i], NULL, process_transactions, &thread_ids[i]) != 0) { // Create a thread for each thread data that will execute the 'process_transactions' function
            perror("Error creating thread");
            return EXIT_FAILURE;
        }
    }

    // Join threads. Used https://www.geeksforgeeks.org/multithreading-in-c/, https://www.tutorialspoint.com/multithreading-in-c#:~:text=A%20multithreaded%20program%20contains%20two,in%20support%20for%20multithreaded%20applications, and https://www.ibm.com/docs/en/i/7.3?topic=ssw_ibm_i_73/apis/rzah4mst.html for reference
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy mutexes. Used https://www.geeksforgeeks.org/multithreading-in-c/, https://www.tutorialspoint.com/multithreading-in-c#:~:text=A%20multithreaded%20program%20contains%20two,in%20support%20for%20multithreaded%20applications, and https://www.ibm.com/docs/en/i/7.3?topic=ssw_ibm_i_73/apis/rzah4mst.html for reference
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&account_locks[i]);
    }

    // Final balances
    printf("\nBalance:\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Account %d: $%d\n", i + 1, accounts[i]);
    }

    free(all_transactions.transactions);

    return EXIT_SUCCESS;
}