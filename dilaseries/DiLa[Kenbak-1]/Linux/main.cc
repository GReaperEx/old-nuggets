#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctime>

#include "PCunit.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <GL/gl.h>
#include <GL/glx.h>

using namespace std;

void PCoutput(Display*, GLXDrawable);

const int windowWidth = 32*12+16*2+32;
const int windowHeight = 32;

const char wmDeleteWindow[ ] = "WM_DELETE_WINDOW";

const char PCledonFile[ ] = "data/LEDon.bmp";
const char PCledoffFile[ ] = "data/LEDoff.bmp";

unsigned char* LEDon;
unsigned char* LEDoff;

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

int main()
{
    XVisualInfo* visual = 0;
    Window window = 0;   
    XSetWindowAttributes attributes;
    
    srand(time(NULL));
    
    memset(&attributes, 0, sizeof(attributes));
    
    Display* display;
    int screennr;
    
    display = XOpenDisplay(0);
    if (!display)
	{
	    printf("Couldn't open display.\n");
	    return -1;
	}
    
    screennr = DefaultScreen(display);
    
    XVisualInfo visTemp;
    int visNumber;
    
    visTemp.screen = screennr;
    visTemp.depth = 24;
    while (!visual && visTemp.depth >= 16)
    {
        visual = XGetVisualInfo(display, VisualScreenMask | VisualDepthMask, &visTemp, &visNumber);
        visTemp.depth -= 8;
    }
    
    Colormap colormap;
	colormap = XCreateColormap(display,
			RootWindow(display, visual->screen),
			visual->visual, AllocNone);

	attributes.colormap = colormap;
	attributes.border_pixel = 0;
	attributes.event_mask = StructureNotifyMask | FocusChangeMask | ExposureMask;
	attributes.event_mask |= PointerMotionMask | KeyPressMask | KeyReleaseMask;
	attributes.override_redirect = 0;
    
    window = XCreateWindow(display, RootWindow(display, visual->screen), 
                           0, 0, windowWidth, windowHeight, 0, visual->depth, 
                           InputOutput, visual->visual,
                           CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
    			           &attributes);
    XMapRaised(display, window);
    
    Atom wmDelete;
    wmDelete = XInternAtom(display, wmDeleteWindow, True);
    XSetWMProtocols(display, window, &wmDelete, 1);
	
	GLXContext context = glXCreateContext(display, visual, 0, false);
	glXMakeCurrent(display, (GLXDrawable)window, context);
	
	glViewport(0, 0, windowWidth, windowHeight);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, windowWidth-1, 0, windowHeight-1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	loadRes();
	int counter = 0, ticksPassed = 0;
	int counter2 = 0;
	clock_t flagTick = clock();
    for (;;)
    {
        if (XPending(display) > 0)
        {
            XEvent event;
            XNextEvent(display, &event);
            
            switch (event.type)
            {
                case ConfigureNotify:
                    if (event.xconfigure.width != windowWidth ||
                        event.xconfigure.height != windowHeight)
                    {
                        // Doesn't let the window be resized
                        XResizeWindow(display, window, windowWidth, windowHeight);
                    }
                break;
                case KeyPress:
                {
                    long unsigned int X11Key;
                    char buf[8]={0};
                    XLookupString(&event.xkey, buf, sizeof(buf), &X11Key, NULL);
				
                    switch (X11Key)
                    {
                        case XK_Escape:
                            glXDestroyContext(display, context);
                            XDestroyWindow(display, window);
                            XCloseDisplay(display);
                            
                            delete [] LEDon;
                            delete [] LEDoff;
                            
                            return 0;
                        break;
                        case XK_q:
                            DiLa::PressButton(DiLa::B_Data7);
                        break;
                        case XK_w:
                            DiLa::PressButton(DiLa::B_Data6);
                        break;
                        case XK_e:
                            DiLa::PressButton(DiLa::B_Data5);
                        break;
                        case XK_r:
                            DiLa::PressButton(DiLa::B_Data4);
                        break;
                        case XK_t:
                            DiLa::PressButton(DiLa::B_Data3);
                        break;
                        case XK_y:
                            DiLa::PressButton(DiLa::B_Data2);
                        break;
                        case XK_u:
                            DiLa::PressButton(DiLa::B_Data1);
                        break;
                        case XK_i:
                            DiLa::PressButton(DiLa::B_Data0);
                        break;
                        
                        case XK_a:
                            DiLa::PressButton(DiLa::B_Clear);
                        break;
                        case XK_s:
                            DiLa::PressButton(DiLa::B_Display);
                        break;
                        case XK_d:
                            DiLa::PressButton(DiLa::B_Set);
                        break;
                        case XK_f:
                            DiLa::PressButton(DiLa::B_Read);
                        break;
                        case XK_g:
                            DiLa::PressButton(DiLa::B_Store);
                        break;
                        case XK_h:
                            DiLa::PressButton(DiLa::B_Start);
                        break;
                        case XK_j:
                            DiLa::PressButton(DiLa::B_Stop);
                        break;
                    }
                }
                break;
                case KeyRelease:
                {
                    long unsigned int X11Key;
                    char buf[8]={0};
                    XLookupString(&event.xkey, buf, sizeof(buf), &X11Key, NULL);
                    
                    if (XEventsQueued(display, QueuedAfterReading))
                    {
                        XEvent newEvent;
                        
                        XPeekEvent(display, &newEvent);
                        if (newEvent.type == KeyPress &&
                            newEvent.xkey.time == event.xkey.time &&
                            newEvent.xkey.keycode == event.xkey.keycode)
                        {
                            // Key wasn't actually released
                            break;
                        }
                    }
				
                    switch (X11Key)
                    {
                        case XK_q:
                            DiLa::ReleaseButton(DiLa::B_Data7);
                        break;
                        case XK_w:
                            DiLa::ReleaseButton(DiLa::B_Data6);
                        break;
                        case XK_e:
                            DiLa::ReleaseButton(DiLa::B_Data5);
                        break;
                        case XK_r:
                            DiLa::ReleaseButton(DiLa::B_Data4);
                        break;
                        case XK_t:
                            DiLa::ReleaseButton(DiLa::B_Data3);
                        break;
                        case XK_y:
                            DiLa::ReleaseButton(DiLa::B_Data2);
                        break;
                        case XK_u:
                            DiLa::ReleaseButton(DiLa::B_Data1);
                        break;
                        case XK_i:
                            DiLa::ReleaseButton(DiLa::B_Data0);
                        break;
                        
                        case XK_a:
                            DiLa::ReleaseButton(DiLa::B_Clear);
                        break;
                        case XK_s:
                            DiLa::ReleaseButton(DiLa::B_Display);
                        break;
                        case XK_d:
                            DiLa::ReleaseButton(DiLa::B_Set);
                        break;
                        case XK_f:
                            DiLa::ReleaseButton(DiLa::B_Read);
                        break;
                        case XK_g:
                            DiLa::ReleaseButton(DiLa::B_Store);
                        break;
                        case XK_h:
                            DiLa::ReleaseButton(DiLa::B_Start);
                        break;
                        case XK_j:
                            DiLa::ReleaseButton(DiLa::B_Stop);
                        break;
                        case XK_p:
                            if (DiLa::IsPowerOn())
                                DiLa::TurnPowerOff();
                            else
                                DiLa::TurnPowerOn();
                        break;
                    }
                }
                break;
                case ClientMessage:
                {
                    char* atom = XGetAtomName(display, event.xclient.message_type);
                    
                    if (*atom == *wmDeleteWindow)
                    {
                        glXDestroyContext(display, context);
                        XDestroyWindow(display, window);
                        XCloseDisplay(display);
                        
                        delete [] LEDon;
                        delete [] LEDoff;
                            
                        return 0;
                    }
                }
                break;
           }
        }
        
        DiLa::ResumeOperation();
        ticksPassed += clock() - flagTick;
        
        ++counter;
        ++counter2;
        
        if (counter == 3)
        {
            if (counter2 >= 5)
            {
                counter2 -= 5;
                PCoutput(display, (GLXDrawable)window);
            }
            
            struct timespec ts = {0,10000000 - 
                                    (ticksPassed/(CLOCKS_PER_SEC/1000))*1000000 };
	        nanosleep(&ts, NULL);
	        
	        counter = ticksPassed = 0;
	        flagTick = clock();
	    }
    }
    
    return 0;
}

void PCoutput(Display* display, GLXDrawable drawable)
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
    
    glXSwapBuffers(display, drawable);
}

