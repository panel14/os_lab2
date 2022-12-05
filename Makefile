FLAGS = -Wall -o
CC = gcc
TARGET = prog
SRC = proc_info_main.c
all: 
	$(CC) $(SRC) $(FLAGS) $(TARGET)
clean:
	rm $(TARGET)
