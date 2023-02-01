CC 		= g++
CFLAGS += -Wall -Werror -Wextra
LDFLAGS += -lm
INCLUDE = -I./inc

S_DIR	= srcs
INC_HEADERS = $(shell find inc -name "*.hpp")
SRC_HEADERS = $(shell find srcs -name "*.hpp")
FILES 	= $(shell find srcs -name "*.cpp" | cut -d/ -f2-)
B_DIR	= build
OBJ		= $(addprefix $(B_DIR)/, $(FILES:.cpp=.o))
NAME 	= webserve

all: $(NAME) 

$(NAME): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

$(B_DIR)/%.o: $(S_DIR)/%.cpp $(INC_HEADERS) $(SRC_HEADERS)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

clean:
	rm -f $(OBJ)
	rm -rf $(B_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
