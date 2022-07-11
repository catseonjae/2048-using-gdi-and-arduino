#include <vector>
#include <conio.h>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <conio.h>
#include <random>
#include "SerialClass.h"	// Library described above

#pragma comment (lib,"Gdiplus.lib")
using namespace Gdiplus;
using namespace std;
using ll = long long;


vector<vector<ll> > board(4, vector<ll>(4, 0));
int n = 4;
int dx[4] = { 1,0,-1,0 }, dy[4] = { 0,-1,0,1 };
int r[2048], g[2048], b[2048];
char incomingData[256] = ""; int dataLength = 255;
int readResult = 0;
vector<int> status(6);
class InputHandler {
    bool activated[8] = {0,};//+x,-x,-y,y,12,3,6,9
    bool just_activated[8]={0,};
public:
    InputHandler() {}
    void update() {
        for (int i = 0; i < 4; i++) {
            just_activated[i + 4] = status[i+2];
        }
        for (int i = 0; i < 2; i++) {
            just_activated[i * 2] = status[i] ==0 ;
            just_activated[i * 2 + 1] = status[i] >= 650;
        }
        for (int i = 0; i < 8; i++) {
            bool t = activated[i];
            activated[i] = just_activated[i];
            just_activated[i] = !t && just_activated[i];
        }
    }
    bool get(int inputType, int id) {
        if (inputType == 0) {
            if (1 <= id && id <= 2) return just_activated[3-id];
            return just_activated[id];
        }
        else {
            //0,1,2,3->3,0,1,2
            id = (id+3) % 4;
            return just_activated[id + 4];
        }
    }
};

InputHandler inputHandler;
void parse() {
    int idx = 0;
    string s;
    for (int i = 0; i<255; i++) {
        if (incomingData[i]<'0' || incomingData[i]>'9' ) {
            int t=0;
            for (auto j : s) {
                t *= 10;
                t += j - '0';
            }
            status[idx] = t;
            idx++;
            s = "";
            if (idx >= 6) return;
        }
        else {
            s.push_back(incomingData[i]);
        }
    }
}


bool is_in(int c) {
    return 0 <= c && c < n;
}
bool move(int dir) {
    bool moved = 0;
    if (dir % 2) { //y방향
        int y;
        if (dir == 1) {
            y = 0;
        }
        else y = n - 1;
        int d = -dy[dir];
        for (int x = 0; x < n; x++) {
            int j = y, k = y + d;
            while (is_in(j)) {
                while (is_in(k) && !board[k][x]) k += d;
                if (!is_in(k)) break;
                if (board[k][x] == board[j][x]) {
                    board[j][x] *= 2;
                    board[k][x] = 0;
                    j += d;
                    moved = 1;
                }
                else {
                    if (board[j][x]) j += d;
                    board[j][x] = board[k][x];
                    if (j != k) {
                        board[k][x] = 0;
                        moved = 1;
                    }
                }
                k += d;
            }
        }
    }
    else { //x방향
        int x;
        if (dir == 2) {
            x = 0;
        }
        else x = n - 1;
        int d = -dx[dir];
        for (int y = 0; y < n; y++) {
            int j = x, k = x + d;
            while (is_in(j)) {
                while (is_in(k) && !board[y][k]) k += d;
                if (!is_in(k)) break;
                if (board[y][j] == board[y][k]) {
                    board[y][j] *= 2;
                    board[y][k] = 0;
                    j += d;
                    moved = 1;
                }
                else {
                    if (board[y][j]) {
                        j += d;
                    }
                    board[y][j] = board[y][k];
                    if (j != k) {
                        board[y][k] = 0;
                        moved = 1;
                    }
                }
                k += d;
            }
        }
    }
    return moved;
}
bool can_move() {
    for (int i = 0; i < board.size(); i++) {
        for (int j = 1; j < board.size(); j++) {
            if (board[i][j] != board[i][j - 1]) return 1;
            if (board[j][i] != board[j - 1][i]) return 1;
        }
    }
    return 0;
}
int value[2] = { 2,4 };
int generate() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, 1);
    return value[dis(gen)];
}
void generate_block() {
    int cnt = 0;
    for (auto i : board) {
        for (auto j : i) {
            if (j == 0) cnt++;
        }
    }
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(1, cnt);
    int order = dis(gen);
    cnt = 0;
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board.size(); j++) {
            if (board[i][j] == 0) {
                cnt++;
                if (cnt == order) {
                    board[i][j] = generate();
                    return;
                }
            }
        }
    }

}
int size(ll v) {
    int k = 1, ret = 0;
    while (k < v) {
        k *= 2;
        ret++;
    } return ret;
}

void setRGB(double rv, double gv, double bv) {
    int i = 0;
    for (; i < 2048 && i * rv <= 145; i++) {
        r[i] = 100+i * rv;
    }
    for (; i < 2048; i++) {
        r[i] = r[i - 1];
    }
    i = 0;
    for (; i < 2048 && i * gv <= 145; i++) {
        g[i] = 100+i * gv;
    }
    for (; i < 2048; i++) {
        g[i] = g[i - 1];
    }
    i = 0;
    for (; i<2048 && i * bv <= 145; i++) {
        b[i] = 100+i * bv;
    }
    for (; i < 2048; i++) {
        b[i] = b[i - 1];
    }

}
VOID OnPaint(HDC hdc)
{
    Graphics graphics(hdc);

    int iScreenCx = GetSystemMetrics(SM_CXSCREEN);
    int iScreenCy = GetSystemMetrics(SM_CYSCREEN);

    int blockSize = 100;
    int sx = iScreenCx/2-blockSize*2, sy = iScreenCy/2-blockSize*2;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int v = size(board[i][j]);
            v = min((int)v, 255);
            SolidBrush solidBrush(Color(50, r[v], g[v], b[v]));
            graphics.FillRectangle(&solidBrush, sx + j * blockSize, sy + i * blockSize, blockSize, blockSize);
        }
    }

    Pen      pen(Color(255, 30, 30, 30),4);
    for (int i = 0; i < 5; i++) {
        graphics.DrawLine(&pen, sx+0, sy+i * blockSize, sx+4 * blockSize, sy+i * blockSize);
        graphics.DrawLine(&pen, sx+i * blockSize, sy+0, sx+i * blockSize, sy+4 * blockSize);
    }
    FontFamily  fontFamily(L"Times New Roman");
    SolidBrush  solidBrush(Color(255, 255,255,255));

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (!board[i][j]) continue;
            RectF      rectF(sx+ blockSize *j, sy+ blockSize *i, sx+blockSize*(j+1), sy+blockSize*(i+1));
            string str = to_string(board[i][j]);
            double len = str.size();
            double piece = 100 / len;
            Font        font(&fontFamily, piece, FontStyleRegular, UnitPixel);

            wstring wide_string = wstring(str.begin(), str.end());
            const wchar_t* result = wide_string.c_str();

            graphics.DrawString(result, -1, &font, rectF, NULL, &solidBrush);
        }
    }


}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
    HWND                hWnd;
    MSG                 msg;
    WNDCLASS            wndClass;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = TEXT("GettingStarted");

    RegisterClass(&wndClass);

    hWnd = CreateWindow(
        TEXT("GettingStarted"),   // window class name
        TEXT("Getting Started"),  // window caption
        WS_OVERLAPPEDWINDOW,      // window style
        CW_USEDEFAULT,            // initial x position
        CW_USEDEFAULT,            // initial y position
        CW_USEDEFAULT,            // initial x size
        CW_USEDEFAULT,            // initial y size
        NULL,                     // parent window handle
        NULL,                     // window menu handle
        hInstance,                // program instance handle
        NULL);                    // creation parameters
    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);
    setRGB(5.0,0.0,0.0);
    generate_block();
    Serial* SP = new Serial("\\\\.\\COM4");
    while (GetMessage(&msg, NULL, 0, 0) && can_move())
    {
        readResult = SP->ReadData(incomingData, dataLength);
        incomingData[readResult] = 0;
        parse(); //x y 12 3 6 9
        inputHandler.update();
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    HDC          hdc;
    PAINTSTRUCT  ps;

    
    for (int i = 0; i < 4; i++) {
        if (inputHandler.get(0, i) || inputHandler.get(1, i)) {
            move(i);
            generate_block();
        }
    }
    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
} // WndProc
