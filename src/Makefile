CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
TARGET = oshell
SRCS = main.c shell.c parser.c builtins.c executor.c utils.c signal.c
OBJS = $(SRCS:.c=.o)
HEADER = shell.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.txt test_output

.PHONY: all clean