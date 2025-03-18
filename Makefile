ifneq ($(OS), Windows_NT)
	SRC = main.c duck.c duil.c
	FLAGS = -lncurses -lm
	INSTALL_DIR = /usr/local/bin
else
	SRC = main.c duck.c duiw.c
endif

all: $(SRC)
	gcc -o duck $^ $(FLAGS)

install: all
	@echo "Installing duck to $(INSTALL_DIR)"
	@install -m 0755 duck $(INSTALL_DIR)
