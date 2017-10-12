// Programmer Alfredo Yebra Jr.
// Last updated: October 11, 2017

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER 1024
#define DELIM " \n\t"

int main(void) {
    pid_t pid = 0;
    int num_pipes = 0;
    int output_pos = 0;
    int input_pos = 0;
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
        output_pos = -1;
        input_pos = -1;
        cmd_len = 0;
        num_tokens = 0;

        // Prompt and get command.
        printf("\nshhh> ");
        cmd = (char*)malloc(BUFFER+1);
        getline(&cmd, &cmd_len, stdin);

        // Tokenize command.
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
                output_pos = i+1;
            else if (!strcmp(tokens[i], "<"))
                input_pos = i+1;
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
            char *argv[argc];
            for (int j = start, c = 0; j < end; j++, c++) {
                argv[c] = malloc(strlen(tokens[j])+1);
                strcpy(argv[c], tokens[j]);
            }
            argv[argc] = NULL;

            if (num_pipes > 0 && i != num_pipes)
                pipe(rfd);

            pid = fork();
            if (pid > 0) {
                // Parent process.
                if (i > 0) {
                    close(lfd[0]);
                    close(lfd[1]);
                }
                lfd[0] = rfd[0];
                lfd[1] = rfd[1];
                wait(&pid);
            } else {
                // Child process.
                execvp(argv[0], argv);
            }

            for (int j = 0; j < argc; j++)
                free(argv[j]);
        }

        free(tokens);
        free(cmd);
    }

    return 0;
}


