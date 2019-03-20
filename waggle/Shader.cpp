#include "stdafx.h"
#include "Shader.h"
#include "File.h"

#include <iostream>

Shader::Shader(GLenum type)
	:_shader(glCreateShader(type))
{
}

Shader::~Shader()
{
	glDeleteShader(_shader);
}

void Shader::printLog() const {
	int max_length = 2048;
	int actual_length = 0;
	char log[2048];
	glGetShaderInfoLog(_shader, max_length, &actual_length, log);
	fprintf(stderr, "shader info log for GL index %i:\n%s\n", _shader, log);
}


void Shader::fromSource(const std::string& source)
{
	const GLchar *p = source.c_str();
	glShaderSource(_shader, 1, &p, NULL);
	glCompileShader(_shader);
	// check for compile errors
	int params = -1;
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		printLog();
		std::cerr << source << std::endl;
		throw std::exception("Shader compile error");
	}
}

void Shader::fromFile(const std::string& path)
{
	fromSource(File(path).read());
}
