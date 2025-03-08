ifneq ($(OS), Windows_NT)
	SRC = main.c duck.c duil.c
	FLAGS = -lncurses -lm
else
	SRC = main.c duck.c duiw.c
endif

all: $(SRC)
	gcc -o duck $^ $(FLAGS)