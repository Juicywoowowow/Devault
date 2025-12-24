CC = gcc
CFLAGS = -Wall -Wextra -I../../include
AR = ar
ARFLAGS = rcs

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = ../../libs/libshims.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
