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

#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <cstdio>
#include <cstdint>
#include <ctime>

#include "player.h"
#include "primative.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "resource.h"
#include "camera.h"
#include "world.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

#define MAP_SIZEX 30
#define MAP_SIZEY 30
using namespace std;
// A struct to hold camera data
struct CamInfo
{
    int Cx,Cy,Cz;
};
struct Entity
{
    int id;
    string name;
    string location;
};
struct Faction
{
    int site;
    string name;
    Entity members[10];
};
struct FactionList
{
    Faction members[5];
};


static GLuint LoadTexture( const char * filename, int wrap )
{
    GLuint texture;
    SDL_Surface* surface;
    GLuint format;

    surface = IMG_Load(filename);
    if (!surface)
        return 0;

    SDL_LockSurface(surface);

    // allocate a texture name
    glGenTextures( 1, &texture );

    // select our current texture
    glBindTexture( GL_TEXTURE_2D, texture );

    // select modulate to mix texture with color for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // when texture area is small, bilinear filter the closest MIP map
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first MIP map
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                     wrap ? GL_REPEAT : GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                     wrap ? GL_REPEAT : GL_CLAMP );

    Uint32 surfaceFormat = surface->format->format;

    if (surfaceFormat == SDL_PIXELFORMAT_RGB888 || surfaceFormat == SDL_PIXELFORMAT_RGB24)
        format = GL_RGB;
    else if (surfaceFormat == SDL_PIXELFORMAT_BGR888 || surfaceFormat == SDL_PIXELFORMAT_BGR24)
        format = GL_BGR;
    else
    {
        cout << "Failed to make texture of " << filename << endl;

        SDL_UnlockSurface(surface);
        SDL_FreeSurface(surface);
        return 0;
    }

    // build our texture MIP maps
    gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, surface->w,
                       surface->h, format, GL_UNSIGNED_BYTE, surface->pixels );

    SDL_UnlockSurface(surface);
    SDL_FreeSurface(surface);

    return texture;
}

// Draw a tile

static void Tile(int x, int y, int z)
{
    /*rtile = 180;
    glRotatef(rtile,0.0f,0.0f,0.f);  //Rotate tile 180*/

    glBegin(GL_QUADS);
    //glColor3d(1,1,1);  //Quad is white under texture
    glTexCoord2f(0.9f, 1.0f);
    glVertex3i(x, y, z);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3i(x + 1, y, z);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3i(x + 1, y + 1, z);
    glTexCoord2f(0.9f, 0.0f);
    glVertex3i(x, y + 1, z);
    glEnd();
}

#endif // FRAMEWORK_H
