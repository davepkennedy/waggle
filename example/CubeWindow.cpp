#include "CubeWindow.h"
#include <GL/glew.h>

#include <Shader.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <windowsx.h>

std::vector<float> points = {
		-1, -1, -1,
		1,  -1, -1,
		1,  1,  -1,
		-1, 1,  -1,
		-1, -1, 1,
		1,  -1, 1,
		1,  1,  1,
		-1, 1,  1
};

std::vector<float> colors = {
		0,  0,  0,  // 0
		1,  0,  0,  // 1
		1,  0,  1,  // 2
		0,  0,  1,  // 3
		0,  1,  0,  // 4
		1,  1,  0,  // 5
		1,  1,  1,  // 6
		0,  1,  1   // 7
};

std::vector<int> indices = {
		0,  3,  2,
		4,  7,  3,
		5,  6,  7,
		1,  2,  6,
		2,  1,  0,
		3,  0,  4,
		7,  4,  5,
		6,  5,  1,
		4,  0,  1,
		1,  5,  4,
		2,  3,  6,
		7,  6,  3

};

void CubeWindow::UpdateLightPos()
{
	GLfloat rotX = sin(glm::radians(_rot)) * (1.5f *_zoom);
	GLfloat rotY = cos(glm::radians(_rot)) * (1.5f *_zoom);

	glm::vec3 lightPos (rotX, rotY, /*-3*/-rotX - rotY);

	glUniform3fv(_lightPosID, 1, glm::value_ptr(lightPos));

	_rot += 0.1f;
	if (_rot > 360) {
		_rot -= 360;
	}
}

void CubeWindow::UpdateRotation()
{
	GLfloat rotX = sin(glm::radians(_dx)) * _zoom;
	GLfloat rotY = cos(glm::radians(_dy)) * _zoom;

	// Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 projection = glm::perspective(glm::radians(67.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
	// Camera matrix
	glm::mat4 view = glm::lookAt(glm::vec3(rotX, rotY, /*-3*/-rotX - rotY), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 model = glm::mat4(1);
	// Our ModelViewProjection : multiplication of our 3 matrices
	_mvp = projection * (view * model);
}

void CubeWindow::UpdateBufferData()
{
	glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * points.size(), points.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, _colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), colors.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);
}

void CubeWindow::MouseDown(WPARAM wParam, LPARAM lParam)
{
	_lastX = GET_X_LPARAM(lParam);
	_lastY = GET_Y_LPARAM(lParam);
}

void CubeWindow::MouseMove(WPARAM wParam, LPARAM lParam)
{
	if ((wParam & MK_LBUTTON) == MK_LBUTTON) {
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		_dx = x - _lastX;
		//_dy = y - _lastY;

	}
	if ((wParam & MK_RBUTTON) == MK_RBUTTON) {
		int y = GET_Y_LPARAM(lParam);
		
		_zoom = (y - _lastY) / 10.0f;
	}
}

using namespace std::placeholders;

CubeWindow::CubeWindow()
	: _zoom (3)
{
	Observe(WM_LBUTTONDOWN, std::bind(&CubeWindow::MouseDown, this, _1, _2));
	Observe(WM_RBUTTONDOWN, std::bind(&CubeWindow::MouseDown, this, _1, _2));
	Observe(WM_MOUSEMOVE, std::bind(&CubeWindow::MouseMove, this, _1, _2));

	Observe(WM_SIZE, [this](WPARAM wParam, LPARAM lParam) {
		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
	});
}


CubeWindow::~CubeWindow()
{
}

void CubeWindow::Setup() 
{
	Size(350, 350);
	Text(TEXT("Cube Window"));


	glViewport(0, 0, 350, 350);

	glClearColor(0.45f, 0.55f, 0.45f, 1.0f);

	glEnable(GL_POLYGON_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glGenVertexArrays(1, &_vertexArrayID);
	glBindVertexArray(_vertexArrayID);

	{
		Shader vertexShader(GL_VERTEX_SHADER);
		Shader fragmentShader(GL_FRAGMENT_SHADER);

		vertexShader.fromFile("shader.vert");
		fragmentShader.fromFile("shader.frag");

		_program.addShader(vertexShader);
		_program.addShader(fragmentShader);
		_program.link();
	}
	glUseProgram(_program);

	// Get a handle for our "MVP" uniform
	_matrixID = glGetUniformLocation(_program, "MVP");
	_objectColorID = glGetUniformLocation(_program, "objectColor");
	_lightColorID = glGetUniformLocation(_program, "lightColor");
	_lightPosID = glGetUniformLocation(_program, "lightPos");

	glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
	glUniform3fv(_objectColorID, 1, glm::value_ptr(objectColor));
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	glUniform3fv(_lightColorID, 1, glm::value_ptr(lightColor));

	// Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 view = glm::lookAt(glm::vec3(4, 3, -3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 model = glm::mat4(1);
	// Our ModelViewProjection : multiplication of our 3 matrices
	_mvp = projection * (view * model);
	_rot = 0;

	glGenBuffers(1, &_vertexbuffer);
	glGenBuffers(1, &_colorbuffer);
	glGenBuffers(1, &_indexBuffer);

}

void CubeWindow::Render()
{
	UpdateRotation();
	UpdateBufferData();
	UpdateLightPos();

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(_program);

	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(_matrixID, 1, GL_FALSE, glm::value_ptr(_mvp));


	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
	//glDrawElements(GL_TRIANGLES, sizeof(g_vertex_indices)/sizeof(int), GL_UNSIGNED_INT, (void*)0);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
	//glDrawArrays(GL_LINES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
