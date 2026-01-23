##
## EPITECH PROJECT, 2024
## make
## File description:
## make
##

all:
	mkdir -p build
	cmake -S . -B build
	cmake --build build

clean:
	rm -rf build

fclean:	clean
	rm -rf terminalShogi

re:	fclean all

run:	re
	./terminalShogi
