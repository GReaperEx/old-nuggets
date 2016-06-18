// Copyright (C) 2014, GReaperEx(Marios F.)
// This file is part of omni3d
/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CReader_STL.h"

#include <algorithm>
#include <cctype>

using namespace std;

bool CReader_STL::checkExt(const string& filename)
{
    string::const_reverse_iterator iter = filename.rbegin();

    if (toupper(*iter++) != 'L' ||
        toupper(*iter++) != 'T' ||
        toupper(*iter++) != 'S' || *iter != '.') // if filename different than "*.stl"
        return false;

    return true;
}

bool CReader_STL::checkSig(const string& filename)
{
    ifstream infile(filename);

    if (infile.is_open())
    {
        string input;

        infile.seekg(0, ios::beg);
        infile >> input;

        if (input == "solid")
            return true;
    }

    return false;
}


CReader_STL::CReader_STL(const string& filename, CVertexBuffer& vBuf, CNormalBuffer& nBuf, CFaceBuffer& fBuf)
: vertexBuffer(vBuf), normalBuffer(nBuf), faceBuffer(fBuf)
{
    if (checkExt(filename))
    {
        if (checkSig(filename))
        {
            isBinary = false;
            infile.open(filename);
        }
        else
        {
            isBinary = true;
            infile.open(filename, ios::binary);
        }
    }
}

bool CReader_STL::loadASCII()
{
    if (!canBeLoaded())
        return false;

    infile.seekg(0, ios::beg);

    string input;

    // Ignoring first line
    infile >> input;
    infile.ignore(numeric_limits<streamsize>::max(), '\n');

    // Read facets it a loop
    while ((infile >> input) && input != "endsolid")
    {
        if (input != "facet")
        {
            omni3d_warn("Expected \"facet\" keyword.");
            return false;
        }

        // Getting the normal

        if (!(infile >> input) || input != "normal")
        {
            omni3d_warn("Expected \"normal\" keyword.");
            return false;
        }

        SNormal normal;
        size_t normIndex;

        if (!(infile >> normal.I >> normal.J >> normal.K))
        {
            omni3d_warn("Expected normal I/J/K components.");
            return false;
        }

        normIndex = normalBuffer.add_normal(normal) + 1;

        SVertex vertex = { 0, 0, 0, 1 };
        size_t vertIndices[3];

        if (!(infile >> input) || input != "outer")
        {
            omni3d_warn("Expected \"outer\" keyword.");
            return false;
        }

        if (!(infile >> input) || input != "loop")
        {
            omni3d_warn("Expected \"loop\" keyword.");
            return false;
        }

        // Getting the three vertices

        for (int i = 0; i < 3; i++){
            if (!(infile >> input) || input != "vertex")
            {
                omni3d_warn("Expected \"vertex\" keyword.");
                return false;
            }

            if (!(infile >> vertex.X >> vertex.Y >> vertex.Z))
            {
                omni3d_warn("Expected vertex X/Y/Z components.");
                return false;
            }

            vertIndices[i] = vertexBuffer.add_vertex(vertex) + 1;
        }

        if (!(infile >> input) || input != "endloop")
        {
            omni3d_warn("Expected \"endloop\" keyword.");
            return false;
        }

        if (!(infile >> input) || input != "endfacet")
        {
            omni3d_warn("Expected \"endfacet\" keyword.");
            return false;
        }

        CFace tempFace;

        tempFace.add_edge(CPoint(vertIndices[0], normIndex));
        tempFace.add_edge(CPoint(vertIndices[1], normIndex));
        tempFace.add_edge(CPoint(vertIndices[2], normIndex));
        faceBuffer.add_face(tempFace);
    }

    if (input != "endsolid")
        omni3d_warn("File doesn't end with \"endsolid\".");

    return true;
}

bool CReader_STL::loadBinary()
{
    if (!canBeLoaded())
        return false;

    infile.seekg(0, ios::beg);

    // Ignoring header
    uint8_t header[80];
    infile.read((char*)header, 80);

    uint32_t facetNum;
    infile.read((char*)&facetNum, sizeof(facetNum));

    for (uint32_t i = 0; i < facetNum; i++){
        SVertex vertex = { 0, 0, 0, 1 };
        SNormal normal;
        float v[3][3];
        float n[3];
        size_t normIndex;
        size_t vertIndices[3];
        uint16_t attrib;

        if (!infile.read((char*)n, sizeof(n)) ||
            !infile.read((char*)v, sizeof(v)) ||
            !infile.read((char*)&attrib, sizeof(attrib)))
        {
            omni3d_warn("File is corrupt or incomplete.");
            return false;
        }

        normal.I = n[0];
        normal.J = n[1];
        normal.K = n[2];

        normIndex = normalBuffer.add_normal(normal) + 1;

        for (int j = 0; j < 3; j++){
            vertex.X = v[j][0];
            vertex.Y = v[j][1];
            vertex.Z = v[j][2];

            vertIndices[j] = vertexBuffer.add_vertex(vertex) + 1;
        }

        CFace tempFace;

        tempFace.add_edge(CPoint(vertIndices[0], normIndex));
        tempFace.add_edge(CPoint(vertIndices[1], normIndex));
        tempFace.add_edge(CPoint(vertIndices[2], normIndex));
        faceBuffer.add_face(tempFace);

        if (attrib != 0)
        {
            stringstream ss;

            ss << "Attribute Byte Count of facet #" << i << " is non-zero.";
            omni3d_warn(ss.str());
        }
    }

    return true;
}
