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

#ifndef CENTITY_H
#define CENTITY_H

class CEntity
{
protected:
    int maxLife;
    int curLife;

    int xPos;
    int yPos;

    int strength;
    int agility;
    int intelligence;
    int wisdom;
    int charisma;

public:
    CEntity(int life, int xpos, int ypos,
            int str = 10, int agi = 10, int intel = 10, int wis = 10, int cha = 10)
    : maxLife(life), curLife(life), xPos(xpos), yPos(ypos), strength(str),
      agility(agi), intelligence(intel), wisdom(wis), charisma(cha)
    {}

    virtual ~CEntity() { }

    virtual bool init() = 0;
    virtual void speak() = 0;
    virtual void die() = 0;
    virtual int attack() = 0;

    virtual void draw() = 0;
    virtual int move(void*) = 0;

    int getLife() const{
        return curLife;
    }

    void setLife(int newValue){
        if (newValue <= maxLife)
            curLife = newValue;
    }

    void getPos(int& x, int& y) const{
        x = xPos;
        y = yPos;
    }

    void setPos(int x, int y){
        xPos = x;
        yPos = y;
    }
};

#endif // CENTITY_H
