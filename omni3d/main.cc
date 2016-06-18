// Copyright (C) 2013-2014, GReaperEx(Marios F.)
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

#include <iostream>
#include "CCmdParser.h"

// Included for testing
#include "CVertexBuffer.h"
#include "CNormalBuffer.h"
#include "CTexCoordBuffer.h"
#include "CFaceBuffer.h"

#include "CReader_STL.h"
#include "CWriter_STL.h"

using namespace std;

int main(int argc, char* argv[])
{
    try
    {
        CCmdParser parser(argc, argv);

        if (parser.is_arg_there("v") || parser.is_arg_there("version"))
        {
            cout << "omni3d v" << OMNI3D_VERSION << "\n" << endl;
            // Printing copyright notice
            cout << "Copyright (C) 2013-2014, GReaperEx(Marios F.)\n"
                    "\n"
                    "This program is free software: you can redistribute it and/or modify\n"
                    "it under the terms of the GNU General Public License as published by\n"
                    "the Free Software Foundation, either version 3 of the License, or\n"
                    "(at your option) any later version.\n"
                    "\n"
                    "This program is distributed in the hope that it will be useful,\n"
                    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                    "GNU General Public License for more details.\n"
                    "\n"
                    "You should have received a copy of the GNU General Public License\n"
                    "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
                 << endl;
        }
        else if (parser.is_arg_there("h") || parser.is_arg_there("help"))
        {
            if (parser.get_value_of("help") != "" || parser.get_value_of("h") != "")
            {
                string helpIssue;

                if (parser.is_arg_there("h"))
                    helpIssue = parser.get_value_of("h");
                else
                    helpIssue = parser.get_value_of("help");
            }
            else
            {
                // Printing general help
                cout << "Omniversal 3D models converter\n"
                        "\n"
                        "Converts the input file format to the output one, with optional filters.\n"
                        "Formats are determined by header signatures, cmd-line args and file extensions.\n"
                        "While argument for input format is optional, if used and the file's header\n"
                        "signature and/or extension is invalid, conversion fails.\n"
                        "\n"
                        "Commands:\n"
                        "                    -v:\n"
                        "             --version: Prints version info and exits\n"
                        "\n"
                        "          -h[[=]topic]:\n"
                        "      --help[[=]topic]: Prints helpful info and exits\n"
                        "\n"
                        "        -i[=]file_path:\n"
                        "   --ifile[=]file_path: Sets a file for input( mandatory )\n"
                        "\n"
                        "        -o[=]file_path:\n"
                        "   --ofile[=]file_path: Sets a file for output( optional )\n"
                        "\n"
                        "        -I[=]format_id:\n"
                        " --iformat[=]format_id: Sets the input file's format( optional )\n"
                        "\n"
                        "        -O[=]format_id:\n"
                        " --oformat[=]format_id: Sets the output file's format( mandatory )\n"
                        "        -F[=]format_id:\n"
                        "  --filter[=]filter_id: Sets an optional filter for output\n"
                        "\n"
                        "Supported formats:\n"
                        "    Input:\n"
                        "            STL\n"
                        "    Output:\n"
                        "            STL\n"
                        "\n"
                        "Supported filters:\n"
                        "    ASCII: Output file is in ASCII, if possible\n"
                        "      BIN: Output file is in binary, if possible( default )\n"
                     << endl;
            }
        }
        else
        {
            // Here all the conversion branching is done

            string inFile("");
            string outFile("");

            CVertexBuffer vBuf;
            CNormalBuffer nBuf;
            CTexCoordBuffer tBuf;
            CFaceBuffer fBuf;

            if (parser.is_arg_there("i"))
                inFile = parser.get_value_of("i");
            else if (parser.is_arg_there("ifile"))
                inFile = parser.get_value_of("ifile");

            if (parser.is_arg_there("o"))
                outFile = parser.get_value_of("o");
            else if (parser.is_arg_there("ofile"))
                outFile = parser.get_value_of("ofile");

            if (inFile == "")
            {
                cerr << "omni3d: Error: No input file specified." << endl;

                return -3;
            }

            string iFormat = "";
            string oFormat = "";

            if (parser.is_arg_there("I"))
                iFormat = parser.get_value_of("I");
            else if (parser.is_arg_there("iformat"))
                iFormat = parser.get_value_of("iformat");

            if (parser.is_arg_there("O"))
                oFormat = parser.get_value_of("O");
            else if (parser.is_arg_there("oformat"))
                oFormat = parser.get_value_of("oformat");

            string filter = "";

            if (parser.is_arg_there("F"))
                filter = parser.get_value_of("F");
            else if (parser.is_arg_there("filter"))
                filter = parser.get_value_of("filter");

            if (oFormat == "")
            {
                cerr << "omni3d: Error: No output format specified." << endl;

                return -4;
            }

            if (iFormat == "STL")
            {
                CReader_STL read_STL(inFile, vBuf, nBuf, fBuf);
                if (!read_STL.canBeLoaded() || !read_STL.loadFormat())
                {
                    omni3d_warn("Unable to read \"" + inFile + "\".");
                    return -5;
                }
            }
            else if (iFormat == "")
            {
                CReader_STL read_STL(inFile, vBuf, nBuf, fBuf);
                /*
                CReader_...
                */

                //! Checks ALL formats for a match

                if (read_STL.canBeLoaded())
                {
                    if (!read_STL.loadFormat())
                    {
                        omni3d_warn("Unable to read \"" + inFile + "\".");
                        return -5;
                    }
                }
                /*
                else if (...)
                {
                }
                */
            }
            else
            {
                omni3d_warn("Input format " + iFormat + " not supported.");
                return 0;
            }

            if (oFormat == "STL")
            {
                if (outFile == "")
                    outFile = inFile + ".stl";

                bool isBin = true;

                if (filter == "ASCII")
                    isBin = false;
                else if (filter != "BIN" && filter != "")
                    omni3d_warn("Filter option " + filter + " not supported.");

                CWriter_STL write_STL(outFile, vBuf, nBuf, fBuf, "", isBin);
                if (!write_STL.saveFormat())
                    omni3d_warn("Unable to write \"" + outFile + "\".");
            }
            else
                omni3d_warn("Output format " + oFormat + " not supported.");
        }
    }
    catch (string error)
    {
        cerr << "omni3d: Error: " << error << endl;

        return -1;
    }
    catch (...)
    {
        cerr << "omni3d: Unknown error!" << endl;
        return -2;
    }

    return 0;
}
