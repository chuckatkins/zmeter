/* timestamp.c - user friendly timestamp interface
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

#include "timestamp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define  NDEBUG   /* uncomment NDEBUG to disable asserts */
#include <assert.h>


/* "constructor" */
TIMESTAMP* timestamp_new( void )
{
  TIMESTAMP* timestamp = (TIMESTAMP*) malloc( sizeof(TIMESTAMP) );
  bzero( timestamp, sizeof(TIMESTAMP) );
  snprintf( timestamp->buf, TIMESTAMP_BUF_SIZE, "Timestamp Unavailable" );
  return timestamp;
}


/* "destructor" */
void timestamp_delete( TIMESTAMP* timestamp )
{
  assert( timestamp );

  free( timestamp );
  timestamp = NULL;
}


/* acquire the current time */
void timestamp_now( TIMESTAMP* timestamp)
{
#ifdef HAVE_SYS_TIMEB_H
  char tbuf[29];
  char tmp[25];
  char tmp1[4];
#endif

  assert( timestamp );

#ifdef HAVE_SYS_TIMEB_H
  ftime(&(timestamp->tstamp));
  tmp[0] = tmp1[0] = tbuf[0] = 0;
  strncat(tmp, ctime(&(timestamp->tstamp.time)), 24);
  strncat(tbuf, tmp, 19);
  sprintf(tmp1, ".%2ui", timestamp->tstamp.millitm);
  strncat(tbuf, tmp1, 3);
  strcat(tbuf, tmp + 19);
  snprintf( timestamp->buf, TIMESTAMP_BUF_SIZE, tbuf );
#else

#ifdef HAVE_TIME_H
  time(&tstamp);
  snprintf( timestamp->buf, TIMESTAMP_BUF_SIZE, ctime(&tstamp) );
#endif
#endif
}


/* get the time stamp */
ssize_t timestamp_sprint( TIMESTAMP* timestamp, char* buf, size_t nbyte )
{
  assert( timestamp );
  
  return snprintf( buf, nbyte, timestamp->buf );
}
