// Programmer Alfredo Yebra Jr.
// Last updated: October 11, 2017

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER 1024
#define DELIM " \n\t"

int main(void) {
    pid_t pid = 0;
    int num_pipes = 0;
    int output_redir = 0;
    int input_redir = 0;
    int rfd[2] = {};
    int lfd[2] = {};
    char *cmd = NULL;
    char *token = NULL;
    char **tokens = NULL;
    size_t cmd_len = 0;
    size_t num_tokens = 0;

    while (1) {
        pid = 0;
        num_pipes = 0;
        output_redir = -1;
        input_redir = -1;
        cmd_len = 0;
        num_tokens = 0;

        // Prompt and get command.
        // Allocate 1.
        printf("\nshhh> ");
        cmd = (char*)malloc(BUFFER+1);
        getline(&cmd, &cmd_len, stdin);

        // Tokenize command.
        // Allocate 2.
        tokens = malloc(BUFFER * sizeof(char*));
        token = strtok(cmd, DELIM);
        while (token != NULL) {
            tokens[num_tokens++] = token;
            token = strtok(NULL, DELIM);
        }
        tokens[num_tokens] = NULL;

        // Find where to redirect io if necessary and count pipes.
        for (int i = 0; i < num_tokens; i++)
            if (!strcmp(tokens[i], "|"))
                num_pipes++;
            else if (!strcmp(tokens[i], ">"))
                output_redir = i+1;
            else if (!strcmp(tokens[i], "<"))
                input_redir = i+1;
            else if (!strcmp(tokens[i], "exit"))
                exit(0);

        // Find positions of pipes.
        int pipes = 0;
        int pipe_positions[num_pipes];
        for (int i = 0; i < num_tokens; i++)
            if (!strcmp(tokens[i], "|"))
                pipe_positions[pipes++] = i;
        pipe_positions[num_pipes] = num_tokens;

        // Iterate and exec shell processes.
        for (int i = 0, start = 0, argc = 0, end = 0; i <= num_pipes; i++) {
            start = i > 0 ? pipe_positions[i-1]+1 : 0;
            end = pipe_positions[i];
            argc = i > 0 ? end-start : end-start;

            // Find processes between '|'.
            // Allocate 3.
            char *argv[argc];
            for (int j = start, c = 0; j < end; j++, c++) {
                argv[c] = malloc(strlen(tokens[j])+1);
                strcpy(argv[c], tokens[j]);
            }
            argv[argc] = NULL;

            // Assert pipe works.
            if (num_pipes > 0 && i != 0) {
                if (pipe(rfd) == -1) {
                    perror("Error calling pipe().\nTerminating...");
                    exit(-1);
                }
            }

            // Exec shell processes.
            pid = fork();
            if (pid == -1) {
                // Error case.
                perror("Error forking process.\nTerminating...");
                exit(-1);
            } else if (pid == 0) {
                // Child process.
                // TODO: Don't assume input redirection will always be first pipe process.
                if (input_redir != -1 && i == 0) {
                }
                if (output_redir != -1 && i == num_pipes) {
                }
                execvp(argv[0], argv);
            } else {
                // Parent process.
                if (i > 0) {
                    close(lfd[0]);
                    close(lfd[1]);
                }
                lfd[0] = rfd[0];
                lfd[1] = rfd[1];
                wait(&pid);
            }

            // Free 3.
            for (int j = 0; j < argc; j++)
                free(argv[j]);
        }

        // Free 2, 1.
        free(tokens);
        free(cmd);
    }
}


