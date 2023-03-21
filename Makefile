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

CC			= c++ -g
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
RM			= rm -rf

FSANITIZE	= # -fsanitize=address

CXXFLAGS    += ${FSANITIZE}

OBJDIR		= build

INCLUDES	:= $(shell find . -name '*.hpp' -type f -exec dirname {} \; | uniq | sed 's/^/-I /' | grep -v Playground)
SOURCES     := $(shell find . -name '*.cpp' -print -type f | grep -v Playground)
HEADERS		:= $(shell find . -name '*.hpp' -print -type f | grep -v Playground)

OBJ			= $(SOURCES:%.cpp=$(OBJDIR)/%.o)

# Colors:
GREEN		=	\e[92;5;118m
YELLOW		=	\e[93;5;226m
GRAY		=	\e[33;2;37m
PURPLE		=	\e[35;5;141m
BLUE		=	\e[34;5;75m
WHITE		=	\e[97;5;255m
RED			=	\e[91;5;196m
RESET		=	\e[0m

# Symbols:
SUCCESS		=	\e[92;5;118m[ SUCCESS ]\e[0m
ERROR		=	\e[91;5;196m[ ERROR ]\e[0m
WARNING		=	\e[93;5;226m[ WARNING ]\e[0m
INFO		=	${PURPLE}[ INFO ]${RESET}

# Styles:
CURSIVE		=	\e[33;3m
BOLD		=	\e[33;1m
UNDERLINE	=	\e[33;4m
CROSS		=	\e[33;9m


#Debug
ifeq ($(DEBUG), 1)
   OPTS = -g
endif

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ) $(HEADERS)
	@$(CC) $(INCLUDES) $(CXXFLAGS) $(OBJ) $(OPTS) -o $(NAME)
	@printf "\n\n$(SUCCESS) $(GREEN)- Executable ready: $(BOLD)$(UNDERLINE)${RED}$(NAME)$(RESET)\n\n"

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	@printf "${CURSIVE}${GRAY}	- Making object file: ${BOLD}${GREEN}%-20s ${RESET}\r" $(notdir $@)
	@$(CC) $(INCLUDES) $(CXXFLAGS) $(OPTS) -c $< -o $@

clean:
	@$(RM) $(OBJDIR) $(OBJ)
	@printf "\n${INFO} - Object files ${BOLD}${UNDERLINE}${WHITE}removed${RESET}.\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "${INFO} - Executable [${BOLD}${UNDERLINE}${RED}${NAME}${RESET}] removed.$(RESET)\n\n"

re: fclean all
