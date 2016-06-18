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

#include "framework.h"


//Currently always 5 factions with 10 entities each. TODO: dynamic sizing

FactionList RunHistory(int span, FactionList factionlist)
{
    printf("Running World History, Span: %d years\n", span);

    int masterid = 0;
//naming and locating factions from the list
    string s0 ("Faction");
    string s1 ("Entity");
    int x;

    for(x=0; x<5; x++)
    {
        factionlist.members[x].site = x;
        factionlist.members[x].name = s0;

        //naming and locating entities per faction.
        int y;
        for(y=0; y<9; y++)
        {
            stringstream ss;

            ss << masterid;
            factionlist.members[x].members[y].id = masterid;
            factionlist.members[x].members[y].name = s1+ss.str();
            factionlist.members[x].members[y].location = factionlist.members[x].site;
            masterid++;
        }
    }

//debug display of fac sites
    int i;
    for(i=0; i<5; i++)
    {
        printf("A faction claims site: %d\n", factionlist.members[i].site);
    }

    //debug passage of time
    int z;
    for (z=0; z<span; z++)
    {
        //a year has passed, do some stuff
        printf(".");
        if (z==span)
        {
            printf("HistoryComplete");
        }

        return factionlist;
    }

    return factionlist;
}
