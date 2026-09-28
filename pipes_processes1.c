#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main() {
    int pipe1[2]; // P1 -> P2
    int pipe2[2]; // P2 -> P1
    pid_t pid;

    char input[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    char result[BUFFER_SIZE];

    // Create pipe from P1 to P2
    if (pipe(pipe1) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create pipe from P2 to P1
    if (pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create child process
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // P1 - Parent Process
    if (pid > 0) {

        // P1 writes to pipe1 and reads from pipe2
        close(pipe1[0]);
        close(pipe2[1]);

        // Get first input from user
        printf("Input : ");
        fflush(stdout);

        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0';

        // Send input from P1 to P2
        write(pipe1[1], input, strlen(input) + 1);

        // Receive completed string from P2
        read(pipe2[0], buffer, BUFFER_SIZE);

        // Add gobison.org
        strncat(buffer, "gobison.org",
                BUFFER_SIZE - strlen(buffer) - 1);

        // Print final output
        printf("Output : %s\n", buffer);

        // Close pipes
        close(pipe1[1]);
        close(pipe2[0]);

        // Wait for child process
        wait(NULL);
    }

    // P2 - Child Process
    else {

        // P2 reads from pipe1 and writes to pipe2
        close(pipe1[1]);
        close(pipe2[0]);

        // Receive string from P1
        read(pipe1[0], buffer, BUFFER_SIZE);

        // Add howard.edu
        strncat(buffer, "howard.edu",
                BUFFER_SIZE - strlen(buffer) - 1);

        // Print output
        printf("Output : %s\n\n", buffer);

        // Get second input from user
        printf("Input : ");
        fflush(stdout);

        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0';

        // Copy the string before appending the second input
        strncpy(result, buffer, BUFFER_SIZE - 1);
        result[BUFFER_SIZE - 1] = '\0';

        // Append second input
        strncat(result, input,
                BUFFER_SIZE - strlen(result) - 1);

        // Send completed string back to P1
        write(pipe2[1], result, strlen(result) + 1);

        // Close pipes
        close(pipe1[0]);
        close(pipe2[1]);

        exit(EXIT_SUCCESS);
    }

    return 0;
}