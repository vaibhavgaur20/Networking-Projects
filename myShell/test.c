#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024

int execute_command(char *command) {
    if (command[0] == '/') {
        // Absolute path provided, execute directly
        pid_t pid;
        int status;

        pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) {
            // Child process
            if (execlp(command, command, NULL) == -1) {
                perror("Execution failed");
                exit(1);
            }
        } else {
            // Parent process
            waitpid(pid, &status, 0);
            return WEXITSTATUS(status);
        }
    } else {
        char path[1024];

        // Check if the command exists in /usr/bin
        snprintf(path, sizeof(path), "/usr/bin/%s", command);
        if (access(path, X_OK) == 0) {
            pid_t pid;
            int status;

            pid = fork();

            if (pid < 0) {
                perror("Fork failed");
                exit(1);
            } else if (pid == 0) {
                // Child process
                if (execlp(path, path, NULL) == -1) {
                    perror("Execution failed");
                    exit(1);
                }
            } else {
                // Parent process
                waitpid(pid, &status, 0);
                return WEXITSTATUS(status);
            }
        } else {
            snprintf(path, sizeof(path), "/usr/local/bin/%s", command);
            if (access(path, X_OK) == 0) {
                pid_t pid;
                int status;

                pid = fork();

                if (pid < 0) {
                    perror("Fork failed");
                    exit(1);
                } else if (pid == 0) {
                    // Child process
                    if (execlp(path, path, NULL) == -1) {
                        perror("Execution failed");
                        exit(1);
                    }
                } else {
                    // Parent process
                    waitpid(pid, &status, 0);
                    return WEXITSTATUS(status);
                }
            } else {
                printf("Command '%s' not found in /usr/bin or /usr/local/bin, and it is not an absolute path.\n", command);
                return 1; // Return an error code
            }
        }
    }

    return 0;
}

int main() {
    char input[MAX_INPUT_SIZE];

    while (1) {
        printf("CustomShell> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove the newline character
        size_t input_length = strlen(input);
        if (input_length > 0 && input[input_length - 1] == '\n') {
            input[input_length - 1] = '\0';
        }

        if (strcmp(input, "exit") == 0) {
            break;  // Exit the shell
        } else {
            int exit_status = execute_command(input);
            printf("Exit status: %d\n", exit_status);
        }
    }

    return 0;
}