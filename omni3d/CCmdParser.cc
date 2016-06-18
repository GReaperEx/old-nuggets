// Copyright (C) 2013, GReaperEx(Marios F.)
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

#include "CCmdParser.h"

#include <algorithm>

using namespace std;

static bool is_not_alpha(int c)
{
    return !isalpha(c);
}

CCmdParser::CCmdParser(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++){
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == '-')
            {
                string argName(&argv[i][2]), argValue("");
                string::iterator j;

                j = find(argName.begin(), argName.end(), '=');
                if (j != argName.end())
                {
                    argValue.insert(argValue.begin(), j+1, argName.end());
                    argName.erase(j, argName.end());
                }
                else if (i < argc-1 && argv[i+1][0] != '-')
                {
                    argValue = argv[i+1];
                    ++i;
                }

                if (find_if(argName.begin(), argName.end(), is_not_alpha) != argName.end())
                    throw (string("Invalid argument string: ") + argName);

                if (CmdArgs.find(argName) != CmdArgs.end())
                    throw (string("Argument already entered: ") + argName);

                CmdArgs[argName] = argValue;
            }
            else
            {
                string argName(""), argValue("");

                if (!isalpha(argv[i][1]))
                    throw (string("Invalid argument token: ") + argv[i][1]);

                argName += argv[i][1];
                if (argv[i][2] == '=')
                    argValue = &argv[1][3];
                else if (i < argc-1 && argv[i+1][0] != '-')
                {
                    argValue = argv[i+1];
                    ++i;
                }

                if (CmdArgs.find(argName) != CmdArgs.end())
                    throw (string("Argument already entered: ") + argName);

                CmdArgs[argName] = argValue;
            }
        }
    }
}

bool CCmdParser::is_arg_there(const string& argName) const
{
    return CmdArgs.find(argName) != CmdArgs.end();
}

string CCmdParser::get_value_of(const string& argName)
{
    return CmdArgs[argName];
}

