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

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <fstream>
#include <string>
#include <map>

#include "framework.h"

class ResourceManager
{
private:
    std::map<std::string, unsigned> itsData;

public:
    void loadAllData(const std::string& startDir){
        std::ifstream dirInfo(startDir + "\\DirInfo");
        if (dirInfo.is_open())
        {
            char type;
            std::string name;
            unsigned data;

            while (dirInfo >> type >> name)
            {
                std::string dirName = (startDir + "\\") + name;

                switch (toupper(type))
                {
                case 'D':
                    loadAllData(dirName);
                break;
                case 'F':
                    data = LoadTexture(dirName.c_str(), true);
                    if (!data)
                        std::cerr << "Failed to load resource \"" << dirName << "\"" << std::endl;
                    else
                    {
                        itsData[name] = data;
                        std::cout << "Loaded successfully: " << dirName << std::endl;
                    }
                break;
                }
            }
        }
        else
            std::cerr << "Unable to gather info about directory \"" << startDir << "\"" << std::endl;
    }

    // Can also be used to determine if a resource exists or not
    bool getResourceData(const std::string& name, unsigned* data) const{
        auto iter = itsData.find(name);

        if (iter == itsData.end())
            return false;

        if (data)
            *data = (*iter).second;

        return true;
    }
};

#endif // RESOURCE_MANAGER_H
