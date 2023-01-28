# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ael-khni <ael-khni@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/07/24 17:30:02 by ael-khni          #+#    #+#              #
#    Updated: 2023/01/28 21:53:41 by ael-khni         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv

CC			= c++
FLAGS		= # -Wall -Wextra -Werror -std=c++98
RM			= rm -rf

OBJDIR		= build

CONFIG 		=  $(addprefix ./config/, Configuration)
SERVER		=  $(addprefix ./server/, Server)
UTILS		=  $(addprefix ./utils/, )

FILES		= main $(addprefix ./srcs/, $(CONFIG) $(SERVER) $(UTILS) )
HEADERS		= -I ./srcs/config/ -I ./srcs/server/

SRC			= $(FILES:=.cpp)
OBJ			= $(addprefix $(OBJDIR)/, $(FILES:=.o))

#Colors:
GREEN		=	\e[92;5;118m
YELLOW		=	\e[93;5;226m
GRAY		=	\e[33;2;37m
RESET		=	\e[0m
CURSIVE		=	\e[33;3m

#Debug
ifeq ($(DEBUG), 1)
   OPTS = -g
endif

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	@$(CC) $(OBJ) $(OPTS) -o $(NAME)
	@printf "$(_SUCCESS) $(GREEN)- Executable ready.\n$(RESET)"

$(OBJDIR)/%.o: %.cpp $(HEADER)
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) $(OPTS) -c $< -o $@

clean:
	@$(RM) $(OBJDIR) $(OBJ)
	@printf "$(YELLOW)    - Object files removed.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "$(YELLOW)    - Executable removed.$(RESET)\n"

re: fclean all