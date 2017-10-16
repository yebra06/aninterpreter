// Programmer Alfredo Yebra Jr.
// Last updated: October 16, 2017

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER 1024
#define DELIM " \n\t"

int main(void) {
    int num_pipes = 0;
    int output_pos = 0;
    int input_pos = 0;
    int tmp_fd = 0;
    int rfd[2] = {};
    int lfd[2] = {};
    int status = 0;
    char* infile = NULL;
    char* outfile = NULL;
    char* cmd = NULL;
    char* token = NULL;
    char** tokens = NULL;
    size_t cmd_len = 0;
    size_t num_tokens = 0;

    do {
        num_pipes = 0;
        output_pos = -1;
        input_pos = -1;
        cmd_len = 0;
        num_tokens = 0;
        tmp_fd = 0;

        // Prompt and get command.
        printf("\nshhh> ");
        cmd = (char*)malloc(BUFFER+1);
        status = getline(&cmd, &cmd_len, stdin);

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
            else if (!strcmp(tokens[i], ">")) {
                output_pos = i+1;
                outfile = malloc(strlen(tokens[output_pos])+1);
                strcpy(outfile, tokens[output_pos]);
            } else if (!strcmp(tokens[i], "<")) {
                input_pos = i+1;
                infile = malloc(strlen(tokens[input_pos])+1);
                strcpy(infile, tokens[input_pos]);
            } else if (!strcmp(tokens[i], "exit"))
                status = -1;

        // Find positions of pipes.
        int pipe_positions[num_pipes];
        for (int i = 0, pipes = 0; i < num_tokens; i++)
            if (!strcmp(tokens[i], "|"))
                pipe_positions[pipes++] = i;
        pipe_positions[num_pipes] = num_tokens;

        // Iterate and exec shell processes.
        for (int i = 0; i <= num_pipes; i++) {
            int argc = 0;
            int start = i > 0 ? pipe_positions[i-1]+1 : 0;
            char *argv[pipe_positions[i]-start+1];

            // Find processes between '|'.
            for (int j = start; j < pipe_positions[i]; j++) {
                char* l = tokens[j];
                if (!strcmp(l, "|")
                    || !strcmp(l, ">")
                    || !strcmp(l, "<")
                    || (input_pos != -1 && !strcmp(l, infile))
                    || (output_pos != -1 && !strcmp(l, outfile)))
                    continue;
                argv[argc] = malloc(strlen(tokens[j])+1);
                strcpy(argv[argc], tokens[j]);
                argc++;
            }
            argv[argc] = NULL;

            // Assert pipe works.
            if (num_pipes > 0 && i != num_pipes) {
                if (pipe(rfd) < 0) {
                    perror("Error calling pipe().");
                    exit(-1);
                }
            }

            // Spawn inner processes.
            pid_t pid = fork();
            if (pid == 0) {
                // Input redirection.
                if (input_pos != -1 && i == 0) {
                    if ((tmp_fd = open(tokens[input_pos], O_RDWR)) < 0) {
                        perror("Error input redirection.");
                        exit(-1);
                    }
                    close(0);
                    dup(tmp_fd);
                    close(tmp_fd);
                }

                // Pipe operations.
                if (num_pipes > 0) {
                    if (i == 0) {
                        close(1);       // Close stdout.
                        dup(rfd[1]);    // Duplicate stdout to write end of pipe.
                        close(rfd[1]);  // Close write end of pipe.
                        close(rfd[0]);  // Close unused read end of pipe.
                    } else if (i > 0 && i < num_pipes) {
                        close(1);       // Close stdout.
                        dup(rfd[1]);    // Duplicate stdout to write end of pipe.
                        close(rfd[1]);  // Close write end of pipe.
                        close(rfd[0]);  // Close unused read end of pipe.
                        close(0);       // Close stdin.
                        dup(rfd[0]);    // Duplicate stdin to read end of pipe.
                        close(rfd[0]);  // Close read end of pipe.
                        close(rfd[1]);  // Close unused read end of pipe.
                    } else if (i == num_pipes) {
                        close(0);       // Close stdin.
                        dup(rfd[0]);    // Duplicate stdin to read end of pipe.
                        close(rfd[0]);  // Close read end of pipe.
                        close(rfd[1]);  // Close unused write end of pipe.
                    }
                }

                // Output redirection.
                if (output_pos != -1 && i == num_pipes) {
                    tmp_fd = creat(tokens[output_pos], 0750);
                    close(1);
                    dup(tmp_fd);
                    close(tmp_fd);
                }

                execvp(argv[0], argv);
            } else if (pid < 0) {
                perror("Error forking.");
                exit(-1);
            }

            // Parent process.
            if (i > 0) {
                close(lfd[0]);
                close(lfd[1]);
            }
            lfd[0] = rfd[0];
            lfd[1] = rfd[1];
            wait(&pid);

            for (int j = 0; j < argc; j++)
                free(argv[j]);
        }
    } while (status != EOF);

    free(outfile);
    free(infile);
    free(tokens);
    free(cmd);
    exit(0);
}
