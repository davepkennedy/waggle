#include <Windows.h>

#include <Application.h>
#include <BaseWindow.h>
#include <GLWindow.h>
#include "CubeWindow.h"

#pragma comment (lib, "opengl32")

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR cmdLine, int nCmdShow)
{
	try {
		CubeWindow window;
		Application app(&window);
		app.Run();
	}
	catch (const std::exception& err) {
		OutputDebugStringA(err.what());
	}
}