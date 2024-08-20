##
## EPITECH PROJECT, 2024
## make
## File description:
## make
##

SRC	=	main.cpp	\
		my_getnbr.cpp

OBJ	=	$(SRC:.cpp=.o)

NAME	=	shogi

all:	$(OBJ)
	gcc -o $(NAME) $(OBJ)

clean:
	rm -rf $(OBJ)

fclean:	clean
	rm -rf $(NAME)

re:	fclean all

run:	re
	./$(NAME)
