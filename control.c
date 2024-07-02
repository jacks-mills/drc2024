#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <semaphore.h>

#define STDOUT_PREFIX "cont: "
#define STDERR_PREFIX "cont: "

#define QUEUE_CONT_VISN_DATA_NAME "/DRC-CONT-VISN-DATA"
#define QUEUE_CONT_VISN_DATA_ELMS 10
#define QUEUE_CONT_VISN_DATA_SIZE 64

#define SEMAPHORE_CONT_VISN_DATA_NAME "/DRC-CONT-DATA"

#define X_MIN 0
#define X_MAX 400
#define Y_MIN 0
#define Y_MAX 200

struct VisionData {
    int type;
    int x;
    int y;
};


int main(int argc, char **argv) {
    struct mq_attr attr;
    attr.mq_maxmsg = QUEUE_CONT_VISN_DATA_ELMS; 
    attr.mq_msgsize = QUEUE_CONT_VISN_DATA_SIZE; 

//    sem_t *dataReceived = sem_open(
//        SEMAPHORE_CONT_VISN_DATA_NAME,
//        O_CREAT,
//        S_IRWXU,
//        0);

    mqd_t visionQueue = mq_open(
        QUEUE_CONT_VISN_DATA_NAME, 
        O_CREAT | O_RDONLY,
        S_IRWXU,
        &attr);

    if (visionQueue == (mqd_t) -1) {
        perror(STDERR_PREFIX "mq_open failed");
        exit(EXIT_FAILURE);
    }

    printf(STDOUT_PREFIX "opened \"%s\" queue\n", QUEUE_CONT_VISN_DATA_NAME);

    size_t messageLen = 0;
    char message[QUEUE_CONT_VISN_DATA_SIZE];

    while (1) {
        messageLen = mq_receive(visionQueue, message, QUEUE_CONT_VISN_DATA_SIZE, NULL);
        if (messageLen == -1) {
            perror(STDERR_PREFIX "mq_receive failed");
            exit(EXIT_FAILURE);
        }

        struct VisionData data = *((struct VisionData *) message);

        printf(
            STDOUT_PREFIX 
            "From queue \"%s\" received data (%i, %i %i)\n",
            QUEUE_CONT_VISN_DATA_NAME,
            data.type,
            data.x,
            data.y);

        float turnDegree = 2 * (((float) data.x) - X_MIN) / (X_MAX - X_MIN) - 1;
        printf(STDOUT_PREFIX "turning degree: %f\n", turnDegree);
    }

}
