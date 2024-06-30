#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define STDOUT_PREFIX "control: "
#define STDERR_PREFIX "control: "

#define QUEUE_CONT_VISN_DATA_NAME "/DRC-CONT-VISN-DATA"
#define QUEUE_CONT_VISN_DATA_ELMS 10
#define QUEUE_CONT_VISN_DATA_SIZE 64


int main(int argc, char **argv) {
    struct mq_attr attr;
    attr.mq_maxmsg = QUEUE_CONT_VISN_DATA_ELMS; 
    attr.mq_msgsize = QUEUE_CONT_VISN_DATA_SIZE; 

    mqd_t visionQueue = mq_open(
        QUEUE_CONT_VISN_DATA_NAME, 
        O_CREAT | O_RDONLY,
        S_IRWXU,
        &attr);

    if (visionQueue == (mqd_t) -1) {
        perror(STDERR_PREFIX "mq_open failed");
        exit(EXIT_FAILURE);
    }

    size_t messageLen = 0;
    char message[QUEUE_CONT_VISN_DATA_SIZE];

    while (1) {
        messageLen = mq_receive(visionQueue, message, QUEUE_CONT_VISN_DATA_SIZE, NULL);
        if (messageLen == -1) {
            perror(STDERR_PREFIX "mq_receive failed");
            exit(EXIT_FAILURE);
        }

        printf(
            STDOUT_PREFIX 
            "Received message \"%s\" from queue \"%s\"\n", 
            message, 
            QUEUE_CONT_VISN_DATA_NAME);
    }

}
