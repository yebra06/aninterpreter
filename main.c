// Programmer Alfredo Yebra Jr.
// Last updated: October 11, 2017

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER 1024
#define DELIM " \n\t"

int main(int argc, char** argv) {
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
        printf("%scmd\n", cmd);

        // Tokenize command.
        tokens = malloc(BUFFER * sizeof(char*));
        token = strtok(cmd, DELIM);
        while (token != NULL) {
            tokens[num_tokens++] = token;
            token = strtok(NULL, DELIM);
        }
        tokens[num_tokens] = NULL;
    }

    return 0;
}

