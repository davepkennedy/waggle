#include "stdafx.h"
#include "File.h"

File::File(const std::string& path)
	: File(path, "r")
{
}

File::File(const std::string& path, const std::string& mode)
{
	if (fopen_s(&_file, path.c_str(), mode.c_str())) {
		exit(EXIT_FAILURE);
	}
}

File::~File()
{
	fclose(_file);
}

size_t File::length() {
	int fd = _fileno(_file);
	struct stat fileInfo;
	fstat(fd, &fileInfo);
	return fileInfo.st_size;
}

std::vector<uint8_t> File::readAll() {
	std::vector<uint8_t> buffer;
	buffer.resize(length());
	fread(buffer.data(), sizeof(char), length(), _file);
	return buffer;
}

std::string File::read() {
	std::vector<uint8_t> buffer = readAll();
	return std::string(buffer.begin(), buffer.end());
}