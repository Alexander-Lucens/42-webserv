#pragma once

#ifndef COLORS_HPP
#define COLORS_HPP

#include <iostream>

#define RESET   "\033[0m"

#define BLACK   "\033[0;30m"
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define BLUE    "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN    "\033[0;36m"
#define WHITE   "\033[0;37m"

#define BBLACK   "\033[1;30m"
#define BRED     "\033[1;31m"
#define BGREEN   "\033[1;32m"
#define BYELLOW  "\033[1;33m"
#define BBLUE    "\033[1;34m"
#define BMAGENTA "\033[1;35m"
#define BCYAN    "\033[1;36m"
#define BWHITE   "\033[1;37m"

#define BG_BLACK   "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"


#define ERROR(text)		std::cerr << BG_RED << " Error " << RESET << " " << RED << text << RESET << std::endl;
	
#define WARNING(text)	std::cerr << BG_YELLOW << " Warning " << RESET << " " << YELLOW << text << RESET << std::endl;

#define INFO(text)		std::cout << BG_CYAN << " Info " << RESET << " " << CYAN << text << RESET << std::endl;

#define SUCCESS(text)		std::cout << BG_GREEN << " Success " << RESET << " " << GREEN << text << RESET << std::endl;

#endif