
# include "FileHandler.hpp"

bool FileHandler::is_directory(const std::string &file_path)
{
	DIR* dir = opendir(file_path.c_str());
	if (dir != NULL)
	{
		closedir(dir);
		return true;
	}
	return false;
}

bool FileHandler::is_readable(const std::string &file_path)
{
	return (access(file_path.c_str(), R_OK) == 0);
}

bool FileHandler::file_exists(const std::string &file_path)
{
	std::ifstream file(file_path.c_str());
	if (!file.is_open())
		return false;
	file.close();
	return true;
}

/* Loads html files dynamically through given file path reference*/
std::string FileHandler::load_file(const std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open())
		return "";

	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	if (size <= 0)
		return "";

	if (static_cast<size_t>(size) > MAX_FILE_SIZE)
		return "";

	std::string content;
	content.resize(size);

	if (!file.read(&content[0], size))
		return "";

	return content;
}

/* Processes content files e.g. css */
std::string FileHandler::find_content_type(const std::string &content_file_path)
{
	if (content_file_path.find(".css") != std::string::npos)
		return ("text/css");
	if (content_file_path.find(".js") != std::string::npos)
		return ("application/javascript");
	if (content_file_path.find(".jpg") != std::string::npos)
		return ("image/jpg");
	if (content_file_path.find(".png") != std::string::npos)
		return ("image/png");
	return ("text/html");
}

