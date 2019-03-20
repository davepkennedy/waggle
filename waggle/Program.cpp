#include "stdafx.h"

#include "Program.h"
#include "Shader.h"

Program::Program()
	: _programId(0)
{
}

Program::~Program()
{
	if (_programId) {
		glDeleteProgram(_programId);
	}
}

void Program::addShader(const Shader& shader)
{
	if (!_programId) {
		_programId = glCreateProgram();
	}
	glAttachShader(_programId, shader);
}

void Program::printLog() const {
	int max_length = 2048;
	int actual_length = 0;
	char log[2048];
	glGetProgramInfoLog(_programId, max_length, &actual_length, log);
	printf("program info log for GL index %u:\n%s", _programId, log);
}

void Program::link()
{
	// link the shader programme. if binding input attributes do that before link
	glLinkProgram(_programId);
	GLint params = -1;
	glGetProgramiv(_programId, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		printLog();
		throw std::exception("Program link error");
	}
}
