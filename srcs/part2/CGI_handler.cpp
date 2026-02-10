# include "Response.hpp"
# include "Request.hpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>

Response Response::handle_post(const Request& request)
{
	if (!FileHandler::validate_content_length(request))
		return handle_error(400);

	if (request.path.find("/submit") != std::string::npos)
		return handle_post_submit(request);

	if (request.path.find("/upload") != std::string::npos)
		return handle_post_upload(request);

    if (request.path.find(".py") != std::string::npos || (request.path.find(".cgi") != std::string::npos))
        return handle_post_cgi(request);

	return handle_error(405);
}

Response Response::handle_post_cgi(const Request& request) {
    int     pipefd_in[2];
    int     pipefd_out[2];
    int     pid;
	int		exit_status;
	int		already_written = 0;
    char    buffer[1024];
    ssize_t bytes_read;
	std::string header;
    std::string output;

	if (pipe(pipefd_in) == -1)
		return handle_error(500);
    
	if (pipe(pipefd_out) == -1)
		return handle_error(500);


    pid = fork();
    if (pid == -1)
	{
		close(pipefd_in[1]);
		close(pipefd_out[0]);
		close(pipefd_in[0]);
		close(pipefd_out[1]);
		return handle_error(500);
	}

	if (pid == 0) // child
	{
		// closing unused pipes
        close(pipefd_in[1]);
		close(pipefd_out[0]);

		// redirecting pipes to stdin and stdout
        dup2(pipefd_in[0], STDIN_FILENO);
        dup2(pipefd_out[1], STDOUT_FILENO);

		 // closing fds that have been duplicated
        close(pipefd_in[0]);
		close(pipefd_out[1]);

        
		// write loop to send in ALL of the headers (which means I need a HTTP_HEADER transformer) all the time

		clearenv(); // start clean for safety - no injection possible
		setenv("REQUEST_METHOD", request.method.c_str(), 1);
		setenv("SCRIPT_NAME", request.path.c_str(), 1);
		setenv("QUERY_STRING", request.query_string.c_str(), 1);
		setenv("SERVER_PROTOCOL", request.version.c_str(), 1);

		std::map<std::string, std::string> headers = request.getAllHeaders();

		for (std::map<std::string, std::string>::iterator it = headers.begin() ; it != headers.end() ; ++it)
		{
			header = "HTTP_" + Utils::upper_case(it->first);
			setenv(header.c_str(), it->second.c_str(), 1);
		}
		execl("/opt/pyenv/shims/python3", "python3", request.path.c_str(), NULL); // 0 is path to Python instal, 1 is version, 2 is the script on server

		exit(1);
    }

    // parent

	// closing unused pipes
	close(pipefd_in[0]);
	close(pipefd_out[1]);

	while (already_written < request.body.size()) {
		bytes_read = ::write(pipefd_in[1], request.body.data() + already_written, request.body.size() - already_written);
		already_written += bytes_read;
		if (bytes_read <= 0)
			break;
	}

	// finished sending the job to Python
    close(pipefd_in[1]);

    while ((bytes_read = ::read(pipefd_out[0], buffer, sizeof(buffer))) > 0) {
        output.append(buffer, bytes_read);
	}

	close(pipefd_out[0]);
	waitpid(pid, &exit_status, 0);

    return;
}
