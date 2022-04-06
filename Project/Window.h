#pragma once
#include "Graphics.h"
//#include "Windows.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <string>
#include <optional>



class Window {

private:
	class WindowClass {
	public:
		static const wchar_t* GetName();
		static HINSTANCE GetInstance();
	private:
		WindowClass();
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const wchar_t* wndClassName = L"Something";
		static WindowClass wndClass;
		HINSTANCE hinst;
	};

public:
	Window(int width, int height, const wchar_t* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::wstring& title);
	static std::optional<int> ProcessMessages();
	Graphics*& Gfx();
	Keyboard* Kbd();
	Mouse* Ms();
	int GetWidth()const;
	int GetHeight()const;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	Keyboard kb;
	Mouse mouse;

	int width, height;
	HWND hwnd;
	Graphics* gfx;
};
