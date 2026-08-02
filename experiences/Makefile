NAME = ft_irc
CC = g++
CFLAGS = -std=c++98 -Wall -Wextra -Werror -g3
INCLUDES_DIR= includes
RM = rm -f
SRCS_DIR = srcs
SRCS = $(SRCS_DIR)/client.cpp $(SRCS_DIR)/server.cpp $(SRCS_DIR)/main.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -I$(INCLUDES_DIR) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -I$(INCLUDES_DIR) -c $< -o $@

clean:
	@$(RM) -r $(OBJS)
	@echo "$(MAGENTA)🧹 Cleaning objects...$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(GREEN)🗑️ Cleaning objects and executable...$(RESET)"

re: fclean clean all