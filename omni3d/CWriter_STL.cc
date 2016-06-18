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

#include "CWriter_STL.h"

#include <iomanip>

using namespace std;

bool CWriter_STL::checkExt(const string& filename)
{
    string::const_reverse_iterator iter = filename.rbegin();

    if (toupper(*iter++) != 'L' ||
        toupper(*iter++) != 'T' ||
        toupper(*iter++) != 'S' || *iter != '.') // if filename different than "*.stl"
        return false;

    return true;
}

CWriter_STL::
CWriter_STL(const string& filename,
            CVertexBuffer& vBuf, CNormalBuffer& nBuf, CFaceBuffer& fBuf,
            const std::string& sName, bool isBin)
: isBinary(isBin), solidName(sName), vertexBuffer(vBuf), normalBuffer(nBuf), faceBuffer(fBuf)
{
    if (checkExt(filename))
    {
        if (isBinary)
            outfile.open(filename);
        else
            outfile.open(filename, ios::binary);
    }
}

bool CWriter_STL::saveASCII()
{
    if (!canBeSaved())
        return false;

    outfile << "solid " << solidName << endl;
    for (size_t i = 0; i < faceBuffer.count_faces(); i++){
        CFace& face = faceBuffer.get_face(i);

        // A face may have more than three edges
        for (size_t j = 2; j < face.count_edges(); j++){
            SNormal norm;

            SVertex a, b, c;

            a = vertexBuffer.get_vertex(face.get_edge(0).get_vertex() - 1);
            b = vertexBuffer.get_vertex(face.get_edge(1).get_vertex() - 1);
            c = vertexBuffer.get_vertex(face.get_edge(j).get_vertex() - 1);

            calcNormal(norm, a, b, c);

            outfile << "facet normal ";
            outfile << scientific << norm.I << " " << norm.J << " " << norm.K << endl;
            outfile << "\touter loop" << endl;
            outfile << "\t\tvertex ";
            outfile << scientific << a.X << " " << a.Y << " " << a.Z << endl;
            outfile << "\t\tvertex ";
            outfile << scientific << b.X << " " << b.Y << " " << b.Z << endl;
            outfile << "\t\tvertex ";
            outfile << scientific << c.X << " " << c.Y << " " << c.Z << endl;
            outfile << "\tendloop" << endl;
            outfile << "endfacet" << endl;
        }
    }
    outfile << "endsolid" << endl;

    return true;
}

bool CWriter_STL::saveBinary()
{
    if (!canBeSaved())
        return false;

    uint8_t header[80] = { 0 };
    outfile.write((char*)header, sizeof(header));
    uint32_t faceNum = faceBuffer.count_faces();

    outfile.write((char*)&faceNum, sizeof(faceNum));

    for (size_t i = 0; i < faceNum; i++){
        CFace& face = faceBuffer.get_face(i);

        // A face may have more than three edges
        for (size_t j = 2; j < face.count_edges(); j++){
            SNormal norm;

            SVertex a, b, c;

            a = vertexBuffer.get_vertex(face.get_edge(0).get_vertex() - 1);
            b = vertexBuffer.get_vertex(face.get_edge(1).get_vertex() - 1);
            c = vertexBuffer.get_vertex(face.get_edge(j).get_vertex() - 1);

            calcNormal(norm, a, b, c);

            scalar_t n[3], v[3][3];
            uint16_t attrib = 0;

            n[0] = norm.I;
            n[1] = norm.I;
            n[2] = norm.I;

            v[0][0] = a.X;
            v[0][1] = a.Y;
            v[0][2] = a.Z;
            v[1][0] = b.X;
            v[1][1] = b.Y;
            v[1][2] = b.Z;
            v[2][0] = c.X;
            v[2][1] = c.Y;
            v[2][2] = c.Z;

            outfile.write((char*)n, sizeof(n));
            outfile.write((char*)v, sizeof(v));
            outfile.write((char*)&attrib, sizeof(attrib));
        }
    }

    return true;
}

