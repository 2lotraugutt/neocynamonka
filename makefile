OBJ=token.yy.o draw.o hosts.o log.o main.o ping.o pipe.o
CC=gcc
CFLAGS=-Ofast -lncurses
NAME=ncm
all: $(NAME)

$(NAME): ${OBJ} 
	$(CC) $(CFLAGS)  ${OBJ} -o $@ -lncurses

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.yy.c: %.l
	lex -o $@ $<
info:
	@echo "Building $(NAME)"
	@echo "CC:     $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "OBJ:    $(OBJ)"
install: all
	cp ncm /bin
clean: 
	rm -f *.o *.yy.c $(NAME)
