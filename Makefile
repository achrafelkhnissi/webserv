
CC 		= g++
CFLAGS += -Wall -Werror -Wextra -std=c++98 -g
LDFLAGS += -lm
#INCLUDE = -I inc -I $(shell find srcs -name "*.hpp" -exec dirname {} \; | uniq)
INCLUDE = $(addprefix -I , inc $(addprefix srcs/, server config utils request))

S_DIR	= srcs
INC_HEADERS = $(shell find inc -name "*.hpp")
SRC_HEADERS = $(shell find srcs -name "*.hpp")
FILES 	= $(shell find srcs -name "*.cpp" | cut -d/ -f2-)
B_DIR	= build
OBJ		= $(addprefix $(B_DIR)/, $(FILES:.cpp=.o))
NAME 	= webserv

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

$(B_DIR)/%.o: $(S_DIR)/%.cpp $(INC_HEADERS) $(SRC_HEADERS)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

run:
	./$(NAME) # ./config/webserv.conf

clean:
	rm -f $(OBJ)
	rm -rf $(B_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
