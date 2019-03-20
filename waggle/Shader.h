#pragma once

class Shader
{
private:
	GLuint _shader;
protected:
	void printLog() const;
public:
	Shader(GLenum type);
	~Shader();

	inline operator GLuint() const { return _shader; }

	void fromSource(const std::string& source);
	void fromFile(const std::string& path);
};

