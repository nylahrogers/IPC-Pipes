#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int pipe1[2];  // cat -> grep
    int pipe2[2];  // grep -> sort

    pid_t pid2;
    pid_t pid3;

    // Check command-line argument
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <grep argument>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create first pipe
    if (pipe(pipe1) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create second pipe
    if (pipe(pipe2) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create P2 (grep)
    pid2 = fork();

    if (pid2 < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0)
    {
        /*
         * P2 - grep
         */

        // Create P3 (sort)
        pid3 = fork();

        if (pid3 < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid3 == 0)
        {
            /*
             * P3 - sort
             */

            // Input for sort comes from grep
            dup2(pipe2[0], STDIN_FILENO);

            // Close all unused pipe ends
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[0]);
            close(pipe2[1]);

            // Execute sort
            execlp("sort", "sort", (char *)NULL);

            perror("sort");
            exit(EXIT_FAILURE);
        }
        else
        {
            /*
             * P2 - grep
             */

            // Input for grep comes from cat
            dup2(pipe1[0], STDIN_FILENO);

            // Output from grep goes to sort
            dup2(pipe2[1], STDOUT_FILENO);

            // Close all unused pipe ends
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[0]);
            close(pipe2[1]);

            // Execute grep with the command-line argument
            execlp("grep", "grep", argv[1], (char *)NULL);

            perror("grep");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        /*
         * P1 - cat scores
         */

        // Output from cat goes to grep
        dup2(pipe1[1], STDOUT_FILENO);

        // Close all unused pipe ends
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        // Execute cat scores
        execlp("cat", "cat", "scores", (char *)NULL);

        perror("cat");
        exit(EXIT_FAILURE);
    }

    return 0;
}