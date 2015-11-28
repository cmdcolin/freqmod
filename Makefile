all: freqmod.c
	gcc -o freqmod freqmod.c -l ncurses
clean: freqmod
	rm freqmod
