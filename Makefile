# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ael-khni <ael-khni@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/07/24 17:30:02 by ael-khni          #+#    #+#              #
#    Updated: 2023/02/19 09:33:57 by fathjami         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv

CC			= c++
FLAGS		= -g #-Wall -Wextra -Werror -std=c++98
RM			= rm -rf

OBJDIR		= build

CONFIG 		=  $(addprefix ./config/, Configuration)
SERVER		=  $(addprefix ./server/, Server Vserver SubServer)
UTILS		=  $(addprefix ./utils/, )

FILES		= main $(addprefix ./srcs/, $(CONFIG) $(SERVER) $(UTILS) )
INC		= -I ./srcs/config/ -I ./srcs/server/

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

S_DIR	= srcs
INC_HEADERS = $(shell find inc -name "*.hpp")
SRC_HEADERS = $(shell find srcs -name "*.hpp")
FILES 	= $(shell find srcs -name "*.cpp" | cut -d/ -f2-)
B_DIR	= build
OBJ		= $(addprefix $(B_DIR)/, $(FILES:.cpp=.o))
NAME 	= webserv

all: $(NAME) 

$(NAME): $(OBJ) $(HEADER)
	@$(CC) $(INC) $(OBJ) $(OPTS) -o $(NAME)
	@printf "$(_SUCCESS) $(GREEN) - Executable ready.\n$(RESET)"

$(OBJDIR)/%.o: %.cpp $(HEADER)
	@mkdir -p $(dir $@)
	@$(CC) $(INC) $(FLAGS) $(OPTS) -c $< -o $@

clean:
	rm -f $(OBJ)
	rm -rf $(B_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
