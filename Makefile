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

# todo: fix the makefile not compiling when editing a header file (tested with Server.hpp)

NAME		= webserv

CC			= c++ -g
CXXFLAGS		= # -Wall -Wextra -Werror -std=c++98
RM			= rm -rf

OBJDIR		= build

INCLUDES	:= $(shell find . -name '*.hpp' -type f -exec dirname {} \; | uniq | sed 's/^/-I /' | grep -v Playground)
SOURCES     := $(shell find . -name '*.cpp' -print -type f | grep -v Playground)

HEADERS		= $($SOURCES:=.hpp)
OBJ			= $(SOURCES:%.cpp=$(OBJDIR)/%.o)

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

$(NAME): $(OBJ) $(HEADERS)
	@$(CC) $(INCLUDES) $(CXXFLAGS) $(OBJ) $(OPTS) -o $(NAME)
	@printf "$(_SUCCESS) $(GREEN)- Executable ready.\n$(RESET)"

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	@$(CC) $(INCLUDES) $(CXXFLAGS) $(OPTS) -c $< -o $@

clean:
	@$(RM) $(OBJDIR) $(OBJ)
	@printf "$(YELLOW)    - Object files removed.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "$(YELLOW)    - Executable removed.$(RESET)\n"

re: fclean all
