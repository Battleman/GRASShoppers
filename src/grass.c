#include "grass.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>



void hijack_flow(){
    printf("Method hijack: Accepted\n");
}

void split_args(char **args, char *line, size_t n_tok) {
    size_t idx = 0;
    char *token = NULL;

    token = strtok(line, TOKENS_DELIM);
    while (token != NULL && idx < (n_tok - 1)) {
        args[idx++] = token;
        token = strtok(NULL, TOKENS_DELIM);
    }

    args[idx] = NULL;
}

int launch(char *output, int output_len, char **cmd) {
    pid_t pid = 0;
    int status = 0;
    int comm[2];

    /* Creates a pipe between forked proceses */
    if (pipe(comm) == -1) {
        perror("Pipe failed");
        return -1; /* Error */
    }

    /* Forks the command execution */
    pid = fork();

    if (pid == 0) {
        /* Child process */

        /* Redirects stdout to communication link */
        dup2(comm[1], STDOUT_FILENO);

        close(comm[0]);
        close(comm[1]);

        execvp(cmd[0], cmd);

        perror("Execution failed");
        exit(EXIT_FAILURE); /* Forces child to die */
    } else if (pid < 0) {
        /* Error forking */
        perror("Fork failed");
        return -1; /* Error */
    } else {
        /* Parent process */

        /* Waits for child to die (#novaxx) */
        waitpid(pid, &status, 0);

        /* Reads communication from child process on success */
        if (status == 0) {
            read(comm[0], output, output_len);
        }
        output[output_len - 1] = '\0';

        close(comm[0]);
        close(comm[1]);
    }

    return status;
}


int check_args(char **args, char *line, size_t n_tok){
    printf(args[0]);
    if(strcmp(args[0], "exit") == 0){
        line = "Byby";
        printf("By friend");
        return 0;
    } else{
        if(strcmp(args[0], "cd")){
            line = "Ain't gonna cd bro";
            return 0;
        } else{
            return 0;
        }
    }
    return 1;
}