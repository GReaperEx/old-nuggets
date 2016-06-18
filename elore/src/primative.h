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

#ifndef PRIMATIVE_H
#define PRIMATIVE_H

#include "CEntity.h"
#include <GL/gl.h>

class Primative : public CEntity
{
private:
    GLuint texID;

public:
    Primative(GLuint ch, int life = 100, int xpos = 17, int ypos = 17);
    ~Primative();

    bool init();
    void speak();
    void die();
    int attack();

    void draw();
    int move(void*);

};

#endif // PRIMATIVE_H
