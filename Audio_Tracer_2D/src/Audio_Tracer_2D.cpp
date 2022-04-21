#include <cstdio>
#include <Windows.h>
#include <cstdint>
#include <chrono>

#define _USE_MATH_DEFINES
#include <math.h>

#include "WndState.h"
#include "Tracer.h"
#include "Utilities.h"

void UpdateMainWindow(WndState *state, HDC hdc);
void Render(WndState *state);
void ResizeDIBSection(WndState *state, long width, long height, long bits_per_pixel);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int constexpr DEFAULT_WIN_WIDTH = 1000, DEFAULT_WIN_HEIGHT = 1000;
int constexpr BITS_PER_PIXEL = 4 * sizeof(uint8_t);
wchar_t const *kAppName = L"Audio Tracer 2D";

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev_inst, LPSTR cmd_line, int show)
{
	WNDCLASS wind_class;
	wind_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wind_class.lpfnWndProc = WindowProc;
	wind_class.cbClsExtra = 0;
	wind_class.cbWndExtra = 0;
	wind_class.hInstance = inst;
	wind_class.hIcon = NULL;
	wind_class.hCursor = NULL;
	wind_class.hbrBackground = 0;
	wind_class.lpszMenuName = kAppName;
	wind_class.lpszClassName = kAppName;

	RegisterClass(&wind_class);

	WndState app_state;
	ResizeDIBSection(&app_state, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, BITS_PER_PIXEL);

	HWND hwnd = CreateWindow(wind_class.lpszClassName,
		kAppName,
		WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		DEFAULT_WIN_WIDTH + 16,
		DEFAULT_WIN_HEIGHT + 39,
		NULL,
		NULL,
		wind_class.hInstance,
		&app_state);

	if (!hwnd)
	{
		printf("Error: Could not create window!");
		return -1;
	}

	ShowWindow(hwnd, show);

	app_state.tracer = new Tracer{ { -1, -1, 1, 1}, {0, 0} };
	//Object o{ {{-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, 0.5}}, {{0, 1}, {1, 2}, {2, 3}, {3, 0}} };
	Object o{ {{0.2, 0.2}, {0.2, -0.2}, {0.6, -0.2}, {0.6, 0.6}, {-0.6, 0.6}, {-0.6, 1.0}, {1.0, 1.0}, {1.0, -0.6}, {-0.2, -0.6}, {-0.2, 0.2}}, {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 8}, {8, 9}, {9, 0}} };
	app_state.tracer->AddObject(o);
	app_state.tracer->Calculate();
	app_state.freq_bin = BINS / 8;

	app_state.running = true;
	
	MSG msg;
	while (app_state.running && GetMessage(&msg, hwnd, 0, 0))
	{
		if (msg.message == WM_QUIT) app_state.running = false;
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		HDC hdc = GetDC(hwnd);
		UpdateMainWindow(&app_state, hdc);
		ReleaseDC(hwnd, hdc);
	}
	

	return 0;
}

void UpdateMainWindow(WndState *state, HDC hdc)
{
	StretchDIBits(hdc,
		0,
		0,
		state->bitmap_info.bmiHeader.biWidth,
		state->bitmap_info.bmiHeader.biHeight,
		0,
		0,
		state->bitmap_info.bmiHeader.biWidth,
		state->bitmap_info.bmiHeader.biHeight,
		state->bitmap_memory,
		&state->bitmap_info,
		DIB_RGB_COLORS,
		SRCCOPY);
}

void Render(WndState *state)
{
	double grid_spacing = state->tracer->getGridSpacing();
	state->tracer->Draw(*state,
						{ -state->bitmap_info.bmiHeader.biWidth * grid_spacing / 2,
						  -state->bitmap_info.bmiHeader.biHeight * grid_spacing / 2,
						   state->bitmap_info.bmiHeader.biWidth * grid_spacing / 2,
						   state->bitmap_info.bmiHeader.biHeight * grid_spacing / 2 });
}

void ResizeDIBSection(WndState *state, long width, long height, long bits_per_pixel)
{
	if (state->bitmap_memory) VirtualFree(state->bitmap_memory, 0, MEM_RELEASE);

	state->bitmap_info.bmiHeader.biSize = sizeof(state->bitmap_info.bmiHeader);
	state->bitmap_info.bmiHeader.biWidth = width;
	state->bitmap_info.bmiHeader.biHeight = height;
	state->bitmap_info.bmiHeader.biPlanes = 1;
	state->bitmap_info.bmiHeader.biBitCount = 32;
	state->bitmap_info.bmiHeader.biCompression = BI_RGB;

	state->bitmap_memory = VirtualAlloc(NULL, width * height * bits_per_pixel, MEM_COMMIT, PAGE_READWRITE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WndState *state;
	if (uMsg == WM_CREATE)
	{
		CREATESTRUCT *pCreate = (CREATESTRUCT*)lParam;
		state = (WndState*)pCreate->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)state);
	}
	else
	{
		state = (WndState*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}

	switch (uMsg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			state->running = false;
			return 0;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			return 0;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			Render(state);

			HDC hdc = BeginPaint(hwnd, &ps);

			UpdateMainWindow(state, hdc);

			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_UP:
				{
					state->freq_bin += BINS / 256;
					state->freq_bin = std::max<int64_t>(0, std::min<int64_t>(state->freq_bin, BINS / 2));
					break;
				}
				case VK_DOWN:
				{
					state->freq_bin -= BINS / 256;
					state->freq_bin = std::max<int64_t>(0, std::min<int64_t>(state->freq_bin, BINS / 2));
					break;
				}
			}
			Render(state);
			return 0;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}