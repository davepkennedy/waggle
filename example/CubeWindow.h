#pragma once
#include <Windows.h>
#include <GLWindow.h>
#include <glm/glm.hpp>
#include <Program.h>
#include <gl/glew.h>

class CubeWindow :
	public GLWindow
{
private:
	GLuint _vertexArrayID;
	// GLuint _programID;
	GLuint _indexBuffer;
	GLuint _vertexbuffer;
	GLuint _colorbuffer;
	GLuint _matrixID;
	glm::mat4 _mvp;
	GLfloat _rot;

	Program _program;

	int _lastX;
	int _lastY;

	float _dx;
	float _dy;
	float _zoom;

	GLuint _objectColorID;
	GLuint _lightColorID;
	GLuint _lightPosID;
protected:
	void UpdateLightPos();
	void UpdateRotation();
	void UpdateBufferData();
	void MouseDown(WPARAM wParam, LPARAM lParam);
	void MouseMove(WPARAM wParam, LPARAM lParam);
public:
	CubeWindow();
	~CubeWindow();

	void Setup();
	void Render();
};

