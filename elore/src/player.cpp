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

#include "player.h"
#include "framework.h"

#include <iostream>


Player::Player(GLuint ch, GLuint mask, int life, int xpos, int ypos)
: CEntity(life, xpos, ypos)
{
    texID[0] = ch;
    texID[1] = mask;
}

Player::~Player()
{
    glDeleteTextures(2, texID);
}

bool Player::init()
{
    curLife = maxLife;
    std::cout << "A player has been born." << std::endl;

    return true;
}

void Player::speak()
{
    std::cout << "Player " << curLife << " and at x: "
              << xPos << " y: " << yPos << std::endl;
}

void Player::die()
{
}

int Player::attack()
{
    return 15;
}

void Player::draw()
{
    if (curLife > 0)
    {
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);

        glBlendFunc(GL_DST_COLOR,GL_ZERO);
        glBindTexture(GL_TEXTURE_2D, texID[1]);
        Tile(xPos, yPos, 1.0);
        glBlendFunc(GL_ONE, GL_ONE);

        glBindTexture(GL_TEXTURE_2D, texID[0]);
        Tile(xPos, yPos, 1.0);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }
}

/* 1 - Down+Left
   2 - Down
   3 - Down+Right
   4 - Right
   5 - Up+Right
   6 - Up
   7 - Up+Left
   8 - Left */
int Player::move(void* keyMapPtr)
{
    int toMove = 0;
    bool *keyMap = (bool*)keyMapPtr;

    if (keyMap[SDLK_z])
        toMove = 1;
    else if (keyMap[SDLK_s])
        toMove = 2;
    else if (keyMap[SDLK_c])
        toMove = 3;
    else if (keyMap[SDLK_d])
        toMove = 4;
    else if (keyMap[SDLK_e])
        toMove = 5;
    else if (keyMap[SDLK_w])
        toMove = 6;
    else if (keyMap[SDLK_q])
        toMove = 7;
    else if (keyMap[SDLK_a])
        toMove = 8;

    return toMove;
}

