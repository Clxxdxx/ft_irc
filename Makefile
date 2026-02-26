CXX = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -Iinclude

NAME = ircserv


DIRSERVER = src/server
SERVER_NAMES = Server.cpp
SERVER_SRCS = $(addprefix $(DIRSERVER)/, $(SERVER_NAMES))

SRCS = src/main.cpp $(SERVER_SRCS)

OBJ_DIR = objects
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CPPFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re