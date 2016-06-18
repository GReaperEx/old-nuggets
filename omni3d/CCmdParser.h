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

#ifndef _C_CMD_PARSER_H_
#define _C_CMD_PARSER_H_

#include <map>
#include <string>

/** Command line argument parser
    Valid arguments:
        -<alphabetical_char> value ( with or without blanks in-between )
        -<alphabetical_char>=value ( with *no* blanks in-between )
        --<alphabetical_string>=value ( with *no* blanks in-between )
        --<alphabetical_string> value ( with *one or more* blanks in-between ) */
class CCmdParser
{
    private:
        std::map<std::string, std::string> CmdArgs;

    public:
        //! Parses and saves the arguments
        //! Raises exception if it finds duplicated or invalid arguments
        CCmdParser(int argc, char* argv[]);

        bool is_arg_there(const std::string& argName) const;
        //! Returns the value associated with that argument
        /** For example:
            User input: omni3d -o output.obj
            Method: get_value_of("o") = "output.obj" */
        std::string get_value_of(const std::string& argName);
};

#endif // _C_CMD_PARSER_H_
