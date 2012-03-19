
/*  slsnif.h - moderately hacked up by Sean Scanlon
 *  to support RadioShack 22-812 DMM with RS232.
 *  Please blame me for any bugs, not Yan!
 *  Copyright (C) 2002 Sean "Zekat" Scanlon (zekat@mail.com)
 *
 *  slsnif.h - originally created by Yan "Warrior" Gurtovoy
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

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#include "common.h"
#include "rs22812.h"
#include "serport.h"
#include "timestamp.h"


#define OPTSTR      "bnhtvVs:" /* list of short options for getopt_long() */
#define TOTALBYTES  "Total bytes transmitted:"
#define MEMFAIL     "Memory allocation failed"
#define PORTFAIL    "Failed to open port"
#define LOGFAIL     "Failed to open log file, defaulting to stdout"
#define SELFAIL     "`Select` failed"
#define RPORTFAIL   "Error reading from port"
#define CLOSEFAIL   "Failed to close file - data loss may occur"
#define RAWFAIL     "Failed to switch into raw mode"
#define SYNCFAIL    "Failed to syncronize ttys"
#define PIPEFAIL    "Failed to create pipe"
#define FORKFAIL    "Failed to create child process"
#define RPIPEFAIL   "Error reading from pipe"

#define max(x,y)	((x) > (y) ? (x) : (y))

#define INBUFFSIZE  32
#define OUTBUFFSIZE (INBUFFSIZE * 16 + 1)
#define PRFXSIZE    12
#define DEFBAUDRATE B4800
#define PORTNAMELEN 11

extern char* optarg;  /* for getopt */

static TIMESTAMP       *timeStamp   = (TIMESTAMP*)NULL;
static SERPORT         *serialPort  = (SERPORT*)NULL;
static RS22812_PACKET  *meterPacket = (RS22812_PACKET*)NULL;
static tty_struct      tty_data;
static pid_t           pid = -1;

/* function prototypes */
void readRC(tty_struct *ptr);
