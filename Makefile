# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lkramer <lkramer@student.42berlin.de>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/24 14:21:13 by akuzmin           #+#    #+#              #
#    Updated: 2026/01/18 19:39:45 by lkramer          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := webserv
TEST_NAME	:= runTest

# **************************************************************************** #

SRCS        :=  srcs/main.cpp \
				srcs/Response.cpp \

TEST_SRCS	:= tests/test_response.cpp \
				srcs/Response.cpp \


# **************************************************************************** #

CC          := c++
CFLAGS      := -Wall -Wextra -Werror -std=c++98
RM          := rm -rf

OBJDIR      := objs
OBJS        := $(SRCS:%.cpp=$(OBJDIR)/%.o)
TESTDIR		:= test_objs
TEST_OBJS	:= $(TEST_SRCS:%.cpp=$(OBJDIR)/%.o)

# **************************************************************************** #

all: $(NAME)

test: $(TEST_NAME)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)/srcs
	@echo "Creating object directory."

$(TEST_NAME): $(TEST_OBJS)
	@$(CC) $(CFLAGS) $(TEST_OBJS) -o $(TEST_NAME)
	@echo "Creating test program $(TEST_NAME)."
	@./$(TEST_NAME)
	
$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "Creating program $(NAME)."

re: fclean all

clean:
	@$(RM) $(OBJDIR) $(TEST_NAME)
	@echo "Deleting object files for program $(NAME)."

fclean: clean
	@$(RM) $(NAME)
	@echo "Deleting program $(NAME)."

.PHONY: all re clean fclean test
