#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define STDOUT_PREFIX "cont: "
#define STDERR_PREFIX "cont: "

#define QUEUE_CONT_DATA_NAME "/DRC-CONT-DATA"
#define QUEUE_CONT_DATA_ELMS 16
#define QUEUE_CONT_DATA_SIZE 64

#define SENDER_UNKN "unknown"
#define SENDER_VISN "visn"
#define SENDER_USND "usnd"

#define X_MIN 0
#define X_MAX 400
#define Y_MIN 0
#define Y_MAX 200

struct VisionData {
    int sender;
    int x;
    int y;
};

enum Sender {
    VISN = 1,
    USND
};


char *get_sender(enum Sender s);

int main(int argc, char **argv) {
    struct mq_attr attr;
    attr.mq_maxmsg = QUEUE_CONT_DATA_ELMS; 
    attr.mq_msgsize = QUEUE_CONT_DATA_SIZE; 

    mqd_t dataQueue = mq_open(
        QUEUE_CONT_DATA_NAME, 
        O_CREAT | O_RDONLY,
        S_IRWXU,
        &attr);

    if (dataQueue == (mqd_t) -1) {
        perror(STDERR_PREFIX "mq_open failed");
        exit(EXIT_FAILURE);
    }

    printf(STDOUT_PREFIX "opened queue \"%s\"\n", QUEUE_CONT_DATA_NAME);

    size_t messageLen = 0;
    char message[QUEUE_CONT_DATA_SIZE];

    while (1) {
        messageLen = mq_receive(dataQueue, message, QUEUE_CONT_DATA_SIZE, NULL);
        if (messageLen == -1) {
            perror(STDERR_PREFIX "mq_receive failed");
            exit(EXIT_FAILURE);
        }

        struct VisionData data = *((struct VisionData *) message);

        printf(
            STDOUT_PREFIX 
            "Received (%3i, %3i) from %s\n",
            data.x,
            data.y,
            get_sender(data.sender));
    }

}

char *get_sender(enum Sender s) {
    switch(s) {
        default:
            return SENDER_UNKN;
        case VISN:
            return SENDER_VISN;
        case USND:
            return SENDER_USND;
    }
}

