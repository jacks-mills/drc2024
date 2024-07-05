from sys import stdout, stderr
from posix_ipc import *
from time import sleep
import struct

STDOUT_PREFIX = "driv: "
STDOUT_PREFIX = "driv: "

SNDR_NONE = 0
SNDR_VISN = 1
SNDR_USND = 2
SNDR_CONT = 3
SNDR_DRIV = 4

SNDR_UNKN_STR = "unknown"
SNDR_VISN_STR = "visn"
SNDR_USND_STR = "usnd"
SNDR_CONT_STR = "cont"
SNDR_DRIV_STR = "driv"

QUEUE_READ_NAME = "/DRC-DRIV-DATA"
QUEUE_READ_SIZE = 64
QUEUE_READ_ELMS = 10

MS_PER_PERC_SPEED_CHANGE = 500


def main():
    readQueue = open_read_queue(QUEUE_READ_NAME)
    print(STDOUT_PREFIX + f"Opened queue \"{QUEUE_READ_NAME}\"")
    stdout.flush()

    state = {
        "turningAngle": 90,
        "setSpeed":     0,
        "realSpeed":    1
    }
    while True:
        read_message(state, readQueue, 2)

        print(
            STDOUT_PREFIX + f"State ("
            f"{state['turningAngle']}, "
            f"{state['setSpeed']}, "
            f"{state['realSpeed']})")
        stdout.flush()

        while(state["realSpeed"] != state["setSpeed"]):
            set_angle(state["turningAngle"]);
            set_speed(state["setSpeed"]);

            speedDiff = state["realSpeed"] - state["setSpeed"]
            state["setSpeed"] += int(speedDiff/abs(speedDiff))

            print(
                STDOUT_PREFIX + f"State ("
                f"{state['turningAngle']:>3}, "
                f"{state['setSpeed']:>3}, "
                f"{state['realSpeed']:>3})")
            stdout.flush()

            sleep(MS_PER_PERC_SPEED_CHANGE / 1000)
            read_message(state, readQueue, 0)


def open_read_queue(qName):
    return MessageQueue(
        qName,
        flags = O_CREAT,
        mode = 0o700,
        max_messages = QUEUE_READ_ELMS,
        max_message_size = QUEUE_READ_SIZE,
        read = True,
        write = False)


def get_sender_str(sender):
    senderToStr = [
        SNDR_UNKN_STR,
        SNDR_VISN_STR,
        SNDR_USND_STR,
        SNDR_CONT_STR,
        SNDR_DRIV_STR]

    try:
        return senderToStr[sender]
    except IndexError:
        return SNDR_UNKN_STR


def read_message(state, readQueue, timeout):
    try:
        message = readQueue.receive(timeout)
    except BusyError:
        return

    sender, realSpeed, turningAngle, = struct.unpack("iii", message)
    print(STDOUT_PREFIX + f"Received message from {get_sender_str(sender)}")
    stdout.flush()

    state["setSpeed"] = realSpeed
    state["turningAngle"] = turningAngle


def set_angle(angle):
    return


def set_speed(speed):
    return


if (__name__ == "__main__"):
    main()
