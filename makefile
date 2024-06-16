CC := gcc
CFLAGS := -Wall -pedantic -std=gnu99 -g
TARGET := robot
SRCS := control.c \
	vision.c vision.h \

OBJS := $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))

.PHONY: all clean
    
all: $(TARGET)
    
clean:
	rm -f $(TARGET) $(OBJS)
    
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS)   -o $@ $^
