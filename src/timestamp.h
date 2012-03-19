/* timestamp.h - user friendly timestamp interface
 *  Copyright (C) 2003 Sean "Zekat" Scanlon (zekat@mail.com)
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

#ifndef timestamp_h
#define timestamp_h

#include <stdlib.h>
#include <config.h>

#ifdef HAVE_SYS_TIMEB_H
#include <sys/timeb.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#define TIMESTAMP_BUF_SIZE (255)

typedef struct TIMESTAMP {
  char         buf[255];

#ifdef HAVE_SYS_TIMEB_H
  struct timeb tstamp;
#else

#ifdef HAVE_TIME_H
  time_t       tstamp;
#endif

#endif

} TIMESTAMP;


/* action methods */
extern TIMESTAMP* timestamp_new( void );
extern void timestamp_delete( TIMESTAMP* timestamp );
extern void timestamp_now( TIMESTAMP* timestamp );

/* accessor methods */
extern ssize_t  timestamp_sprint( TIMESTAMP* timestamp,
                                  char* buf,
                                  size_t nbyte );

#endif
