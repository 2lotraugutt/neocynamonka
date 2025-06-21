OBJ:=$(patsubst %.c, %.o, $(wildcard *.c)) $(patsubst %.l, %.yy.o, $(wildcard *.l))
CC=gcc
CFLAGS=-Ofast
CLIB=-lncurses
NAME=ncm

all: $(NAME) 

info:
	@echo == Compiling Neocynamonka ==
	@echo NAME:		$(NAME)
	@echo OBJ:		$(OBJ)
	@echo
	@echo CC:		$(CC)
	@echo CFLAGS:	$(CFLAGS)
	@echo CLIB:		$(CLIB)
	@echo ============================

clean:
	rm -f *.o *.yy.c $(NAME)

$(NAME): ${OBJ} info
	$(CC) $(CFLAGS) ${OBJ} -o $(NAME) $(CLIB)
%.o: %.c info
	$(CC) $(CFLAGS) -c $< -o $@
%.yy.c: %.l info
	lex -o $@ $<

.PHONY: clean info all


