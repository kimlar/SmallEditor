#include <Windows.h>
#include <memory>
#include "D3DTextDemo.h"


D3DTextDemo* demoPtr = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "SmallEditorClass";

	if (!RegisterClassEx(&wndClass))
		return -1;

	RECT rc = { 0,0, 800,600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindow("SmallEditorClass", "A small editor",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL, NULL, hInstance, NULL);

	if (!hwnd)
		return -1;

	ShowWindow(hwnd, cmdShow);

	D3DTextDemo demo;
	demoPtr = &demo;

	// Demo Initialize
	bool result = demo.Initialize(hInstance, hwnd);

	// Error reporting if there is an issue
	if (result == false)
		return -1;

	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Update and Draw
			demo.Update(0.0f);
			demo.Render();
		}
	}

	// Demo Shutdown
	demo.Shutdown();

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDC;

	switch (message)
	{
	case WM_PAINT:
		hDC = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_HOME:	// Home
			// move caret to beginning of line
			break;

		case VK_END:	// End
			// move caret to end of line
			break;

		case VK_PRIOR:	// Page Up
			// move caret up one page
			break;

		case VK_NEXT:	// Page Down
			// move caret down one page
			break;

		case VK_LEFT:	// Left arrow
			// move caret one to left
			break;

		case VK_RIGHT:	// Right arrow
			// move caret one to right
			break;

		case VK_UP:		// Up arrow
			// move caret one line up
			break;

		case VK_DOWN:	// Down arrow
			// move caret one line down
			break;

		case VK_DELETE:	// Delete
			// delete one character from right of current position
			break;
		}

		// Adjust caret position
		return 0;

	case WM_CHAR:
		switch (wParam)
		{
		case 0x08:	// Backspace
			demoPtr->CreateKeyboardEvent((char)wParam);
			break;

		case 0x09:	// Tab ('\t')
			demoPtr->CreateKeyboardEvent((char)wParam);
			break;

		case 0x0D:	// Carriage return ('\r')
			demoPtr->CreateKeyboardEvent((char)wParam);
			break;

		case 0x1B:	// Escape
			break;

		case 0x0A:	// Linefeed ('\n')
			break;

		default:	// Add characters to text buffer
			demoPtr->CreateKeyboardEvent((char)wParam);
			break;
		}

		// Adjust caret position
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}
