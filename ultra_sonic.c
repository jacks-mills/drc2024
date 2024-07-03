#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

#define STDOUT_PREFIX "usnd: "
#define STDERR_PREFIX "usnd: "

#define QUEUE_CONT_DATA_NAME "/DRC-CONT-DATA"
#define QUEUE_CONT_DATA_ELMS 16
#define QUEUE_CONT_DATA_SIZE 64

// Distance range used to decided if to send a message
#define MIN_DIST 0
#define MAX_DIST 100

mqd_t open_queue(int attempts, struct timespec *delay);
itn get_distance();

int main(int argc, char **argv) {
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 200000000; // 200ms
    
    mqd_t dataQueue = -1;
    while (dataQueue == (mqd_t) -1) {
        dataQueue = open_queue(10, &delay);
        printf(STDOUT_PREFIX "Failed to open queue \"%s\"\n", QUEUE_CONT_DATA_NAME);
        printf(STDOUT_PREFIX "Retrying\n");
    }

    if (dataQueue == (mqd_t) -1) {
        perror(STDERR_PREFIX "mq_open failed");
        exit(EXIT_FAILURE);
    }

    printf(STDOUT_PREFIX "Opened queue \"%s\"\n", QUEUE_CONT_DATA_NAME);


    srand(time(NULL));
    while(1) {
        int distance = get_distance();
    }
}

mqd_t open_queue(int attempts, struct timespec *delay) {
    mqd_t q;
    while (attempts > 0 || attempts == -1) {
        q = mq_open(QUEUE_CONT_DATA_NAME, O_WRONLY);

        if (q != (mqd_t) -1) {
            return q;
        }

        if (q == (mqd_t) -1 && errno != ENOENT) {
            perror(STDERR_PREFIX "mq_open failed");
            exit(EXIT_FAILURE);
        }

        nanosleep(delay, NULL);
        attempts--;
    }

    return -1;
}
