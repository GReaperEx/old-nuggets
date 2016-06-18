// Copyright (C) 2011-2013 celem@sourceforge.net
// Copyright (C) 2014 GReaperEx(Marios F.)
//! This file is part of Elowatha-Reloaded
/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "framework.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "ResourceManager.h"


//#include <gl/glut.h>
//#include "tile_properties.h"
//#include "map_loader.h"

using namespace std;

CamInfo Cam;

//Char location and Default start pos
int cposx, cposy;
int loadedmap=0;
bool playintro = 1;
bool playpause=0;
int pposx = 17;
int pposy = 17;
GLuint base;
int reportbool = 1;

bool moved;
int nohostiles = 0;
char buffer[100]="Help is not implemented, see http://sourceforge.net/projects/elore/";
char buffer2[25]="Help";
char buffer3[25]="Coming Soon";
//Checks for held keys
bool wp,sp,dp,ap,rp;

bool keyPressed;


//0 - blue.bmp, 1 - red.bmp, 2 - forest_big.bmp
char Map[MAP_SIZEX][MAP_SIZEY] =
{
    {2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {2, 2, 2, 2, 8, 1, 7, 2, 2, 8, 8, 7, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {2, 2, 2, 0, 2, 1, 2, 8, 8, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 0, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 0, 2, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 8, 7, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 8, 8, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 2, 2, 2, 1, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {0, 2, 1, 1, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
};

char walkability[MAP_SIZEX][MAP_SIZEY];


GLfloat rtile;
GLuint LoadTextureBMP( const char * filename, int wrap );

//Key presses
bool keys[256];

GLuint texture_id[11];

void BuildFont()					// Build Our Bitmap Font
{

}

GLvoid glPrint(const char *fmt, ...)				// Custom GL "Print" Routine
{

}

ResourceManager rsrcMngr;

void LoadData()
{
    rsrcMngr.getResourceData("blue.png", &texture_id[0]);
    rsrcMngr.getResourceData("red.png", &texture_id[1]);
    rsrcMngr.getResourceData("forest_big.png", &texture_id[2]);
    rsrcMngr.getResourceData("charnew.png", &texture_id[3]);
    rsrcMngr.getResourceData("char.png", &texture_id[4]);
    rsrcMngr.getResourceData("intro.png", &texture_id[5]);
    rsrcMngr.getResourceData("river_sbankf.png", &texture_id[6]);
    rsrcMngr.getResourceData("river_wse.png", &texture_id[7]);
    rsrcMngr.getResourceData("river_we.png", &texture_id[8]);
    rsrcMngr.getResourceData("charnew_mask.png", &texture_id[10]);
}

// Initialize OpenGL graphics
void InitGraphics()
{
    //Initial Cam
    Cam.Cx=3;
    Cam.Cy=17;
    Cam.Cz=18;

    glClearColor(0, 0, 0, 0.5);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    BuildFont();						// Build The Font

    rsrcMngr.loadAllData("data");
    LoadData();
}

// Resize graphics to fit window
void ResizeGraphics(int width, int height)
{
    GLfloat aspect;

    aspect = (GLfloat)width / height;

    // Adjust graphics to window size
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //Flip
    //glScalef(1.0, -1.0, 1.0);
    //gluOrtho2D(-5, 35, -5, 35);
    gluPerspective(45.0, aspect, 1.0, -1);
    glMatrixMode(GL_MODELVIEW);
}

//Dump data
/*void report(tile_properties properties)
{
	int x, y, id;
	x = properties.x;
	y = properties.y;
	id= properties.texture_id;

    printf("This is properties[%d]",x);
    printf("[%d].\n", y);
    printf("It has tile type %d", id);

}*/


//Intromovie
void DrawIntro(SDL_Window* window)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set location in front of camera
    glLoadIdentity();
    //glTranslated(0, 0, -10);

    //Gets the view squared off a bit and pulls back so it fits
    //Still learning this control.  1st and 4th is x-axis, 2nd and 5th is y-axis, 3rd is zoom
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glBindTexture( GL_TEXTURE_2D, texture_id[3]);
    Tile(0,0,0);
    glRasterPos2f(-2,-1);
    glPrint("Welcome to Elowatha Reloaded"); // Print GL Text To The Screen
    glRasterPos2f(-2,-0.5);
    glPrint("Press X to begin a new adventure");

    SDL_GL_SwapWindow(window);
}
void DrawPause(SDL_Window* window)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set location in front of camera
    glLoadIdentity();
    //glTranslated(0, 0, -10);

    //Gets the view squared off a bit and pulls back so it fits
    //Still learning this control.  1st and 4th is x-axis, 2nd and 5th is y-axis, 3rd is zoom
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glBindTexture( GL_TEXTURE_2D, texture_id[3]);
    Tile(0,0,0);
    glRasterPos2f(-2,-1);
    glPrint("Elowatha Reloaded is paused"); // Print GL Text To The Screen
    glRasterPos2f(-2,-0.5);
    glPrint("Press X to resume your adventure");

    SDL_GL_SwapWindow(window);
}

void DoAI(CEntity *primative1, CEntity *player1)
{
    int choice;

    int pposx, pposy;

    primative1->getPos(pposx, pposy);

    choice = primative1->move((void*)player1);

    switch (choice)
    {
    case 0:
        pposx += 1;
        break;
    case 1:
        pposx -= 1;
        break;
    case 2:
        pposy += 1;
        break;
    case 3:
        pposy -= 1;
        break;
    }


    primative1->setPos(pposx, pposy);

    primative1->speak();
}
void DrawPlayer (int x, int y)
{
//Render character and primative

    glEnable(GL_BLEND);							// Enable Blending
    glDisable(GL_DEPTH_TEST);						// Disable Depth Testing

    glBlendFunc(GL_DST_COLOR,GL_ZERO);				// Blend Screen Color With Zero (Black)
    glBindTexture(GL_TEXTURE_2D, texture_id[10]);//mask
    Tile(x,y,1.0);
    glBlendFunc(GL_ONE, GL_ONE);					// Copy Color To The Screen

    glBindTexture(GL_TEXTURE_2D, texture_id[3]);//sprite
    Tile(x, y, 1.0);
    glEnable(GL_DEPTH_TEST);						// Enable Depth Testing
    glDisable(GL_BLEND);							// Disable Blending

    //glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}
void DrawPrimative(int x, int y)
{
    glBindTexture(GL_TEXTURE_2D, texture_id[4]);
    Tile(x, y, 1.0);
}

/*void assign_properties(int x, int y, int tile, tile_properties properties)
{
	switch (tile)
	{
		case '1' : properties.walkable = TRUE;
				properties.texture_id = tile;
				properties.x = x;
				properties.y = y;

		case '2' : properties.walkable = TRUE;
			properties.texture_id = tile;
			properties.x = x;
			properties.y = y;
		case '0' : properties.walkable = FALSE;
			properties.texture_id = tile;
			properties.x = x;
			properties.y = y;
		default:
			properties.walkable = TRUE;
			properties.texture_id = tile;
			properties.x = x;
			properties.y = y;
	}
}*/

//Draws a vertically standing tile.  uses blend to make sprite parts transparent,
//stands at a 0.5 y value to centre on a tile.  For working with tilted world
/*void testVert()
{
    glEnable(GL_BLEND);							// Enable Blending
    glDisable(GL_DEPTH_TEST);						// Disable Depth Testing

    glBlendFunc(GL_DST_COLOR,GL_ZERO);				// Blend Screen Color With Zero (Black)
    glBindTexture(GL_TEXTURE_2D, texture_id[10]);//mask
    glBegin(GL_QUADS);
    //glColor3d(1,1,1);  //Quad is white under texture
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(float(4), float(17.5), float(1));
    glTexCoord2f(0.9f, 0.0f);
    glVertex3f(float(4 + 1), float(17.5), float(1));
    glTexCoord2f(0.9f, 1.0f);
    glVertex3f(float(4 + 1), float(17.5), float(0));
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(float(4), float(17.5), float(0));
    glEnd();
    glBlendFunc(GL_ONE, GL_ONE);					// Copy Color To The Screen

    glBindTexture(GL_TEXTURE_2D, texture_id[3]);//sprite
    glBegin(GL_QUADS);
    //glColor3d(1,1,1);  //Quad is white under texture
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(float(4), float(17.5), float(1));
    glTexCoord2f(0.9f, 0.0f);
    glVertex3f(float(4 + 1), float(17.5), float(1));
    glTexCoord2f(0.9f, 1.0f);
    glVertex3f(float(4 + 1), float(17.5), float(0));
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(float(4), float(17.5), float(0));
    glEnd();
    glEnable(GL_DEPTH_TEST);						// Enable Depth Testing
    glDisable(GL_BLEND);							// Disable Blending
}*/

// Draw frame
void DrawGraphics()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set location in front of camera
    glLoadIdentity();

    //Gets the view squared off a bit and pulls back so it fits
    //Still learning this control.  1st and 4th is x-axis, 2nd and 5th is y-axis, 3rd is zoom
    // (y+10) gives tilted world for pissing about with 3d models instead of sprites

    gluLookAt(Cam.Cx, (Cam.Cy), Cam.Cz, Cam.Cx, Cam.Cy, 0.0, 0.0, 1.0, 0.0);

    int tile;
    int loopy;
    int loopx;

    //tile_properties properties[20][20];
    //Loops through map array, binds texture appropriately and calls to Tile()
    for (loopy = 0; loopy < (MAP_SIZEY); loopy++)
    {
        for (loopx = 0; loopx < (MAP_SIZEX); loopx++)
        {
            //Tiles map, stores properties of tiles in properties array

            tile = Map[loopy][loopx];

            walkability[loopx][loopy]= tile;
            glBindTexture( GL_TEXTURE_2D, texture_id[tile]);
            Tile(loopx,loopy, 0);

            //assign_properties(loopx, loopy, tile, properties[loopx][loopy]);
            if (reportbool)
            {
                //report(properties[loopx][loopy]);
                reportbool--;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* displayWindow;
    SDL_Renderer* displayRenderer;
    SDL_RendererInfo displayRendererInfo;

    SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_RENDERER_PRESENTVSYNC, &displayWindow, &displayRenderer);
    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);

    /*TODO: Check that we have OpenGL */
    if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 ||
    (displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
        /*TODO: Handle this. We have no render surface and not accelerated. */
        return -1;
    }

    SDL_GL_CreateContext(displayWindow);

    InitGraphics();
    ResizeGraphics(800, 600);

    //cout << (int)SDLK_SLEEP << endl;

    SDL_SetWindowTitle(displayWindow, "EloRe v0.1");

    FactionList factionlist;
    //World Init   -   no persistence in the FactionList object yet
//FactionList factionlist;
//factionlist.members[3].site = 3;
    factionlist = RunHistory(100, factionlist);
    int test=0;
    test = factionlist.members[3].site;
    printf("%d\n", test);
    int mysite = factionlist.members[3].site;
    printf("Testing faction on site: %d\n", mysite);
    //Gen a player

    CEntity *player1 = (CEntity*)(new Player(texture_id[3], texture_id[10]));
    player1->init();

    //Gen a primative
    CEntity *primative1 = (CEntity*)(new Primative(texture_id[4]));
    primative1->init();

    bool done = false;
    // Event loop
    while (!done)
    {
        SDL_Event event;

        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                done = true;
            break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    ResizeGraphics(event.window.data1, event.window.data2);
            break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym < 256)
                {
                    if (!keys[event.key.keysym.sym])
                        keyPressed = true;

                    keys[event.key.keysym.sym] = true;
                }

            break;
            case SDL_KEYUP:
                if (event.key.keysym.sym < 256)
                    keys[event.key.keysym.sym] = false;
            break;
            }
        }

        if (!keys[SDLK_x] && playintro)    //Runs intro before game till X
        {
            DrawIntro(displayWindow);
        }
        else
        {
            playintro=0;
            if(keys[SDLK_x])       //Shows pause screen after SPACE till X
            {
                playpause=0;
            }

            if (keys[SDLK_ESCAPE])				// Was ESC Pressed?
            {
                done = true;
                break;			// ESC Signalled A Quit
            }
            else						// Not Time To Quit, Update Screen
            {
                //Movement, guards against type 0, 8 and 7 tiles, checks against held keys
                int movement = player1->move((void*)keys);
                int x, y, camX, camY;

                player1->getPos(x, y);
                camX = Cam.Cx;
                camY = Cam.Cy;

                moved = true;

                switch (movement)
                {
                case 1: // Down+Left
                    --x;
                    --y;
                    --camX;
                    --camY;
                    break;
                case 2: // Down
                    --y;
                    --camY;
                    break;
                case 3: // Down+Right
                    ++x;
                    --y;
                    ++camX;
                    --camY;
                    break;
                case 4: // Right
                    ++x;
                    ++camX;
                    break;
                case 5: // Up+Right
                    ++x;
                    ++y;
                    ++camX;
                    ++camY;
                    break;
                case 6: // Up
                    ++y;
                    ++camY;
                    break;
                case 7: // Up+Left
                    --x;
                    ++y;
                    --camX;
                    ++camY;
                    break;
                case 8: // Left
                    --x;
                    --camX;
                    break;
                default:
                    moved = false;
                }

                if (moved && keyPressed)
                {
                    if (walkability[x][y] != 0 &&
                        walkability[x][y] != 8 &&
                        walkability[x][y] != 7)
                    {
                        player1->setPos(x, y);
                        Cam.Cx = camX;
                        Cam.Cy = camY;

                        DoAI(primative1, player1);
                    }
                    else
                        moved = false;

                    keyPressed = false;
                }
                else
                    moved = false;

                if(keys[SDLK_r] && keyPressed)	//Attack
                {
                    int x1, y1, x2, y2;

                    player1->getPos(x1, y1);
                    primative1->getPos(x2, y2);

                    if((x1 == x2) && (y1 == y2))
                    {
                        int damage;

                        damage = player1->attack();
                        primative1->setLife(primative1->getLife() - damage);

                        if (primative1->getLife() <=0)
                        {
                            primative1->setLife(0);
                            cout << "Primitive has been slain!";
                        }
                        else
                        {
                            cout << "Primitive has: " << primative1->getLife() << endl;
                            cout << "Player has: " << player1->getLife() << endl;
                        }
                    }

                    keyPressed = false;
                }
                if(keys[SDLK_SPACE])  //pause implement
                    playpause=1;

//Map Bounds checks for movement, also restricts camera
                player1->getPos(x, y);

                if (x < 0)
                {
                    x=0;
                    Cam.Cx = 0;
                }
                else if (x > (MAP_SIZEX-1))
                {
                    x  = (MAP_SIZEX-1);
                    Cam.Cx = (MAP_SIZEX-1);
                }

                if (y < 0)
                {
                    y  = 0;
                    Cam.Cy =0;
                }
                else if (y > (MAP_SIZEY-1))
                {
                    y  = (MAP_SIZEY-1);
                    Cam.Cy = (MAP_SIZEY-1);
                }

                player1->setPos(x, y);
//Pause on space
                if (playpause)
                    DrawPause(displayWindow);
                else
                {
                    DrawGraphics();

                    primative1->draw();
                    player1->draw();

                    //testVert();
                    //FPS stuff
                    static int lastTime = (clock()/CLOCKS_PER_SEC);
                    static int framesDone;
                    int currentTime = (clock()/CLOCKS_PER_SEC);
                    if(currentTime > lastTime)
                    {
                        //Dumps number of frames drawn over previous second to console, fires once a second
                        int fps = framesDone;
                        framesDone=0;
                        lastTime = currentTime;
                        printf("Current FPS: %d\n", fps);
                        int mysite = factionlist.members[3].site;
                        printf("Testing faction on site: %d\n", mysite);

                    }
                    framesDone++;
                    cposx = x;
                    cposy = y;
                    SDL_GL_SwapWindow(displayWindow);
                }
                //Remind the character where it is now

                /*primative1.m_xpos = pposx;
                		primative1.m_ypos = pposy;*/
                //If the character moved this cycle this block fires
                if (moved)
                {
                    moved = FALSE;
                    player1->speak();
                    primative1->speak();
                    //Respawner for primitive
                    if(primative1->getLife() <= 0)
                    {
                        if (nohostiles>5)
                        {
                            primative1->init();
                            nohostiles=0;
                        }
                        nohostiles++;
                    }
                }
            }
        }

        SDL_Delay(10);
    }

    IMG_Quit();
    SDL_Quit();

    delete player1;
    delete primative1;

    return 0;
}

