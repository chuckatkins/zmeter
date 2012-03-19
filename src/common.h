/*  common.h - moderately hacked up by Sean Scanlon
 *  to support RadioShack 22-812 DMM with RS232.
 *  Please blame me for any bugs, not Yan!
 *  Copyright (C) 2002 Sean "Zekat" Scanlon (zekat@mail.com)
 *
 *  common.h - originally created by Yan "Warrior" Gurtovoy
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

#ifndef _COMMON_H
#include <termios.h>
#include <string.h>

#ifndef FALSE
#define FALSE       0
#endif
#ifndef TRUE
#define TRUE        1
#endif

/* read and write sides of pipe, respectively */
#define RPIPE (0)
#define WPIPE (1)

typedef struct _tty_struct {
    int             logfd;
    int             portraw;
    int             cstatpipefd[2];
    int             portpipefd[2];
    int             dspbytes;
    int             tstamp;
    int             nolock;
    struct termios  portstate_orig;
    speed_t         baudrate;
    int             verbose;
    int             maxSamples;
    int             numSamples;
} tty_struct;

#define _COMMON_H
#endif
