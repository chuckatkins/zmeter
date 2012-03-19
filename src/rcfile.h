/*  rcfile.h - moderately hacked up by Sean Scanlon
 *  to support RadioShack 22-812 DMM with RS232.
 *  Please blame me for any bugs, not Yan!
 *  Copyright (C) 2002 Sean "Zekat" Scanlon (zekat@mail.com)
 *
 *  rcfile.h - originally created by Yan "Warrior" Gurtovoy
 *  Copyright (C) 2001 Yan "Warrior" Gurtovoy (ymg@azstarnet.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#define RCFNAME  ".zmeterrc"
#define NAMELEN  11
#define VALUELEN 256
#define ON       "ON"
#define OFF      "OFF"

typedef struct _rc_struct {
    char *name;
    void (*fn)();
} rc_struct;

/* external functions */

/* callback function prototypes */
void rc_get_bytes(tty_struct *ptr, char *value);
void rc_get_tstamp(tty_struct *ptr, char *value);
void rc_get_nolock(tty_struct *ptr, char *value);
void rc_get_verbose(tty_struct *ptr, char *value);

rc_struct rc_data[] = {
/*  identificator,  function to call */
    {"NOLOCK",      rc_get_nolock},
    {"TIMESTAMP",   rc_get_tstamp},
    {"TOTALBYTES",  rc_get_bytes},
    {"VERBOSE",     rc_get_verbose},
    {NULL,         NULL}
};
