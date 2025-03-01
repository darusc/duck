SRC = $(wildcard *.c)

all: $(SRC)
	gcc -o duck $^