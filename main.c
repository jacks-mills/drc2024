#include <stdio.h>
#include <sys/wait.h>

#include "control.h"
#include "vision.h"

int main(int argc, char **argv) {

    pid_t controlPid = control_create_proc(80);
    if (controlPid == -1) {
        perror("conctrol_create_proc() failed");
    }

    // wait for all child processes to terminate
    while (wait(NULL) > -1);

    return 0;
}
