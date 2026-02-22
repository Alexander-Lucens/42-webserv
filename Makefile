# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lkramer <lkramer@student.42berlin.de>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/24 14:21:13 by akuzmin           #+#    #+#              #
#    Updated: 2026/02/22 13:31:17 by lkramer          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := webserv
TEST_NAME	:= runTest

# **************************************************************************** #

INCLUDE     := includes

SRC_DIR		:= srcs

SRCS        :=  $(SRC_DIR)/main.cpp \
                $(SRC_DIR)/Request.cpp $(SRC_DIR)/Response.cpp $(SRC_DIR)/FileHandler.cpp $(SRC_DIR)/Utils.cpp \
                $(SRC_DIR)/Socket.cpp $(SRC_DIR)/EventLoop.cpp $(SRC_DIR)/ConfigParser.cpp \
                $(SRC_DIR)/Connection.cpp $(SRC_DIR)/CGI.cpp $(SRC_DIR)/Logger.cpp $(SRC_DIR)/Server.cpp
                
TEST_SRCS	:= tests/test_response.cpp \
               tests/test_request.cpp \
               tests/test_socket.cpp \
               srcs/Response.cpp \
               srcs/Request.cpp \
			   srcs/ConfigParser.cpp \
			   srcs/Utils.cpp \
			   srcs/Logger.cpp \
			   srcs/Socket.cpp \
			   srcs/Connection.cpp \
			   srcs/CGI.cpp \
			   srcs/FileHandler.cpp \
               srcs/Socket.cpp 

# **************************************************************************** #

CFLAGS      := -Wall -Wextra -Werror -std=c++98 -I$(INCLUDE)
CC          := c++
RM          := rm -rf

OBJ_DIR      := objs
OBJS        := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TESTDIR		:= test_objs
TEST_OBJS	:= $(OBJ_DIR)/tests/test_response.o \
			 	$(OBJ_DIR)/Request.o \
				$(OBJ_DIR)/Response.o \
				$(OBJ_DIR)/ConfigParser.o \
				$(OBJ_DIR)/Utils.o \
				$(OBJ_DIR)/Logger.o \
				$(OBJ_DIR)/Socket.o \
				$(OBJ_DIR)/Connection.o \
				$(OBJ_DIR)/FileHandler.o \
				$(OBJ_DIR)/Socket.o
               

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
