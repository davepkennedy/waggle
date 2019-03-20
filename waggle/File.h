#pragma once

class File
{
private:
	FILE* _file;
public:
	File(const std::string& path);
	File(const std::string& path, const std::string& mode);
	~File();

	size_t length();
	std::vector<uint8_t> readAll();
	std::string read();
};

