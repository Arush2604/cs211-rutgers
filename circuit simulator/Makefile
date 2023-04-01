all: simulator

simulator: simulator.c
	gcc -lm -Wall -Werror -fsanitize=address simulator.c -o simulator

clean: 
	rm -rf simulator
