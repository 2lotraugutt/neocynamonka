OBJ=$(patsubst %.c, %.o , $(wildcard *.c))
CC=gcc
CFLAGS=-Ofast -lncurses
NAME=neocynamonka

all: $(NAME)

$(NAME): ${OBJ}
	$(CC) $(CFLAGS) ${OBJ} -o $@ -lncurses

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean: 
	rm -f *.o $(NAME)
	
