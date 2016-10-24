all: freqmod.c
	gcc -o freqmod freqmod.c -lncurses -lm
clean: freqmod
	rm freqmod
