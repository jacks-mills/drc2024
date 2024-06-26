#include <stdio.h>
#include <unistd.h>
#include <sched.h>

#include "control.h"

static void control_proc();

pid_t control_create_proc(int priority) {
    pid_t child = fork();
    if (child != 0) {
        // parent process
        return child;
    }
    //child process
    struct sched_param param;
    param.sched_priority = priority;

    int error = sched_setscheduler(0, SCHED_FIFO, &param);
    if (error == -1) {
        return -1;
    }

    control_proc();

    return -1; // should never reach here
}

static void control_proc() {
}
