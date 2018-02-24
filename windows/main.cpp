#include <windows.h>
#include <windowsx.h>
#include "game/Game.h"

static const char* PROJECT_NAME = "Balance";

static Game* game;

static bool landscape = true;
static const int SCREEN_WIDTH = 720;
static const int SCREEN_HEIGHT = 1280;

static HWND hwnd;
static HDC hdc;
static HGLRC gl_context;

static float rotangle = 180.0f;
static int frames = 0;

static float sensor_data[9] = {0.0f, 0.0f, 0.0f,
							   0.0f, 0.0f, 0.0f,
							   0.0f, 0.0f, 0.0f };

static void GetWindowDimensions(int& width, int& height)
{
	int frame_width	= GetSystemMetrics(SM_CXSIZEFRAME);
	int frame_height = GetSystemMetrics(SM_CYSIZEFRAME);
	int caption_height = GetSystemMetrics(SM_CYCAPTION);

	if (landscape)
	{
		width = (SCREEN_HEIGHT - 1) + frame_width;
		height = (SCREEN_WIDTH - 1) + frame_height + caption_height;
	}
	else
	{
		width = (SCREEN_WIDTH - 1) + frame_width;
		height = (SCREEN_HEIGHT - 1) + frame_height + caption_height;
	}
}

static void CreateOGLContext()
{
	if (gl_context != NULL)
		wglDeleteContext(gl_context);

	hdc = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd;
    int pixelformat;

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.dwLayerMask = PFD_MAIN_PLANE;
    pfd.iPixelType = PFD_TYPE_COLORINDEX;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32;
    pfd.cAccumBits = 0;
    pfd.cStencilBits = 0;

    pixelformat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelformat, &pfd);

	gl_context = wglCreateContext(hdc);
	wglMakeCurrent(hdc, gl_context);
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int CALLBACK wWinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 PWSTR pCmdLine,
					 int nCmdShow)
{
	INT64	freq, time_start, time_end;
	INT64 fps_start;

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = PROJECT_NAME;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);

	int win_width, win_height;
	GetWindowDimensions(win_width, win_height);

	hwnd = CreateWindowEx(
		0,							// extended window style
		PROJECT_NAME,				// window class
		PROJECT_NAME,				// window title
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,

		CW_USEDEFAULT,				// x-coord
		CW_USEDEFAULT,				// y-coord
		win_width-1,					// width
		win_height-1,				// height

		NULL,						// parent window
		NULL,						// menu
		hInstance,					// instance
		NULL						// additional app data
		);

	if (hwnd == NULL)
		return 0;

	CreateOGLContext();

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	ShowCursor(TRUE);

	glewInit();

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);

	game = new Game();

	if (!landscape)
		game->init(SCREEN_WIDTH, SCREEN_HEIGHT);
	else
		game->init(SCREEN_HEIGHT, SCREEN_WIDTH);

	MSG msg = { };

	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
	QueryPerformanceCounter((LARGE_INTEGER *)&time_start);
	QueryPerformanceCounter((LARGE_INTEGER *)&time_end);
	QueryPerformanceCounter((LARGE_INTEGER *)&fps_start);

	bool quit = false;
	while (!quit)
	{
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
                quit = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		QueryPerformanceCounter((LARGE_INTEGER *)&time_start);

		sensor_data[0] = cos(rotangle * M_PI / 180.0f);
		sensor_data[1] = sin(rotangle * M_PI / 180.0f);

		sensor_data[3] = 90.0f;

		game->onSensor(sensor_data);
		game->onUpdate(Game::LOGIC_PERIOD_NS);
		game->onRender();
		frames++;
		SwapBuffers(hdc);

		QueryPerformanceCounter((LARGE_INTEGER *)&time_end);

		INT64 diff = time_end - time_start;
		double tm = ((double)(time_end - time_start) / freq);
		int millisecs = (int)(tm * 1000);

		game->reportMS(millisecs);

		INT64 fpsdiff = time_end - fps_start;
		double fpstm = ((double)(time_end - fps_start) / freq);
		if (fpstm >= 1.0)
		{
			game->reportFPS(frames);
			frames = 0;
			fps_start = time_end;
		}

		if (millisecs < 16)
		{
			Sleep(16-millisecs-1);
		}
	}

	game->shutdown();
	delete game;

	wglDeleteContext(gl_context);
    ReleaseDC(hwnd, hdc);
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			//FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

			EndPaint(hwnd, &ps);
			break;
		}

		case WM_KEYUP:
		{
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			else if (wParam == VK_F1)
			{
				/*
				if (landscape)
				{
					landscape = false;
					game->onViewportChange(SCREEN_WIDTH, SCREEN_HEIGHT);
				}
				else
				{
					landscape = true;
					game->onViewportChange(SCREEN_HEIGHT, SCREEN_WIDTH);
				}

				int win_width, win_height;
				GetWindowDimensions(win_width, win_height);

				RECT rect;
				GetWindowRect(hwnd, &rect);

				MoveWindow(hwnd, rect.left, rect.top, win_width, win_height, TRUE);
				
				hdc = GetDC(hwnd);
				wglMakeCurrent(hdc, gl_context);
				*/
			}
			break;
		}

		case WM_KEYDOWN:
		{
			if (wParam == VK_LEFT)
			{
				rotangle--;
			}
			else if (wParam == VK_RIGHT)
			{
				rotangle++;
			}
			break;
		}

		case WM_LBUTTONDOWN:
		{
			int xpos = GET_X_LPARAM(lParam);
			int ypos = GET_Y_LPARAM(lParam);

			game->onTouch(UI::UI_TOUCH_DOWN, (float)xpos, (float)ypos);
			break;
		}

		case WM_RBUTTONDOWN:
		{
			int xpos = GET_X_LPARAM(lParam);
			int ypos = GET_Y_LPARAM(lParam);

			// mouse rotation controls
			RECT rect;

			GetClientRect(hwnd, &rect);
			int centerx = rect.left + ((rect.right - rect.left) / 2);
			int centery = rect.top + ((rect.bottom - rect.top) / 2);

			float sx = (float)(xpos) - (float)(centerx);
			float sy = (float)(ypos) - (float)(centery);

			rotangle = atan2(sx, sy) * 180.0f / M_PI;
			break;
		}

		case WM_LBUTTONUP:
		{
			int xpos = GET_X_LPARAM(lParam);
			int ypos = GET_Y_LPARAM(lParam);

			game->onTouch(UI::UI_TOUCH_UP, (float)xpos, (float)ypos);
			break;
		}

		case WM_MOUSEMOVE:
		{
			if (wParam & MK_LBUTTON)
			{
				int xpos = GET_X_LPARAM(lParam);
				int ypos = GET_Y_LPARAM(lParam);

				game->onTouch(UI::UI_TOUCH_MOVE, (float)xpos, (float)ypos);
			}
			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}