from posix_ipc import *
from time import sleep
import struct
import random
import sys

QUEUE_CONT_VISN_DATA_NAME = "/DRC-CONT-VISN-DATA"
QUEUE_CONT_VISN_DATA_ELMS = 10
QUEUE_CONT_VISN_DATA_SIZE = 64

STDOUT_PREFIX = "visn: "
STDOUT_PREFIX = "visn: "

X_MIN = 0
X_MAX = 400

Y_MIN = 0
Y_MAX = 200

def main():
    q = MessageQueue(
        QUEUE_CONT_VISN_DATA_NAME,
        flags = O_CREAT,
        mode = 0o700,
        max_messages = QUEUE_CONT_VISN_DATA_ELMS,
        max_message_size = QUEUE_CONT_VISN_DATA_SIZE,
        read = False,
        write = True)

    print(STDOUT_PREFIX + "opened \"" + QUEUE_CONT_VISN_DATA_NAME + "\" queue")

    count = 1
    while True:
        x, y = random.randint(X_MIN, X_MAX), random.randint(Y_MIN, Y_MAX)

        message = struct.pack("iii", 1, x, y)

        q.send(message)

        print(STDOUT_PREFIX + "In queue \"" + QUEUE_CONT_VISN_DATA_NAME + "\" send " + f"({1}, {x}, {y})")

        count = count + 1
        sleep(1)

if (__name__ == "__main__"):
    main()
