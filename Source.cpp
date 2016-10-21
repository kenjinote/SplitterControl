#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <windowsx.h>

TCHAR szClassName[] = TEXT("Window");
#define SPLITTER_WIDTH 4

VOID Splitter_Draw(HWND hWnd, INT x)
{
	const HDC hdc = GetDC(hWnd);
	if (hdc)
	{
		const HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
		if (hBrush)
		{
			RECT rect;
			GetClientRect(hWnd, &rect);
			const RECT rectSplit = { x - SPLITTER_WIDTH / 2, 0, x + SPLITTER_WIDTH / 2, rect.bottom };
			SetROP2(hdc, R2_NOT);
			SetBkMode(hdc, TRANSPARENT);
			const HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);
			Rectangle(hdc, rectSplit.left, rectSplit.top, rectSplit.right, rectSplit.bottom);
			SelectObject(hdc, hOldBrush);
			DeleteObject(hBrush);
		}
		ReleaseDC(hWnd, hdc);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit1;
	static HWND hEdit2;
	static double percent = 0.5;
	static int oldposx;
	switch (msg)
	{
	case WM_CREATE:
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		{
			RECT rect;
			GetClientRect(hWnd, &rect);
			const int nWidth = rect.right;
			const int nHeight = rect.bottom;
			const int nEdit1Width = (int)(nWidth * percent);
			MoveWindow(hEdit1, 0, 0, nEdit1Width - SPLITTER_WIDTH / 2, nHeight, TRUE);
			MoveWindow(hEdit2, nEdit1Width + SPLITTER_WIDTH / 2, 0, nWidth - nEdit1Width - SPLITTER_WIDTH / 2, nHeight, TRUE);
		}
		break;
	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		oldposx = -1;
		break;
	case WM_MOUSEMOVE:
		if (GetCapture() == hWnd)
		{
			const int posx = GET_X_LPARAM(lParam);
			RECT rect;
			GetClientRect(hWnd, &rect);
			percent = (double)posx / (double)rect.right;
			if (percent < 0.0) percent = 0.0;
			else if (percent > 1.0) percent = 1.0;
			const int x = (int)(rect.right * percent);
			if (oldposx != x)
			{
				if (oldposx > 0) Splitter_Draw(hWnd, oldposx);
				Splitter_Draw(hWnd, x);
				oldposx = x;
			}
		}
		break;
	case WM_LBUTTONUP:
		if (GetCapture() == hWnd)
		{
			ReleaseCapture();
			SendMessage(hWnd, WM_SIZE, 0, 0);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_SIZEWE),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Splitter Control"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
