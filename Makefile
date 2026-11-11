CC = gcc
# Enforce all warnings as required by the rubric[cite: 1]
CFLAGS = -Wall -g 
TARGET = mysh


SRCS = CS311_A01_2024149_main.c CS311_A01_2024149_parser.c CS311_A01_2024149_execute.c CS311_A01_2024149_builtins.c CS311_A01_2024149_process_list.c linenoise.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)