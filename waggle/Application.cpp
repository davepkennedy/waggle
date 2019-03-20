#include "stdafx.h"
#include "Application.h"
#include "BaseWindow.h"

Application::Application(BaseWindow* window)
	: _window(window)
{
}


Application::~Application()
{
}

void Application::Run() const
{
	MSG msg;

	_window->Create();
	_window->Show();
	_window->Update();

	ZeroMemory(&msg, sizeof(MSG));
	while (true) {

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		_window->Idle();
	}
}
