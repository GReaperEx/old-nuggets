//! Copyright (C) 2014 GReaperEx(Marios F.)
//! This file is part of raw2bmp
/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <cstring>

using namespace std;

void raw2bmp(const string& filename, int width, int height);

int main(int argc, char* argv[])
{
    int width, height;

    cout << "Give raw file dimensions: ";
    cin >> width >> height;

    for (int i = 1; i < argc; i++){
        raw2bmp(argv[i], width, height);
    }

    return 0;
}

#pragma pack(push, 1)
typedef struct
{
    uint16_t sig;
    uint32_t fileSize;
    uint16_t _res1;
    uint16_t _res2;
    uint32_t dataOffset;
}BmpFileHeader;
#pragma pack(pop)

typedef struct
{
    uint32_t headerSize;
     int32_t bmpWidth;
     int32_t bmpHeight;
    uint16_t colorPlanes;
    uint16_t bmpBPP;
    uint32_t compression;
    uint32_t dataSize;
     int32_t xRes;
     int32_t yRes;
    uint32_t paletteColors;
    uint32_t importantColors;
}BmpInfoHeader;

void raw2bmp(const string& filename, int width, int height)
{
    string newFileName = filename;
    string::iterator where = find(newFileName.begin(), newFileName.end(), '.');

    if (where != newFileName.end())
    {
        newFileName.erase(where + 1, newFileName.end());
        newFileName += "bmp";
    }

    ifstream infile(filename.c_str(), ios::binary);
    if (infile.is_open())
    {
        BmpFileHeader bfh;
        BmpInfoHeader bih;
        uint8_t *imgData;

        bfh.sig = 0x4D42;
        bfh.fileSize = sizeof(bfh) + sizeof(bih) + width*height*3;
        bfh._res1 = bfh._res2 = 0;
        bfh.dataOffset = sizeof(bfh) + sizeof(bih);

        bih.headerSize = sizeof(bih);
        bih.bmpWidth = width;
        bih.bmpHeight = height;
        bih.colorPlanes = 1;
        bih.bmpBPP = 24;
        bih.compression = 0;
        bih.dataSize = width*height*3;
        bih.xRes = bih.yRes = 0;
        bih.paletteColors = 0;
        bih.importantColors = 0;

        imgData = new uint8_t[bih.dataSize];

        infile.read((char*)imgData, bih.dataSize);
        //! Invert R and B values
        for (uint32_t i = 0; i < bih.dataSize; i += 3){
            uint8_t temp;

            temp = imgData[i];
            imgData[i] = imgData[i+2];
            imgData[i+2] = temp;
        }

        uint8_t *temp = new uint8_t[bih.bmpWidth*3];
        //! Invert vertically
        for (int32_t i = 0; i < bih.bmpHeight/2; i++){
            memcpy(temp, &imgData[i*(bih.bmpWidth*3)], bih.bmpWidth*3);
            memcpy(&imgData[i*(bih.bmpWidth*3)], &imgData[(bih.bmpHeight - i - 1)*(bih.bmpWidth*3)], bih.bmpWidth*3);
            memcpy(&imgData[(bih.bmpHeight - i - 1)*(bih.bmpWidth*3)], temp, bih.bmpWidth*3);
        }

        ofstream outfile(newFileName, ios::binary);
        if (outfile.is_open())
        {
            outfile.write((char*)&bfh, sizeof(bfh));
            outfile.write((char*)&bih, sizeof(bih));
            outfile.write((char*)imgData, bih.dataSize);
        }

        delete [] imgData;
    }
}
