# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akuzmin <akuzmin@student.42berlin.de>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/24 14:21:13 by akuzmin           #+#    #+#              #
#    Updated: 2026/01/18 20:29:16 by akuzmin          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := webserv

# **************************************************************************** #

SRCS        :=  srcs/main.cpp \
				srcs/shared/Request.cpp srcs/shared/Response.cpp \

# **************************************************************************** #

CC          := c++
CFLAGS      := -Wall -Wextra -Werror -std=c++98
RM          := rm -rf

OBJDIR      := objs
OBJS        := $(SRCS:%.cpp=$(OBJDIR)/%.o)

# **************************************************************************** #

all: $(NAME)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "Creating program $(NAME)."

re: fclean all

clean:
	@$(RM) $(OBJDIR)
	@echo "Deleting object files for program $(NAME)."

fclean: clean
	@$(RM) $(NAME)
	@echo "Deleting program $(NAME)."

.PHONY: all re clean fclean
