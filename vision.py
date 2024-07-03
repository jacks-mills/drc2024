from posix_ipc import *
from time import sleep
import struct
import random
import sys

QUEUE_CONT_DATA_NAME = "/DRC-CONT-DATA"
QUEUE_CONT_DATA_ELMS = 16
QUEUE_CONT_DATA_SIZE = 64

SIGNITURE = 1

STDOUT_PREFIX = "visn: "
STDOUT_PREFIX = "visn: "

X_MIN = 0
X_MAX = 400

Y_MIN = 0
Y_MAX = 200


def main():
    q = None
    while (q == None):
        q = open_queue(10, 200)
        print(STDOUT_PREFIX + f"Failed to open queue \"{QUEUE_CONT_DATA_NAME}\"")
        print(STDOUT_PREFIX + f"Retrying...")
    
    print(STDOUT_PREFIX + f"Opened queue \"{QUEUE_CONT_DATA_NAME}\"")

    count = 1
    while True:
        x, y = get_centroid()

        message = create_message(x, y)

        q.send(message)

        print(STDOUT_PREFIX + f"Sent ({x:>3}, {y:>3})")

        count = count + 1
        sleep(1)


def open_queue(attempts, delay):
    while (attempts > 0 or attempts == -1):
        try:
            return MessageQueue(
                QUEUE_CONT_DATA_NAME,
                flags = 0,
                mode = 0o700,
                max_messages = QUEUE_CONT_DATA_ELMS,
                max_message_size = QUEUE_CONT_DATA_SIZE,
                read = False,
                write = True)
        except ExistentialError:
            sleep(delay / 1000)
            if (attempts > 0):
                attempts = attempts - 1
    return None


def get_centroid():
    return random.randint(X_MIN, X_MAX), random.randint(Y_MIN, Y_MAX)


def create_message(x, y):
    return struct.pack("iii", SIGNITURE, x, y)

if (__name__ == "__main__"):
    main()
