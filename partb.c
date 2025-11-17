#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("USAGE: partb FILEIN FILEOUT\n");
        return 1;
    }

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    int fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0770);
    if (fd < 0) {
        perror("open");
        return 2;
    }

    if (dup2(fd, STDOUT_FILENO) < 0) {
        perror("dup2");
        close(fd);
        return 3;
    }

    close(fd);

    // Print CSV header before exec'ing parta
    printf("Category,Count\n");
    fflush(stdout);

    char *parta_args[] = {
        "parta",
        (char *)input_filename,
        NULL
    };

    execv("./parta", parta_args);

    // If we reach here, execv failed
    perror("execv");
    return 4;
}
