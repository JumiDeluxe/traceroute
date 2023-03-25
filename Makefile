# Julia Matuszewska 324093

CC = gcc
STD = -std=c99
CFLAGS = -Wall -Wextra -Wpedantic -O2

TARGET = traceroute

SRC = $(shell find . -name "*.c")
OBJ = $(patsubst %.c, %.o, $(SRC))

$(TARGET) : $(OBJ)
	$(CC) $(STD) $(FLAGS) -o $(TARGET) $(OBJ)

%.o : %.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	$(RM) traceroute
	$(RM) *.o
	$(RM) *.out