/* serport.h - user friendly RS-232 raw serial port interface
 *  Copyright (C) 2002 Sean "Zekat" Scanlon (zekat@mail.com)
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

#ifndef serport_h
#define serport_h

#include <termios.h>
#include <unistd.h>
#include <stdlib.h>


/* serial port formats */
typedef enum SERPORT_FORMAT {
 F8N1,    /* 8 databits, No parity, 1 stopbit */  
 FCOUNT
} SERPORT_FORMAT;


typedef struct SERPORT {
  char*          device_name;
  int            speed;
  SERPORT_FORMAT format;  
  int            file_des;
  int            is_locked;
  struct termios oldtio;
  struct termios newtio;
} SERPORT;


/* action methods */
extern SERPORT* serport_new( void );
extern void serport_delete( SERPORT* serport );
extern void serport_dump( SERPORT* serport );  /* debug fn */
extern void serport_open( SERPORT* serport );
extern void serport_close( SERPORT* serport );
extern void serport_lock( SERPORT* serport );
extern void serport_unlock( SERPORT* serport );
extern ssize_t serport_read( SERPORT* serport, void* buf, size_t nbyte );
extern ssize_t serport_write( SERPORT* serport, const void* buf, size_t nbyte );

/* accessor methods */
extern void  serport_set_device_name( SERPORT* serport, char* devname );
extern char* serport_get_device_name( SERPORT* serport );
extern void  serport_set_speed( SERPORT* serport, int speed );
extern int   serport_get_speed( SERPORT* serport );
extern void  serport_set_format( SERPORT* serport, SERPORT_FORMAT format );
extern SERPORT_FORMAT serport_get_format( SERPORT* serport );
extern void  serport_set_raw_mode( SERPORT* serport );

#endif

