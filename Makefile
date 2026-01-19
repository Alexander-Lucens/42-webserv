# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lkramer <lkramer@student.42berlin.de>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/24 14:21:13 by akuzmin           #+#    #+#              #
#    Updated: 2026/01/19 13:31:07 by lkramer          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := webserv
TEST_NAME	:= runTest

# **************************************************************************** #

INCLUDE     := includes

SRC_DIR		:= srcs

SRCS        :=  $(SRC_DIR)/main.cpp \
				$(SRC_DIR)/shared/Request.cpp $(SRC_DIR)/shared/Response.cpp \

TEST_SRCS	:= tests/test_response.cpp \
				srcs/Response.cpp \

# **************************************************************************** #

CFLAGS      := -Wall -Wextra -Werror -std=c++98 -I$(INCLUDE)
CC          := c++
RM          := rm -rf

OBJ_DIR      := objs
OBJS        := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TESTDIR		:= test_objs
TEST_OBJS	:= $(TEST_SRCS:%.cpp=$(OBJDIR)/%.o)

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

clean:
	@echo "Cleaning objects..."
	@$(RM) $(OBJ_DIR)

fclean: clean
	@echo "Removing $(NAME)..."
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
