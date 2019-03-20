#pragma once

#include "Observable.h"

class BaseWindow : public Observable<UINT, WPARAM, LPARAM>
{
private:
	HWND _hwnd;
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	operator HWND() const { return _hwnd; }
	bool Register();
public:
	BaseWindow();
	~BaseWindow();

	void Create();

	void Close();
	virtual void Destroy();

	virtual void Size(int cx, int cy);
	virtual void Size(SIZE sz);

	SIZE Size() const;

	void Text(LPCTSTR text);

	virtual LPCTSTR ClassName() { return TEXT("BaseWindow"); }
	virtual LPCTSTR WindowName() { return ClassName(); }
	virtual UINT ClassStyle() { return CS_HREDRAW | CS_VREDRAW; }
	virtual UINT WindowStyle() { return WS_OVERLAPPEDWINDOW; }

	void Show(int show = SW_SHOW);
	void Update();

	virtual void Idle() {};
};

