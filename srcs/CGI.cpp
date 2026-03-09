
# include "CGI.hpp"
# include "Logger.hpp"
# include <unistd.h>

void set_cgi_env(const Request& request) {
	std::string		header;

	setenv("REQUEST_METHOD", request.method.c_str(), 1);
	setenv("SCRIPT_NAME", request.path.c_str(), 1);
	setenv("QUERY_STRING", request.query_string.c_str(), 1);
	setenv("SERVER_PROTOCOL", request.version.c_str(), 1);

	std::map<std::string, std::string> headers = request.getAllHeaders();

	for (std::map<std::string, std::string>::iterator it = headers.begin();
	it != headers.end();
	++it)
	{
		Utils::dash_to_underscore(it->first);
		Utils::upper_case(it->first);
		header = "HTTP_" +  it->first;
		setenv(header.c_str(), it->second.c_str(), 1);
	}
}

bool execute_cgi(const Request& request, char *path, Language lang) {
	if (lang == PYTHON)
	{
		execlp("python3", "python3", (std::string(path) + "/www" + request.path).c_str(), NULL);
	}
	else
	{
		execl("www/cgi-bin/rust_program", "rust_program", (std::string(path) + "/www" + request.path).c_str(), NULL);
	}
	return (false);
}

Response handle_post_cgi(const Request& request, Response& response, Language lang) {
	int				pipefd_in[2];
	int				pipefd_out[2];
	int				pid;
	int				exit_status = 0;	
	std::size_t		already_written = 0;
	char			buffer[1024];
	ssize_t			bytes_read;
	std::string		output;

	char path[1024];
	if (getcwd(path, sizeof(path)) == NULL)
	{
		LOG_ERROR("CGI: getcwd error.");
		return response.handle_error(500);
	}

	if (pipe(pipefd_in) == -1)
	{
		LOG_ERROR("CGI: Piping error.");
		return response.handle_error(500);
	}

	if (pipe(pipefd_out) == -1)
	{
		close(pipefd_in[0]);
		close(pipefd_in[1]);
		LOG_ERROR("CGI: Piping error.");
		return response.handle_error(500);
	}

	pid = fork();
	if (pid == -1)
	{
		close(pipefd_in[1]);
		close(pipefd_out[0]);
		close(pipefd_in[0]);
		close(pipefd_out[1]);
		LOG_ERROR("CGI: Forking error.");
		return response.handle_error(500);
	}

	if (pid == 0)
	{
		setpgid(0, 0);
		signal(SIGALRM, SIG_DFL);
		alarm(1);

		close(pipefd_in[1]);
		close(pipefd_out[0]);

		if (dup2(pipefd_in[0], STDIN_FILENO) == -1)
		{
			LOG_ERROR("CGI: dup2 error.");
			_exit(1);
		}
		if (dup2(pipefd_out[1], STDOUT_FILENO) == -1)
		{
			LOG_ERROR("CGI: dup2 error.");
			_exit(1);
		}

		close(pipefd_in[0]);
		close(pipefd_out[1]);
		set_cgi_env(request);

		if (!execute_cgi(request, path, lang))
		{
			LOG_ERROR("CGI: execl failure.");
			_exit(1);
		}
		_exit(1);
	}

	close(pipefd_in[0]);
	close(pipefd_out[1]);

	while (already_written < request.body.size()) {
		bytes_read = ::write(pipefd_in[1], request.body.data() + already_written, request.body.size() - already_written);
		if (bytes_read <= 0)
			break;
		already_written += bytes_read;
	}

	close(pipefd_in[1]);

	while ((bytes_read = ::read(pipefd_out[0], buffer, sizeof(buffer))) > 0) {
		output.append(buffer, bytes_read);
	}

	close(pipefd_out[0]);
	waitpid(pid, &exit_status, 0);
	if (WIFEXITED(exit_status) && WEXITSTATUS(exit_status) != 0)
		return response.handle_error(500);

	return response.response_body(200, output);
}

Response handle_get_cgi(const Request& request, Response& response, Language lang) {
	int				pipefd_out[2];
	int				pid;
	int				exit_status = 0;
	char			buffer[1024];
	ssize_t			bytes_read;
	std::string		output;

	char path[1024];
	if (getcwd(path, sizeof(path)) == NULL)
	{
		LOG_ERROR("CGI: getcwd error.");
		return response.handle_error(500);
	}
		
	if (pipe(pipefd_out) == -1)
	{
		LOG_ERROR("CGI: Piping error.");
		return response.handle_error(500);
	}

	pid = fork();
	if (pid == -1)
	{
		close(pipefd_out[0]);
		close(pipefd_out[1]);
		LOG_ERROR("CGI: Forking error.");
		return response.handle_error(500);
	}

	if (pid == 0)
	{
		setpgid(0, 0);
		signal(SIGALRM, SIG_DFL);
		alarm(1);
		
		close(pipefd_out[0]);

		if (dup2(pipefd_out[1], STDOUT_FILENO) == -1)
		{
			LOG_ERROR("CGI: dup2 error.");
			_exit(1);
		}

		close(pipefd_out[1]);

		set_cgi_env(request);

		if (!execute_cgi(request, path, lang))
		{
			LOG_ERROR("CGI: execl failure.");
			_exit(1);
		}
		_exit(1);
	}

	close(pipefd_out[1]);

	while ((bytes_read = ::read(pipefd_out[0], buffer, sizeof(buffer))) > 0) {
		output.append(buffer, bytes_read);
	}

	close(pipefd_out[0]);
	waitpid(pid, &exit_status, 0);
	if (WIFEXITED(exit_status) && WEXITSTATUS(exit_status) != 0)
		return response.handle_error(500);

	return response.response_body(200, output);
}
