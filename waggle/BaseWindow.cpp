#include "stdafx.h"
#include "BaseWindow.h"

LRESULT CALLBACK BaseWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BaseWindow* window = nullptr;
	if (msg == WM_CREATE) {
		LPCREATESTRUCT lpCreateStruct = (LPCREATESTRUCT)lParam;
		window = (BaseWindow*)lpCreateStruct->lpCreateParams;
		window->_hwnd = hWnd;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
	}
	else {
		window = (BaseWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	if (window) {
		window->Invoke(msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool BaseWindow::Register()
{
	WNDCLASSEX wcx;

	::ZeroMemory(&wcx, sizeof(WNDCLASSEX));

	if (GetClassInfoEx(GetModuleHandle(nullptr), ClassName(), &wcx)) {
		return true;
	}

	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcx.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcx.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wcx.hIconSm = wcx.hIcon;
	wcx.hInstance = GetModuleHandle(nullptr);
	wcx.lpfnWndProc = WndProc;
	wcx.lpszClassName = ClassName();
	wcx.lpszMenuName = nullptr;
	wcx.style = ClassStyle();

	return RegisterClassEx(&wcx) != 0;
}

BaseWindow::BaseWindow()
	: _hwnd(nullptr)
{
}

BaseWindow::~BaseWindow()
{
	Close();
	Destroy();
}

void BaseWindow::Create()
{
	Register();

	CreateWindow(ClassName(), WindowName(), WindowStyle(),
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, GetModuleHandle(nullptr), this);

	Observe(WM_CLOSE, [](WPARAM, LPARAM) {PostQuitMessage(0); });
}

void BaseWindow::Close()
{
	if (_hwnd) {
		CloseWindow(_hwnd);
	}
}

void BaseWindow::Destroy()
{
	if (_hwnd) {
		DestroyWindow(_hwnd);
		_hwnd = nullptr;
	}
}

void BaseWindow::Size(int cx, int cy)
{
	SIZE sz = { cx, cy };
	Size(sz);
}

void BaseWindow::Size(SIZE sz)
{
	RECT rect{ 0, 0, sz.cx, sz.cy };

	DWORD style = GetWindowLong(_hwnd, GWL_STYLE);
	DWORD exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);
	AdjustWindowRectExForDpi(&rect , style, false, exStyle, GetDpiForWindow(_hwnd));
	ClientToScreen(_hwnd, (LPPOINT)&rect.left);
	ClientToScreen(_hwnd, (LPPOINT)&rect.right);
	SetWindowPos(_hwnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
}

SIZE BaseWindow::Size() const
{
	RECT rect;
	SIZE size = { 0, 0 };

	if (GetWindowRect(_hwnd, &rect)) {
		size.cx = rect.right - rect.left;
		size.cy = rect.bottom - rect.top;
	}
	return size;
}

void BaseWindow::Text(LPCTSTR text)
{
	SetWindowText(_hwnd, text);
}

void BaseWindow::Show(int show)
{
	ShowWindow(_hwnd, show);
}

void BaseWindow::Update()
{
	UpdateWindow(_hwnd);
}
