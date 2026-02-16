
# include "CGI.hpp"

Response handle_post_cgi(const Request& request, Response& response, Language lang) {
	int				pipefd_in[2];
	int				pipefd_out[2];
	int				pid;
	int				exit_status;
	std::size_t		already_written = 0;
	char			buffer[1024];
	ssize_t			bytes_read;
	std::string		header;
	std::string		output;

	if (pipe(pipefd_in) == -1)
		return response.handle_error(500);

	if (pipe(pipefd_out) == -1)
		return response.handle_error(500);


	pid = fork();
	if (pid == -1)
	{
		close(pipefd_in[1]);
		close(pipefd_out[0]);
		close(pipefd_in[0]);
		close(pipefd_out[1]);
		return response.handle_error(500);
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


		// write loop to send in ALL of the headers all the time

		clearenv(); // start clean for safety - no injection possible
		setenv("REQUEST_METHOD", request.method.c_str(), 1);
		setenv("SCRIPT_NAME", request.path.c_str(), 1);
		setenv("QUERY_STRING", request.query_string.c_str(), 1);
		setenv("SERVER_PROTOCOL", request.version.c_str(), 1);

		std::map<std::string, std::string> headers = request.getAllHeaders();

		for (std::map<std::string, std::string>::iterator it = headers.begin();
		it != headers.end();
		++it)
		{
			header = "HTTP_" + Utils::upper_case(it->first);
			setenv(header.c_str(), it->second.c_str(), 1);
		}
		if (lang == PYTHON)
			execl("/opt/pyenv/shims/python3", "python3", ("/home/amargolo/Desktop/webserv/www" + request.path).c_str(), NULL); // 0 is path to Python instal, 1 is arbitrary name, 2 is the script on server
		else
			execl("www/cgi-bin/rust_program", "rust_program", ("/home/amargolo/Desktop/webserv/www" + request.path).c_str(), NULL);

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

	// finished sending the job to program/interpreter
	close(pipefd_in[1]);

	while ((bytes_read = ::read(pipefd_out[0], buffer, sizeof(buffer))) > 0) {
		output.append(buffer, bytes_read);
	}

	close(pipefd_out[0]);
	waitpid(pid, &exit_status, 0);

	return response.response_body(200, output);
}


Response handle_get_cgi(const Request& request, Response& response, Language lang) {
	int				pipefd_out[2];
	int				pid;
	int				exit_status;
	char			buffer[1024];
	ssize_t			bytes_read;
	std::string		header;
	std::string		output;

	if (pipe(pipefd_out) == -1)
		return response.handle_error(500);


	pid = fork();
	if (pid == -1)
	{
		close(pipefd_out[0]);
		close(pipefd_out[1]);
		return response.handle_error(500);
	}

	if (pid == 0) // child
	{
		// closing unused pipe
		close(pipefd_out[0]);

		// redirecting pipe to stdout
		dup2(pipefd_out[1], STDOUT_FILENO);

			// closing fd that has been duplicated
		close(pipefd_out[1]);


		clearenv(); // start clean for safety - no injection possible
		setenv("REQUEST_METHOD", request.method.c_str(), 1);
		setenv("SCRIPT_NAME", request.path.c_str(), 1);
		setenv("QUERY_STRING", request.query_string.c_str(), 1);
		setenv("SERVER_PROTOCOL", request.version.c_str(), 1);

		std::map<std::string, std::string> headers = request.getAllHeaders();

		for (std::map<std::string, std::string>::iterator it = headers.begin();
		it != headers.end();
		++it)
		{
			header = "HTTP_" + Utils::upper_case(it->first);
			setenv(header.c_str(), it->second.c_str(), 1);
		}
		if (lang == PYTHON)
			execl("/opt/pyenv/shims/python3", "python3", ("/home/amargolo/Desktop/webserv/www" + request.path).c_str(), NULL); // 0 is path to Python instal, 1 is arbitrary name, 2 is the script on server
		else
			execl("www/cgi-bin/rust_program", "rust_program", ("/home/amargolo/Desktop/webserv/www" + request.path).c_str(), NULL);

		exit(1);
	}

	// parent

	// closing unused pipe
	close(pipefd_out[1]);

	while ((bytes_read = ::read(pipefd_out[0], buffer, sizeof(buffer))) > 0) {
		output.append(buffer, bytes_read);
	}

	close(pipefd_out[0]);
	waitpid(pid, &exit_status, 0);

	return response.response_body(200, output);
}
