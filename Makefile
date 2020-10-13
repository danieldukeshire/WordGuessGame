CFLAGS= -Wall
all: word_guess.cpp
	clang++ $(CFLAGS) word_guess.cpp -o word_guess.out
