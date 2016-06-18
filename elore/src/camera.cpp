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

#include "camera.h"

#include <stdio.h>
//This is no longer used....replaced by CamInfo struct in framework.h

Camera::Camera(int zoom, int xpos, int ypos):m_zoom(zoom),m_xpos(xpos),m_ypos(ypos) {}
void Camera::Die()
{

}

void Camera::setCamera(int x, int y, int currentZoom)
{
    m_xpos = x;
    m_ypos = y;
    m_zoom = currentZoom;
}

void Camera::Speak()
{
    printf("Camera at x: %d", m_xpos);
    printf(" y: %d\n",m_ypos);
}

void Camera::initCamera()
{

    m_zoom = 20;


    printf("A Camera has been born.\n");
    m_xpos = 3;
    m_ypos = 17;


}
