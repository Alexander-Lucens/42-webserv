# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akuzmin <akuzmin@student.42berlin.de>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/24 14:21:13 by akuzmin           #+#    #+#              #
#    Updated: 2026/01/18 20:45:45 by akuzmin          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := webserv

# **************************************************************************** #

INCLUDE     := includes

SRC_DIR		:= srcs

SRCS        :=  $(SRC_DIR)/main.cpp \
				$(SRC_DIR)/shared/Request.cpp $(SRC_DIR)/shared/Response.cpp \

# **************************************************************************** #

CFLAGS      := -Wall -Wextra -Werror -std=c++98 -I$(INCLUDE)
CC          := c++
RM          := rm -rf

OBJ_DIR      := objs

OBJS        := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# **************************************************************************** #

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Linking $(NAME)..."
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "Done."

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
