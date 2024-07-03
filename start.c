#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    pid_t pid = -1;

    pid = fork();
    if (pid == 0) {
        execl("/usr/bin/chrt", "chrt", "81", "./control", (char*) NULL);
        perror("execl faialed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == 0) {
        execl("/usr/bin/chrt", "chrt", "80", ".venv/drc2024/bin/python", "vision.py", (char*) NULL);
        perror("execl faialed");
        exit(EXIT_FAILURE);
    }

    // wait for all child processes to terminate
    while (wait(NULL) > -1);

    return 0;
}

