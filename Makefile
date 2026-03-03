# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lkramer <lkramer@student.42berlin.de>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/24 14:21:13 by akuzmin           #+#    #+#              #
#    Updated: 2026/03/03 11:30:07 by lkramer          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := webserv

# **************************************************************************** #

INCLUDE     := includes

SRC_DIR		:= srcs

SRCS        :=  $(SRC_DIR)/main.cpp \
                $(SRC_DIR)/Request.cpp $(SRC_DIR)/Response.cpp $(SRC_DIR)/FileHandler.cpp $(SRC_DIR)/Utils.cpp \
                $(SRC_DIR)/Socket.cpp $(SRC_DIR)/EventLoop.cpp $(SRC_DIR)/ConfigParser.cpp \
                $(SRC_DIR)/Connection.cpp $(SRC_DIR)/CGI.cpp $(SRC_DIR)/Logger.cpp $(SRC_DIR)/Server.cpp \
				$(SRC_DIR)/ResponseTemplates.cpp \
				$(SRC_DIR)/file_path_check.cpp $(SRC_DIR)/validate_request_by_configuration.cpp


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

$(OBJ_DIR)/%.o: $(SRC_DIR)/*/%.cpp
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
