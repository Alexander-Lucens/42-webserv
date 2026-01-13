# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akuzmin <akuzmin@student.42berlin.de>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/24 14:21:13 by akuzmin           #+#    #+#              #
#    Updated: 2026/01/06 11:56:52 by akuzmin          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := webserv

# **************************************************************************** #

SRCS        :=  srcs/main.cpp \
				srcs/Request.cpp \
				srcs/Response.cpp \
				srcs/Buffer.cpp \
				srcs/Socket.cpp \
				srcs/Connection.cpp \
				srcs/EventLoop.cpp \
				srcs/HTTPParser.cpp \
				srcs/HTTPSerializer.cpp \
				srcs/HTTPConnectionState.cpp \
				srcs/Router.cpp \
				srcs/ServerConfig.cpp \
				srcs/StaticFileHandler.cpp

# **************************************************************************** #

CC          := c++
CFLAGS      := -Wall -Wextra -Werror -std=c++98
RM          := rm -rf

OBJDIR      := objs
OBJS        := $(SRCS:%.cpp=$(OBJDIR)/%.o)

# **************************************************************************** #

all: $(NAME)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

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
