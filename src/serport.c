/* serport.c - user friendly RS-232 raw serial port interface
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

#include "serport.h"
#include "devlck.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define  NDEBUG   /* uncomment NDEBUG to disable asserts */
#include <assert.h>


/* "constructor" */
SERPORT* serport_new( void )
{
  SERPORT* serport = (SERPORT*) malloc( sizeof(SERPORT) );
  bzero( serport, sizeof(SERPORT) );
  serport->file_des = -1;
  return serport;
}


/* "destructor" */
void serport_delete( SERPORT* serport )
{
  assert( serport );

  serport_close( serport );
  serport_unlock( serport );
  free( serport->device_name );
  free( serport );
  serport = NULL;
}


/* print a serport dump for debugging */
void serport_dump( SERPORT* serport)
{
  assert( serport );

  (void)printf("device name: %s\n", serport->device_name);
  (void)printf("speed:       %d\n", serport->speed);
  (void)printf("format:      %d\n", serport->format);
  (void)printf("file des:    %d\n", serport->file_des);
  (void)printf("locked:      %d\n", serport->is_locked);
}


/* open the serial port in raw mode */
void serport_open( SERPORT* serport )
{
  char buf[256];
  assert( serport );

  serport->file_des = open( serport->device_name, O_RDWR|O_NONBLOCK );
  if ( serport->file_des < 0 ) {
    snprintf( buf, 256, "Could not open %s\n", serport->device_name );
    perror( buf );
  }

  /* save old termios */
  tcgetattr( serport->file_des, &(serport->oldtio) );

  /* set new termios*/
  tcsetattr( serport->file_des, TCSAFLUSH, &(serport->newtio) );
}


/* close the serial port, restore old mode */
void serport_close( SERPORT* serport )
{
  assert( serport );

  /* reset serial port and close */
  if (serport->file_des >= 0) {
    tcsetattr( serport->file_des, TCSAFLUSH, &(serport->oldtio) );
    close( serport->file_des );
  }
}


/* lock the serial port */
void serport_lock( SERPORT* serport )
{
  assert( serport );
  serport->is_locked = 1;
    
  if (-1 == dev_setlock( serport->device_name ) ) {
    serport->is_locked = 0;
  }
}


/* unlock the serial port */
void serport_unlock( SERPORT* serport )
{
  assert( serport );
    
  if (serport->is_locked && (dev_getlock( serport->device_name ) > 0) ) {
    dev_unlock( serport->device_name );
  }
}


/* read from the serial port */
ssize_t serport_read( SERPORT* serport, void* buf, size_t nbyte )
{
  assert( serport );

  return read( serport->file_des, buf, nbyte );
}


/* write to the serial port */
ssize_t serport_write( SERPORT* serport, const void* buf, size_t nbyte )
{
  assert( serport );

  return write( serport->file_des, buf, nbyte );
}


/* set device name */
void serport_set_device_name( SERPORT* serport, char* devname )
{
  assert( serport );

  free( serport->device_name );
  serport->device_name = strdup( devname );
}


/* get device name */
char* serport_get_device_name( SERPORT* serport )
{
  assert( serport );

  return( serport->device_name );
}


/* set speed */
void serport_set_speed( SERPORT* serport, int speed )
{
  assert( serport );

  switch (speed) {
  case B2400: case B4800: case B9600: /* fall thru */
    serport->speed = speed;
    cfsetispeed( &(serport->newtio), serport->speed );
    cfsetospeed( &(serport->newtio), serport->speed );	
    break;
  default:
    break;
  }
}


/* get speed */
int serport_get_speed( SERPORT* serport )
{
  assert( serport );

  return( serport->speed );
}


/* set format */
void serport_set_format( SERPORT* serport, SERPORT_FORMAT format )
{
  assert( serport );

  serport->newtio.c_lflag &= ~(ICANON | IEXTEN | ISIG | ECHO);
  serport->newtio.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON | BRKINT);
  serport->newtio.c_oflag &= ~OPOST;
  serport->newtio.c_cc[VMIN]  = 1;
  serport->newtio.c_cc[VTIME] = 0;

  switch( format ) {
  case F8N1:
    serport->format = F8N1;
    serport->newtio.c_cflag |= CS8;
    break;
  default:
    break;
  }
}


/* get format */
SERPORT_FORMAT serport_get_format( SERPORT* serport )
{
  assert( serport );

  return( serport->format );
}

/* set port to raw i/o mode */
void  serport_set_raw_mode( SERPORT* serport )
{
  assert( serport );
  
  serport->newtio.c_lflag &= ~(ICANON | IEXTEN | ISIG | ECHO);
  serport->newtio.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON | BRKINT);
  serport->newtio.c_oflag &= ~OPOST;
  serport->newtio.c_cflag |= CS8;	
  serport->newtio.c_cc[VMIN]  = 1;
  serport->newtio.c_cc[VTIME] = 0;
}
