It always frustrated me that almost NO image tool can handle RAW image
files. That's why I built my own! It's nothing fancy, it takes files from
the cmd-line and asks for the images' dimensions. I programmed it in less
than 10 minutes :-p

Keep in mind that it asks for the images' dimensions only once, therefore
all RAW files should have the same width and height.

It compiles with Standard C++( currently C++11 ), optimal compiling settings
would be:
    gcc -Wall -O2 -DNDEBUG -s -std=gnu++11 -o output_name raw2bmp.cc


Copyright (C) 2014 GReaperEx(Marios F.)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
