#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("USAGE: partc FILEIN\n");
        return 1;
    }

    const char *input_filename = argv[1];

    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return 2;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 3;
    }

    if (pid > 0) {
        // Parent: become parta, writing into the pipe
        close(pipefd[0]);  // close read end

        if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
            perror("dup2");
            close(pipefd[1]);
            return 4;
        }
        close(pipefd[1]);

        char *parta_args[] = {
            "parta",
            (char *)input_filename,
            NULL
        };

        execv("./parta", parta_args);
        perror("execv parta");
        return 5;
    } else {
        // Child: become sort, reading from the pipe
        close(pipefd[1]);  // close write end

        if (dup2(pipefd[0], STDIN_FILENO) < 0) {
            perror("dup2");
            close(pipefd[0]);
            _exit(4);
        }
        close(pipefd[0]);

        char *sort_args[] = {
            "sort",
            "-t,",
            "-k2",
            "-n",
            NULL
        };

        execvp("sort", sort_args);
        perror("execvp sort");
        _exit(5);
    }
}
