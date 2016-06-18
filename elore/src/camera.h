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

#ifndef CAMERA_H
#define CAMERA_H

//This is no longer used, replaced by CamInfo struct in framework.h
class Camera
{
public:
    Camera(int zoom = 20, int xpos = 3, int ypos = 17);
    int m_zoom;
    int m_xpos;
    int m_ypos;


    void Die();
    void setCamera(int x, int y, int currentZoom);
    void Speak();
    void initCamera();

};

#endif // CAMERA_H
