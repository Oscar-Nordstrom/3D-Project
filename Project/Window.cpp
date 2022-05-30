#include "Window.h"
#include <iostream>
#include "imgui_impl_win32.h"

#define WindowStyles WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() : hinst(GetModuleHandle(nullptr)) {

	WNDCLASSEX wc = { 0 };

	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = (LPCWSTR)GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() {
	UnregisterClass((LPCWSTR)wndClassName, GetInstance());
}

const wchar_t* Window::WindowClass::GetName() {
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() {
	return wndClass.hinst;
}

Window::Window(int width, int height, const wchar_t* name)
	:width(width), height(height)
{

	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (AdjustWindowRect(&wr, WindowStyles, false) == 0) {
		std::cerr << "Can't adjust windows rect.\n";
	}


	hwnd = CreateWindow((LPCTSTR)WindowClass::GetName(), (LPCTSTR)name,
		WindowStyles,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this);

	if (hwnd == NULL) {
		std::cerr << "Create window.\n";
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);

	gfx = new Graphics(width, height, hwnd);
}

Window::~Window() {
	delete gfx;
	DestroyWindow(hwnd);
}

void Window::SetTitle(const std::wstring& title)
{
	if (SetWindowText(hwnd, (LPCWSTR)title.c_str()) == 0) {
		std::cerr << "Can't set title\n";
	}
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return {};
}

Graphics*& Window::Gfx()
{
	return gfx;
}

Keyboard* Window::Kbd()
{
	return &kb;
}

Mouse* Window::Ms()
{
	return &mouse;
}

int Window::GetWidth() const
{
	return width;
}

int Window::GetHeight() const
{
	return height;
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (msg == WM_CREATE) {
		const CREATESTRUCT* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
		return true;
	}

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		//Keyboard//
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:	
		kb.Keydown(static_cast<unsigned char>(wParam));
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:	
		kb.KeyUp(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:	
		break;
		//Mouse//
	case WM_MOUSEMOVE:
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnMouseMove(pt.x, pt.y);
		break;
	}


	return DefWindowProc(hwnd, msg, wParam, lParam);
}









