#include <windows.h>

#define WIN_WIDTH   640
#define WIN_HEIGHT  480

#define PADDLE_W 10
#define PADDLE_H 80
#define BALL_SIZE 10

int py = 200;
int ay = 200;
int bx = 320;
int by = 240;
int vx = -5;
int vy = 4;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;
    RECT r;

    switch (msg) {
    case WM_KEYDOWN:
        if (wParam == 'W' && py > 0)
            py -= 10;
        if (wParam == 'S' && py < WIN_HEIGHT - PADDLE_H)
            py += 10;
        if (wParam == VK_ESCAPE)
            PostQuitMessage(0);
        return 0;

    case WM_TIMER:
        /* ball movement */
        bx += vx;
        by += vy;

        if (by <= 0 || by >= WIN_HEIGHT - BALL_SIZE)
            vy = -vy;

        /* player paddle */
        if (bx <= 20 &&
            by + BALL_SIZE >= py &&
            by <= py + PADDLE_H)
            vx = -vx;

        /* AI paddle */
        if (bx >= WIN_WIDTH - 30 &&
            by + BALL_SIZE >= ay &&
            by <= ay + PADDLE_H)
            vx = -vx;

        /* scoring reset */
        if (bx < 0 || bx > WIN_WIDTH) {
            bx = WIN_WIDTH / 2;
            by = WIN_HEIGHT / 2;
            vx = -vx;
        }

        /* AI movement */
        if (ay + PADDLE_H / 2 < by)
            ay += 4;
        else if (ay + PADDLE_H / 2 > by)
            ay -= 4;

        InvalidateRect(hwnd, NULL, TRUE);
        return 0;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);

        /* background */
        GetClientRect(hwnd, &r);
        FillRect(hdc, &r, (HBRUSH)(COLOR_WINDOW + 1));

        /* paddles */
        Rectangle(hdc, 10, py, 10 + PADDLE_W, py + PADDLE_H);
        Rectangle(hdc, WIN_WIDTH - 20, ay,
                         WIN_WIDTH - 20 + PADDLE_W, ay + PADDLE_H);

        /* ball */
        Ellipse(hdc, bx, by, bx + BALL_SIZE, by + BALL_SIZE);

        EndPaint(hwnd, &ps);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev,
                   LPSTR lpCmd, int nShow) {
    WNDCLASS wc;
    HWND hwnd;
    MSG msg;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "PONG_WIN32";

    RegisterClass(&wc);

    hwnd = CreateWindow(
        "PONG_WIN32",
        "Win32 Pong (TCC)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WIN_WIDTH, WIN_HEIGHT,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, nShow);
    UpdateWindow(hwnd);

    SetTimer(hwnd, 1, 16, NULL); /* ~60 FPS */

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
