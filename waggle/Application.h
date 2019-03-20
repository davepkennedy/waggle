#pragma once

class BaseWindow;

class Application
{
private:
	BaseWindow* _window;
public:
	Application(BaseWindow* window);
	~Application();

	void Run() const;
};

