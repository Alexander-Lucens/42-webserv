# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akuzmin <akuzmin@student.42berlin.de>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/24 14:21:13 by akuzmin           #+#    #+#              #
#    Updated: 2026/01/20 15:51:20 by akuzmin          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := webserv
TEST_NAME	:= runTest

# **************************************************************************** #

INCLUDE     := includes

SRC_DIR		:= srcs

SRCS        :=  $(SRC_DIR)/main.cpp \
				$(SRC_DIR)/shared/Request.cpp $(SRC_DIR)/shared/Response.cpp \
				$(SRC_DIR)/part1/Socket.cpp \
				tests/test_request.cpp tests/test_socket.cpp tests/test_response.cpp\
				
TEST_SRCS	:= tests/test_response.cpp \
				srcs/Response.cpp 

# **************************************************************************** #

CFLAGS      := -Wall -Wextra -Werror -std=c++98 -I$(INCLUDE)
CC          := c++
RM          := rm -rf

OBJ_DIR      := objs
OBJS        := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TESTDIR		:= test_objs
TEST_OBJS	:= $(OBJ_DIR)/tests/test_response.o \
			 	$(OBJ_DIR)/srcs/shared/Request.o \
				$(OBJ_DIR)/srcs/shared/Response.o
               

# **************************************************************************** #

all: $(NAME)

test: $(TEST_NAME)

$(NAME): $(OBJS)
	@echo "Linking $(NAME)..."
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "Done."


$(TEST_NAME): $(TEST_OBJS)
	@$(CC) $(CFLAGS) $(TEST_OBJS) -o $(TEST_NAME)
	@echo "Creating test program $(TEST_NAME)."
	@./$(TEST_NAME)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/*/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/tests/%.o: tests/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/srcs/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning objects..."
	@$(RM) $(OBJ_DIR)

fclean: clean
	@echo "Removing $(NAME)..."
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re test
