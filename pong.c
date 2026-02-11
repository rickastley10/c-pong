#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>

/* ============================================================
   CONFIG
   ============================================================ */
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

/* ============================================================
   GLOBALS
   ============================================================ */
static HBITMAP backbuffer_bitmap;
static HDC backbuffer_dc;
static void* backbuffer_memory;
static BITMAPINFO backbuffer_info;
static int running = 1; /* 1 = running, 0 = exit */

/* ============================================================
   RENDERER
   ============================================================ */
void clear_screen(uint32_t color)
{
    int i;
    uint32_t* pixel = (uint32_t*)backbuffer_memory;
    for (i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
        pixel[i] = color;
}

void draw_rect(int x, int y, int w, int h, uint32_t color)
{
    int px, py, sx, sy;
    for (py = 0; py < h; py++)
    {
        sy = y + py;
        if (sy < 0 || sy >= WINDOW_HEIGHT) continue;

        for (px = 0; px < w; px++)
        {
            sx = x + px;
            if (sx < 0 || sx >= WINDOW_WIDTH) continue;

            ((uint32_t*)backbuffer_memory)[sy * WINDOW_WIDTH + sx] = color;
        }
    }
}

/* ============================================================
   WINDOW
   ============================================================ */
LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CLOSE || msg == WM_DESTROY)
    {
        running = 0;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

/* ============================================================
   BACKBUFFER
   ============================================================ */
void init_backbuffer(HWND hwnd)
{
    HDC window_dc;
    window_dc = GetDC(hwnd);
    backbuffer_dc = CreateCompatibleDC(window_dc);

    /* Initialize bitmap info */
    memset(&backbuffer_info, 0, sizeof(backbuffer_info));
    backbuffer_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    backbuffer_info.bmiHeader.biWidth = WINDOW_WIDTH;
    backbuffer_info.bmiHeader.biHeight = -WINDOW_HEIGHT; /* top-down */
    backbuffer_info.bmiHeader.biPlanes = 1;
    backbuffer_info.bmiHeader.biBitCount = 32;
    backbuffer_info.bmiHeader.biCompression = BI_RGB;

    backbuffer_bitmap = CreateDIBSection(
        window_dc,
        &backbuffer_info,
        DIB_RGB_COLORS,
        &backbuffer_memory,
        NULL,
        0
    );

    SelectObject(backbuffer_dc, backbuffer_bitmap);
    ReleaseDC(hwnd, window_dc);
}

/* ============================================================
   MAIN LOOP
   ============================================================ */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR cmd, int show)
{
    WNDCLASS wc;
    HWND hwnd;
    MSG msg;
    HDC window_dc;

    int rect_x, rect_y;

    /* Register window class */
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = window_proc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "Minimal2DEngine";
    RegisterClass(&wc);

    /* Create window */
    hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        "Minimal 2D Engine",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL
    );

    init_backbuffer(hwnd);

    rect_x = 100;
    rect_y = 100;
    int bx = 100, by = 100;
    int px = 20, py = 0;
    int vx = 20, vy = 20;
    int pwidth =5, phiegth=100;

    /* ============================================================
   MAIN LOOP
   ============================================================ */
while (running)
{
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // ===== PADDLE CONTROLS =====
    if (GetAsyncKeyState('W') & 0x8000) {
        py -= 10; // move up
        if (py < 0) py = 0; // clamp to top
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        py += 10; // move down
        if (py + phiegth > WINDOW_HEIGHT) py = WINDOW_HEIGHT - phiegth; // clamp to bottom
    }

    /* Example draw */
    clear_screen(0xFFFFFFFF); /* white background */

    /* draw borders */
    draw_rect(0, 500, 500, 1, 0x00202020);
    draw_rect(0, 0, 500, 1, 0x00202020);
    draw_rect(0, 0, 1, 500, 0x00202020);
    draw_rect(500, 0, 1, 500, 0x00202020);

    /* draw ball */
    draw_rect(bx, by, 20, 20, 0x00202020);

    /* draw paddle */
    draw_rect(px, py, pwidth, phiegth, 0x00202020);

    // ===== BALL MOVEMENT =====
    bx += vx;
    by += vy;

    // Ball collision with paddle
    if (bx + 20 >= px && bx <= px + pwidth && by + 20 >= py && by <= py + phiegth) {
        vx = -vx;
        
        // Optional: tweak vy depending on where it hits the paddle
    }

    // Ball collision with walls
    if (bx + 20 >= 500) vx = -vx;
    if (by <= 0 || by + 20 >= 500) vy = -vy;
    if (bx==0){bx=100;by=100;}
    /* Flip backbuffer */
    window_dc = GetDC(hwnd);
    BitBlt(window_dc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, backbuffer_dc, 0, 0, SRCCOPY);
    ReleaseDC(hwnd, window_dc);

    Sleep(16); // ~60 FPS
}


    return 0;
}
