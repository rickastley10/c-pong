#include <windows.h>

#define WIDTH 800
#define HEIGHT 600

#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 80
#define BALL_SIZE 10

#define TIMER_ID 1
#define TIMER_INTERVAL 16  // ~60 FPS

// Game state
int playerY = HEIGHT / 2 - PADDLE_HEIGHT / 2;
int aiY = HEIGHT / 2 - PADDLE_HEIGHT / 2;

int ballX = WIDTH / 2;
int ballY = HEIGHT / 2;
int ballVX = 5;
int ballVY = 4;

int upPressed = 0;
int downPressed = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        SetTimer(hwnd, TIMER_ID, TIMER_INTERVAL, NULL);
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_UP) upPressed = 1;
        if (wParam == VK_DOWN) downPressed = 1;
        return 0;

    case WM_KEYUP:
        if (wParam == VK_UP) upPressed = 0;
        if (wParam == VK_DOWN) downPressed = 0;
        return 0;

    case WM_TIMER:
        // Player movement
        if (upPressed && playerY > 0)
            playerY -= 6;
        if (downPressed && playerY < HEIGHT - PADDLE_HEIGHT)
            playerY += 6;

        // AI movement (very simple)
        if (ballY > aiY + PADDLE_HEIGHT / 2)
            aiY += 4;
        else if (ballY < aiY + PADDLE_HEIGHT / 2)
            aiY -= 4;

        // Ball movement
        ballX += ballVX;
        ballY += ballVY;

        // Top/bottom collision
        if (ballY <= 0 || ballY >= HEIGHT - BALL_SIZE)
            ballVY = -ballVY;

        // Player paddle collision
        if (ballX <= 20 &&
            ballY + BALL_SIZE >= playerY &&
            ballY <= playerY + PADDLE_HEIGHT)
        {
            ballVX = -ballVX;
        }

        // AI paddle collision
        if (ballX >= WIDTH - 30 &&
            ballY + BALL_SIZE >= aiY &&
            ballY <= aiY + PADDLE_HEIGHT)
        {
            ballVX = -ballVX;
        }

        // Reset if out of bounds
        if (ballX < 0 || ballX > WIDTH)
        {
            ballX = WIDTH / 2;
            ballY = HEIGHT / 2;
        }

        InvalidateRect(hwnd, NULL, TRUE);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Clear background
        HBRUSH bg = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &ps.rcPaint, bg);
        DeleteObject(bg);

        HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));
        SelectObject(hdc, white);

        // Player paddle
        Rectangle(hdc, 10, playerY,
                  10 + PADDLE_WIDTH, playerY + PADDLE_HEIGHT);

        // AI paddle
        Rectangle(hdc, WIDTH - 20, aiY,
                  WIDTH - 20 + PADDLE_WIDTH, aiY + PADDLE_HEIGHT);

        // Ball
        Ellipse(hdc, ballX, ballY,
                ballX + BALL_SIZE, ballY + BALL_SIZE);

        DeleteObject(white);

        EndPaint(hwnd, &ps);
    }
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_ID);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "PongWindowClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Simple WinAPI Pong",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WIDTH, HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
