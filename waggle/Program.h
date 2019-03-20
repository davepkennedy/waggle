#pragma once

#include <GL/glew.h>

class Shader;

class Program
{
private:
	GLuint _programId;
protected:
	void printLog() const;
public:
	Program();
	~Program();

	inline operator GLuint() const { return _programId; }

	void addShader(const Shader& shader);
	void link();
};
