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

#include "primative.h"
#include "framework.h"

#include <iostream>

Primative::Primative(GLuint ch, int life, int xpos, int ypos)
: CEntity(life, xpos, ypos)
{
    texID = ch;
}

Primative::~Primative()
{
}

bool Primative::init()
{
    curLife = maxLife;
    std::cout << "A Primative has been born with life " << curLife << "." << std::endl;

    return true;
}

void Primative::speak()
{
    std::cout << "Primative " << curLife << " and at x: "
              << xPos << " y: " << yPos << std::endl;
}

void Primative::die()
{
}

int Primative::attack()
{
    return 0;
}

void Primative::draw()
{
    if (curLife > 0)
    {
        glEnable(GL_BLEND);

        glBlendFunc(GL_DST_COLOR,GL_ZERO);
        glBindTexture(GL_TEXTURE_2D, texID);
        Tile(xPos, yPos, 1.0);

        glDisable(GL_BLEND);
    }
}

int Primative::move(void* playerPtr)
{
    int toMove = 4;

    if (curLife > 0 && playerPtr)
    {
        CEntity *player = (CEntity*)playerPtr;

        int x1, y1, x2, y2;

        player->getPos(x1, y1);
        this->getPos(x2, y2);

        int diffx, diffy;

        diffx = abs(x1 - x2);
        diffy = abs(y1 - y2);

        if (diffx > diffy)
        {
            if (x1 > x2)
                toMove = 0;
            else if (x1 < x2)
                toMove = 1;
        }
        else
        {
            if (y1 > y2)
                toMove = 2;
            else if (y1 < y2)
                toMove = 3;
        }
    }

    return toMove;
}

