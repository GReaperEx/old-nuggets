#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctime>

#include "PCunit.h"

#include <windows.h>

#include <GL/gl.h>
#include <GL/glext.h>

using namespace std;

void PCoutput();

const int windowWidth = 32*12+16*2+32;
const int windowHeight = 32;
const int windowStyle = WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION;

const char PCledonFile[ ] = "data/LEDon.bmp";
const char PCledoffFile[ ] = "data/LEDoff.bmp";

unsigned char* LEDon;
unsigned char* LEDoff;

HDC g_hDC;

struct BitmapFileHeader
{
    unsigned short Type;
    unsigned   int Size;
    unsigned short Res1;
    unsigned short Res2;
    unsigned   int BitOffset;
}__attribute__((packed));

struct BitmapInfoHeader
{
    unsigned   int InfoSize;
    unsigned   int Width;
    unsigned   int Height;
    unsigned short NumOfPlanes;
    unsigned short BitsPerPixel;
    unsigned   int Compression;
    unsigned   int ImageSize;
    unsigned   int xPixelsPerMeter;
    unsigned   int yPixelsPerMeter;
    unsigned   int NumOfUsedColors;
    unsigned   int NumOfImportantColors;
};

void loadRes()
{
    FILE* ifile;
    BitmapFileHeader bfh;
    BitmapInfoHeader bih;

    ifile = fopen(PCledonFile, "rb");

    if (!ifile)
    {
        cout << "Couldn't open " << PCledonFile << endl;
        return;
    }

    (void) fread(&bfh, sizeof(bfh), 1, ifile);
    (void) fread(&bih, sizeof(bih), 1, ifile);

    (void) fseek(ifile, bfh.BitOffset, SEEK_SET);

    LEDon = (unsigned char*)malloc(bih.ImageSize);
    (void) fread(LEDon, 1, bih.ImageSize, ifile);

    fclose(ifile);
    ifile = fopen(PCledoffFile, "rb");

    if (!ifile)
    {
        cout << "Couldn't open " << PCledoffFile << endl;
        return;
    }

    (void) fread(&bfh, sizeof(bfh), 1, ifile);
    (void) fread(&bih, sizeof(bih), 1, ifile);

    (void) fseek(ifile, bfh.BitOffset, SEEK_SET);

    LEDoff = (unsigned char*)malloc(bih.ImageSize);
    (void) fread(LEDoff, 1, bih.ImageSize, ifile);

    fclose(ifile);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    WNDCLASSEX windowClass;
    HWND hwnd;
    MSG msg;

    srand(time(NULL));

    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = "DiLa[Kenbak-1]";
    windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    if (!RegisterClassEx(&windowClass))
        return 0;

    RECT windowRect;

    windowRect.top = 0;
    windowRect.left = 0;
    windowRect.bottom = windowHeight;
    windowRect.right = windowWidth;

    AdjustWindowRectEx(&windowRect, windowStyle, FALSE, 0);

    hwnd = CreateWindowEx(0,
                          "DiLa[Kenbak-1]",
                          "DiLa[Kenbak-1]",
                          windowStyle,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                          NULL,
                          0,
                          hInstance,
                          NULL);

    if (!hwnd)
        return 0;

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

	loadRes();
	int counter = 0;
	int counter2 = 0;
    for (;;)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            DiLa::ResumeOperation();

            ++counter;
            ++counter2;

            if (counter == 3)
            {
                if (counter2 >= 5)
                {
                    counter2 -= 5;
                    PCoutput();
                }

                Sleep(1);

                counter = 0;
            }
        }
    }

    return msg.wParam;
}

void PCoutput()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(183.0f/255.0f, 183.0f/255.0f, 183.0f/255.0f, 0.0f);

    unsigned char* ptr[ ] = { LEDoff, LEDon };

    glPushMatrix();

    glRasterPos2i(0, 0);
    glDrawPixels(32, 32, GL_BGR, GL_UNSIGNED_BYTE,
                     ptr[DiLa::IsLEDon(DiLa::LED0)]);
    glRasterPos2i(32, 0);
    glDrawPixels(32, 32, GL_BGR, GL_UNSIGNED_BYTE,
                     ptr[DiLa::IsLEDon(DiLa::LED1)]);
    glRasterPos2i(32*2+16, 0);
    glDrawPixels(32, 32, GL_BGR, GL_UNSIGNED_BYTE,
                     ptr[DiLa::IsLEDon(DiLa::LED2)]);
    glRasterPos2i(32*3+16, 0);
    glDrawPixels(32, 32, GL_BGR, GL_UNSIGNED_BYTE,
                     ptr[DiLa::IsLEDon(DiLa::LED3)]);
    glRasterPos2i(32*4+16, 0);
    glDrawPixels(32, 32, GL_BGR, GL_UNSIGNED_BYTE,
                     ptr[DiLa::IsLEDon(DiLa::LED4)]);
    glRasterPos2i(32*5+16*2, 0);
    glDrawPixels(32, 32, GL_BGR, GL_UNSIGNED_BYTE,
                     ptr[DiLa::IsLEDon(DiLa::LED5)]);
    glRasterPos2i(32*6+16*2, 0);
    glDrawPixels(32, 32, GL_BGR, GL_UNSIGNED_BYTE,
                     ptr[DiLa::IsLEDon(DiLa::LED6)]);
    glRasterPos2i(32*7+16*2, 0);
    glDrawPixels(32, 32, GL_BGR, GL_UNSIGNED_BYTE,
                     ptr[DiLa::IsLEDon(DiLa::LED7)]);
    glRasterPos2i(32*8+16*2+32, 0);
    glDrawPixels(32, 32, GL_BGR, GL_UNSIGNED_BYTE,
                     ptr[DiLa::IsLEDon(DiLa::LED8)]);
    glRasterPos2i(32*9+16*2+32, 0);
    glDrawPixels(32, 32, GL_BGR, GL_UNSIGNED_BYTE,
                     ptr[DiLa::IsLEDon(DiLa::LED9)]);
    glRasterPos2i(32*10+16*2+32, 0);
    glDrawPixels(32, 32, GL_BGR, GL_UNSIGNED_BYTE,
                     ptr[DiLa::IsLEDon(DiLa::LED10)]);
    glRasterPos2i(32*11+16*2+32, 0);
    glDrawPixels(32, 32, GL_BGR, GL_UNSIGNED_BYTE,
                     ptr[DiLa::IsLEDon(DiLa::LED11)]);

    glPopMatrix();

    SwapBuffers(g_hDC);
}

void SetupPixelFormat(HDC hDC)
{
    int nPixelFormat;

    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL |
        PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        16,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0 };

    nPixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, nPixelFormat, &pfd);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HGLRC hRC;
    static HDC hDC;
    int width, height;

    switch(message)
    {
        case WM_CREATE:
            hDC = GetDC(hwnd);
            g_hDC = hDC;
            SetupPixelFormat(hDC);

            hRC = wglCreateContext(hDC);
            wglMakeCurrent(hDC, hRC);
        break;
        case WM_DESTROY:
            wglMakeCurrent(hDC, NULL);
            wglDeleteContext(hRC);

            PostQuitMessage(0);
        break;
        case WM_SIZE:
            height = HIWORD(lParam);
            width = LOWORD(lParam);

            if (height==0)
                height=1;

            glViewport(0, 0, width, height);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            glOrtho(0, windowWidth-1, 0, windowHeight-1, -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        break;
        case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_ESCAPE:
                    DestroyWindow(hwnd);

                    delete [] LEDon;
                    delete [] LEDoff;
                break;
                case 'Q':
                    DiLa::PressButton(DiLa::B_Data7);
                break;
                case 'W':
                    DiLa::PressButton(DiLa::B_Data6);
                break;
                case 'E':
                    DiLa::PressButton(DiLa::B_Data5);
                break;
                case 'R':
                    DiLa::PressButton(DiLa::B_Data4);
                break;
                case 'T':
                    DiLa::PressButton(DiLa::B_Data3);
                break;
                case 'Y':
                    DiLa::PressButton(DiLa::B_Data2);
                break;
                case 'U':
                    DiLa::PressButton(DiLa::B_Data1);
                break;
                case 'I':
                    DiLa::PressButton(DiLa::B_Data0);
                break;

                case 'A':
                    DiLa::PressButton(DiLa::B_Clear);
                break;
                case 'S':
                    DiLa::PressButton(DiLa::B_Display);
                break;
                case 'D':
                    DiLa::PressButton(DiLa::B_Set);
                break;
                case 'F':
                    DiLa::PressButton(DiLa::B_Read);
                break;
                case 'G':
                    DiLa::PressButton(DiLa::B_Store);
                break;
                case 'H':
                    DiLa::PressButton(DiLa::B_Start);
                break;
                case 'J':
                    DiLa::PressButton(DiLa::B_Stop);
                break;
            }
        break;
        case WM_KEYUP:
            switch (wParam)
            {
                case VK_ESCAPE:
                    CloseWindow(hwnd);

                    delete [] LEDon;
                    delete [] LEDoff;
                break;
                case 'Q':
                    DiLa::ReleaseButton(DiLa::B_Data7);
                break;
                case 'W':
                    DiLa::ReleaseButton(DiLa::B_Data6);
                break;
                case 'E':
                    DiLa::ReleaseButton(DiLa::B_Data5);
                break;
                case 'R':
                    DiLa::ReleaseButton(DiLa::B_Data4);
                break;
                case 'T':
                    DiLa::ReleaseButton(DiLa::B_Data3);
                break;
                case 'Y':
                    DiLa::ReleaseButton(DiLa::B_Data2);
                break;
                case 'U':
                    DiLa::ReleaseButton(DiLa::B_Data1);
                break;
                case 'I':
                    DiLa::ReleaseButton(DiLa::B_Data0);
                break;

                case 'A':
                    DiLa::ReleaseButton(DiLa::B_Clear);
                break;
                case 'S':
                    DiLa::ReleaseButton(DiLa::B_Display);
                break;
                case 'D':
                    DiLa::ReleaseButton(DiLa::B_Set);
                break;
                case 'F':
                    DiLa::ReleaseButton(DiLa::B_Read);
                break;
                case 'G':
                    DiLa::ReleaseButton(DiLa::B_Store);
                break;
                case 'H':
                    DiLa::ReleaseButton(DiLa::B_Start);
                break;
                case 'J':
                    DiLa::ReleaseButton(DiLa::B_Stop);
                break;
                case 'P':
                    if (DiLa::IsPowerOn())
                        DiLa::TurnPowerOff();
                    else
                        DiLa::TurnPowerOn();
                break;
            }
        break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

