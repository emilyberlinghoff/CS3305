#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

// Function to check if a number is prime
// Used https://www.geeksforgeeks.org/c-program-to-print-prime-numbers-from-1-to-n/ for reference
int is_prime(int num) {
    if (num < 2) return 0;
    for (int i = 2; i <= sqrt(num); i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

// Function for prime counting and summing
void calculate_primes(int start, int end, int *prime_count, int *prime_sum) {
    *prime_count = 0;
    *prime_sum = 0;
    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            (*prime_count)++;
            (*prime_sum) += i;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <mode> <min> <max>\n", argv[0]);
        return 1;
    } printf("Parent process id: %d\n", getpid());

    // Converting strings to integers using atoi()
    // Used https://www.geeksforgeeks.org/c-atoi-function/ for reference
    char *modeStr = argv[1];
    char *minStr = argv[2];
    char *maxStr = argv[3];

    int mode = atoi(modeStr);
    int min = atoi(minStr);
    int max = atoi(maxStr);

    if (min > max) {
        fprintf(stderr, "Error: Minimum must be less than or equal to Maximum.\n");
        return 1;
    }

    int range = (max - min + 1);
    int step = range / 4;
    int remainder = range % 4;
    int start, end, prime_count, prime_sum;

    if (mode == 0) { // Serial mode
        for (int i = 0; i < 4; i++) {
            start = min + i * step;
            end = (i == 3) ? (min + (i + 1) * step + remainder - 1) : (min + (i + 1) * step - 1);
            calculate_primes(start, end, &prime_count, &prime_sum);
            printf("pid: %d, ppid: %d - Count and sum of prime numbers between %d and %d are %d and %d\n",
                   getpid(), getppid(), start, end, prime_count, prime_sum);
        }
    } else { // Parallel mode
        pid_t pids[4];
        for (int i = 0; i < 4; i++) {
            start = min + i * step;
            end = (i == 3) ? (min + (i + 1) * step + remainder - 1) : (min + (i + 1) * step - 1);
            if ((pids[i] = fork()) == 0) { // Used https://github.com/torvalds/linux/blob/master/kernel/fork.c for reference
                // Child process
                calculate_primes(start, end, &prime_count, &prime_sum);
                printf("pid: %d, ppid: %d - Count and sum of prime numbers between %d and %d are %d and %d\n",
                       getpid(), getppid(), start, end, prime_count, prime_sum);
                exit(0);
            }
        }
        // Parent waits for all child processes
        for (int i = 0; i < 4; i++) {
            waitpid(pids[i], NULL, 0);
        }
    }
    return 0;
}
