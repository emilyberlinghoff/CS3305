#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void handle_error(const char *msg) { // Helper function to handle errors and exit
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 3) { // Check if the number of arguments is correct
        fprintf(stderr, "Usage: %s <4-digit integer> <4-digit integer>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Convert command-line arguments to integers
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);

    if (a < 1000 || a > 9999 || b < 1000 || b > 9999) { // Check if the numbers are 4-digit integers
        fprintf(stderr, "Come on Beldman. You can do better than this.\n");
        return EXIT_FAILURE;
    }

    // Split numbers into two 2-digit parts
    int a1 = a / 100, a2 = a % 100;
    int b1 = b / 100, b2 = b % 100;

    printf("Your integers are %d %d\n", a, b);

    int pipe1[2], pipe2[2];  // Parent to Child, Child to Parent pipes
    // Used https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/ and https://www.wyzant.com/resources/answers/859625/write-a-c-program-where-two-child-processes-are-created-using-fork for reference (as well as man pages)
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        handle_error("Pipe failed");
    }

    pid_t pid = fork(); // Fork a child process
    if (pid < 0) {
        handle_error("Fork failed");
    }

    if (pid == 0) {  // Child process
        close(pipe1[1]); // Close the unused write end of parent-to-child pipe
        close(pipe2[0]); // Close the unused read end of child-to-parent pipee

        int x, y, result;

        for (int i = 0; i < 4; i++) { // Child process handles 4 multiplication steps
            // Read two numbers from the parent
            read(pipe1[0], &x, sizeof(int));
            read(pipe1[0], &y, sizeof(int));

            printf("Child (PID %d): Received %d from parent\n", getpid(), x);
            printf("Child (PID %d): Received %d from parent\n", getpid(), y);

            result = x * y;
            printf("Child (PID %d): Sending %d to parent\n", getpid(), result);
            write(pipe2[1], &result, sizeof(int)); // Send the result to the parent
        }

        close(pipe1[0]); // Close read end of parent-to-child pipe
        close(pipe2[1]); // Close write end of child-to-parent pipe
        exit(EXIT_SUCCESS);

    } else {  // Parent process
        close(pipe1[0]); // Close read end of parent-to-child pipe
        close(pipe2[1]); // Close write end of child-to-parent pipe

        printf("Parent (PID %d): created child (PID %d)\n", getpid(), pid);

        int A, B, C, D, X, Y, Z, result;

        // Send a1, b1 to child
        printf("\n###\n# Calculating X\n###\n");
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a1);
        write(pipe1[1], &a1, sizeof(int));
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b1);
        write(pipe1[1], &b1, sizeof(int));
        read(pipe2[0], &A, sizeof(int));
        printf("Parent (PID %d): Received %d from child\n", getpid(), A);
        X = A * 10000;

        // Send a1, b2 to child
        printf("\n###\n# Calculating Y\n###\n");
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a1);
        write(pipe1[1], &a1, sizeof(int));
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b2);
        write(pipe1[1], &b2, sizeof(int));
        read(pipe2[0], &C, sizeof(int));
        printf("Parent (PID %d): Received %d from child\n", getpid(), C);

        // Send a2, b1 to child
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a2);
        write(pipe1[1], &a2, sizeof(int));
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b1);
        write(pipe1[1], &b1, sizeof(int));
        read(pipe2[0], &B, sizeof(int));
        printf("Parent (PID %d): Received %d from child\n", getpid(), B);

        Y = (B + C) * 100;

        // Send a2, b2 to child
        printf("\n###\n# Calculating Z\n###\n");
        printf("Parent (PID %d): Sending %d to child\n", getpid(), a2);
        write(pipe1[1], &a2, sizeof(int));
        printf("Parent (PID %d): Sending %d to child\n", getpid(), b2);
        write(pipe1[1], &b2, sizeof(int));
        read(pipe2[0], &D, sizeof(int));
        printf("Parent (PID %d): Received %d from child\n", getpid(), D);

        Z = D;

        close(pipe1[1]); // Close write end of parent-to-child pipe
        close(pipe2[0]); // Close read end of child-to-parent pipe

        wait(NULL); // Wait for child to finish

        result = X + Y + Z;
        printf("\n%d*%d == %d + %d + %d == %d\n", a, b, X, Y, Z, result);
    }

    return EXIT_SUCCESS;
}
