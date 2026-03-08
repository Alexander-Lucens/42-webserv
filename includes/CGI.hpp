
#pragma once

#include <string>
#include <map>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/wait.h>
# include "Request.hpp"
# include "Response.hpp"

Response handle_post_cgi(const Request& request, Response& response, Language lang);
Response handle_get_cgi(const Request& request, Response& response, Language lang);
