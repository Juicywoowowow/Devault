CC = gcc
CFLAGS = -Wall -Wextra -I../../include
LDFLAGS = -L../../libs -ldevault -lshims

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = ../../devault

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) ../../libs/libdevault.a ../../libs/libshims.a
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
