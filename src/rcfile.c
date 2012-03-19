/*  rcfile.c - moderately hacked up by Sean Scanlon
 *  to support RadioShack 22-812 DMM with RS232.
 *  Please blame me for any bugs, not Yan!
 *  Copyright (C) 2002 Sean "Zekat" Scanlon (zekat@mail.com)
 *
 *  rcfile.c - originally created by Yan "Warrior" Gurtovoy
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"
#include "rcfile.h"

void str2upper(char *str) {
/* converts given string to upper case */
    int i = 0;
    while (str[i]) toupper(str[i++]);
    return;
}

void str2lower(char *str) {
/* converts given string to lower case */
    int i = 0;
    while (str[i]) tolower(str[i++]);
    return;
}

/* callback functions */

void rc_get_bytes(tty_struct *ptr, char *value) {
    str2upper(value);
    ptr->dspbytes = strcmp(ON, value) ? FALSE : TRUE;
    return;
}

void rc_get_tstamp(tty_struct *ptr, char *value) {
    str2upper(value);
    ptr->tstamp = strcmp(ON, value) ? FALSE : TRUE;
    return;
}

void rc_get_nolock(tty_struct *ptr, char *value) {
    str2upper(value);
    ptr->nolock = strcmp(ON, value) ? FALSE : TRUE;
    return;
}

void rc_get_verbose(tty_struct *ptr, char *value) {
    str2upper(value);
    ptr->verbose = strcmp(ON, value) ? FALSE : TRUE;
    return;
}
/* end of callback functions */

void readRC(tty_struct *ptr) {
/* read settings from rc-file .zmeterrc */

    char buffer[256];
    char name[NAMELEN + 1];
    char value[VALUELEN + 1];
    int  i;
    FILE *fp;

    /* get path to the rc-file */
    sprintf(buffer, "%s/%s", getenv("HOME"), RCFNAME);
    /* try to open rc-file */
    if (!(fp = fopen(buffer, "r"))) {
        return;
    } else {
        /* read rc-file line by line */
        while (fgets(buffer, 256, fp)) {
            /* check for comments */
            if (buffer[0] != '#') {
                /* get data */
                sscanf(buffer, "%s %[^\n]", name, value);
                i = 0;
                /* if identificator is valid, process data */
                while (rc_data[i].name && strcmp(rc_data[i].name, name)) i++;
                if (rc_data[i].name) rc_data[i].fn(ptr, value);
            }
        }
        fclose(fp);
    }    
}
