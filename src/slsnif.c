/*  slsnif.c - moderately hacked up by Sean Scanlon
 *  to support RadioShack 22-812 DMM with RS232.
 *  Please blame me for any bugs, not Yan!
 *  Copyright (C) 2002 Sean "Zekat" Scanlon (zekat@mail.com)
 *
 *  slsnif.c - originally created by Yan "Warrior" Gurtovoy
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "slsnif.h"
#include "devlck.h"

/*#define  NDEBUG */   /* uncomment NDEBUG to disable asserts */
#include <assert.h>

void writeDataFromPort(int in, int out);
void writeDataFromPipe(int in, int out);


void copyright() {
    printf("\n\nZmeter Version %s\n", VERSION);
    printf("\tRadioShack 22-812 Digital Multi-Meter Interface. \n");
    printf("\tCopyright (C) 2002 Sean \"Zekat\" Scanlon (zekat@mail.com)\n\n");
    printf("\tDerived from:\n");
    printf("Serial Line Sniffer. Version 0.4.1\n");
    printf("\tCopyright (C) 2001 Yan \"Warrior\" Gurtovoy (ymg@azstarnet.com)\n\n");
}

void usage() {
    printf("Usage: zmeter [options] <port>\n\n");
    printf("REQUIRED PARAMETERS:\n");
    printf("  <port>     - serial port to use (i.e /dev/ttyS0, /dev/ttyS1, etc.)\n\n");
    printf("OPTIONS:\n");
    printf("  -h (--help)       - displays this help.\n");
    printf("  -b (--bytes)      - print number of bytes transmitted on every read.\n");
    printf("  -n (--nolock)     - don't try to lock the port.\n");
    printf("  -s (--samples) N  - take N samples then exit.\n");
    printf("  -t (--timestamp)  - print timestamp for every transmission.\n");
    printf("  -v (--verbose)    - print extra info besides measurement result.\n");
    printf("  -V (--version)    - print version and copyright information.\n");
    printf("Example: zmeter -n /dev/ttyS1\n\n");
}

void fatalError(char *msg) {
    perror(msg);
    _exit(-1);
}

int setRaw(int fd, struct termios *ttystate_orig) {
/* set tty into raw mode */

    struct termios    tty_state;
	
    if (tcgetattr(fd, &tty_state) < 0) return 0;
    /* save off original settings */
    *ttystate_orig = tty_state;
    /* set raw mode */
    tty_state.c_lflag &= ~(ICANON | IEXTEN | ISIG | ECHO);
    tty_state.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON | BRKINT);
    tty_state.c_oflag &= ~OPOST;
    tty_state.c_cflag |= CS8;	
    tty_state.c_cc[VMIN]  = 1;
    tty_state.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSAFLUSH, &tty_state) < 0) return 0;
    return 1;
}

void fmtData(unsigned char *in, char *out, int in_size) {
/* format data */

  char  charbuf[15];
  int   i;

  /* flush output buffer */
  out[0] = 0;
    
  if (in_size >= 8) {
    /* new (meter packet) pretty print to string */
    rs22812_packet_read  ( meterPacket, in );
    rs22812_packet_cook  ( meterPacket );
#ifdef DEBUG
    rs22812_packet_dump  ( meterPacket );  /* debug fn */
#endif
    rs22812_packet_sprint( meterPacket, out, OUTBUFFSIZE );
   }

  else {
    printf("whoa! fmtData didn\'t get enough data--only %d bytes\n",
    in_size);
    /* old (raw) hex dump */
    for (i = 0; i < in_size; i++) {
      /* it's a data byte */
      sprintf(charbuf, "(%02x) ", in[i] & 0xff);

      /* put formatted data into output buffer */
      strcat(out, charbuf);
    }
  }
}

void writeData(int in, int out, int aux, int mode) {
    printf(":TODO: writeData call that has not yet been converted found!\n");
    switch(mode) {
    case 0:  writeDataFromPipe(in, out);  break;
    case 1:  writeDataFromPort(in, out);  break;
    default: assert(0);  
    }
}

void writeDataFromPipe(int in, int out) {
/* reads data from `in`, formats it, writes it to `out`.
 * mode 0 - read from pipe
 */

    unsigned char   buffer[INBUFFSIZE];
    char            outbuf[OUTBUFFSIZE];
    int             n;
    char            tstamp[30];
    int             tstamp_len;

    if ((n = read(in, buffer, INBUFFSIZE)) < 0) {  /* read nothing */
        perror(RPIPEFAIL);
    }
    
    else {  /* successfully read something from somewhere */
        if (n >=8) {  /* need at least 8 bytes for a meter packet */
        /* read from pipe */
            /* print timestamp if necessary */
            if (tty_data.tstamp) {
                timestamp_now( timeStamp );
                tstamp_len = timestamp_sprint( timeStamp, tstamp, 30 );
                write(out, tstamp, tstamp_len);
                write(out, "\t", 1);
            }
               
            /* format data */
            fmtData(buffer, outbuf, n);
            /* print data */
            write(out, outbuf, strlen(outbuf));
                
            /* count (decoded packet) writes */
            tty_data.numSamples++;
                
            /* print total number of bytes if necessary */
            if (tty_data.dspbytes) {
                buffer[0] = 0;
                sprintf(buffer, "\t%s %i\n", TOTALBYTES, n);
                write (out, buffer, strlen(buffer));
            }
        } /* end if n >= 8 */
    } /* end else there were characters to read */
} /* end writeData() */

void writeDataFromPort(int in, int out) {
/* reads data from `in`, formats it, writes it to `out`.
 * out    - the file descriptor (pipe) the child process reads
 * mode 1 - read from port
 */

    unsigned char   buffer[INBUFFSIZE];
    int             n;

    if ((n = read(in, buffer, INBUFFSIZE)) < 0) {  /* read nothing */
        /* mode 1 => read from port */
        if (errno == EIO)
            sleep(1);
        else
            perror(RPORTFAIL);
    }
    
    else {  /* successfully read something from somewhere */
        if (n >=8) {  /* need at least 8 bytes for a meter packet */
            /* read from port */
            write(out, buffer, n);
        } /* end if n >= 8 */
    } /* end else there were characters to read */
} /* end writeData() */

void pipeReader() {
/* get data from pipes */

    int             maxfd;
    fd_set          read_set;

    maxfd = tty_data.portpipefd[RPIPE];

    while ( (!tty_data.maxSamples) ||
            (tty_data.numSamples < tty_data.maxSamples) )
    {
        FD_ZERO(&read_set);
        FD_SET(tty_data.portpipefd[RPIPE], &read_set);
        if (select(maxfd + 1, &read_set, NULL, NULL, NULL) < 0) {
            perror(SELFAIL);
            return;
        }
        if (FD_ISSET(tty_data.portpipefd[RPIPE], &read_set))
            writeDataFromPipe(tty_data.portpipefd[RPIPE], tty_data.logfd);
    }
    /* tell parent it's Miller time ;-) */
    write(tty_data.cstatpipefd[WPIPE], "Q", 1);
    sleep(2);  /* give parent a chance to respond */
}

void closeAll() {
    /* clean up meter packet interface */
    rs22812_packet_delete(meterPacket);

    /* clean up serial port interface */
    serport_delete(serialPort);
    
    /* close write pipes */
    if (tty_data.cstatpipefd[WPIPE] >= 0)
        close(tty_data.cstatpipefd[WPIPE]);
    if (tty_data.portpipefd[WPIPE] >= 0)
        close(tty_data.portpipefd[WPIPE]);

    /* kill child */
    if (pid >= 0)
        kill(pid, SIGINT);
}

RETSIGTYPE sigintP(int sig) {
/*parent signal handler for SIGINT */
    closeAll();
    _exit(1);
}

RETSIGTYPE sigintC(int sig) {
/* child signal handler for SIGINT */
    /* close read pipes */
    if (tty_data.cstatpipefd[RPIPE] >= 0)
        close(tty_data.cstatpipefd[RPIPE]);
    if (tty_data.portpipefd[RPIPE] >= 0)
        close(tty_data.portpipefd[RPIPE]);
    _exit(1);    
}

RETSIGTYPE sigchldP(int sig) {
/* signal handler for SIGCHLD */

    int status;
    
    wait(&status);
}

int main(int argc, char *argv[]) {
    
    int             maxfd, optret = 1;
    char            *ptr1, *ptr2;
    fd_set          rset;
    
#ifdef HAVE_GETOPT_LONG
    struct option longopts[] = {
        {"bytes",      0, NULL, 'b'},
        {"help",       0, NULL, 'h'},
        {"nolock",     0, NULL, 'n'},
        {"samples",    1, NULL, 's'},
        {"timestamp",  0, NULL, 't'},
        {"verbose",    0, NULL, 'v'},
        {"version",    0, NULL, 'V'},
        {NULL,         0, NULL,   0}
    };
#endif
    /* don't lose last chunk of data when output is non-interactive */
    setvbuf(stdout,NULL,_IONBF,0);
    setvbuf(stderr,NULL,_IONBF,0);

    /* initialize variables */
    tty_data.logfd = STDOUT_FILENO;
    tty_data.portraw = tty_data.nolock = FALSE;
    tty_data.dspbytes = tty_data.tstamp = tty_data.verbose = FALSE;
    tty_data.cstatpipefd[RPIPE] = tty_data.cstatpipefd[WPIPE] = -1;
    tty_data.portpipefd[RPIPE] = tty_data.portpipefd[WPIPE] = -1;
    tty_data.maxSamples = tty_data.numSamples = 0;
    timeStamp   = timestamp_new();
    meterPacket = rs22812_packet_new();
    serialPort  = serport_new();
    serport_set_format(serialPort, F8N1);
    serport_set_speed(serialPort, DEFBAUDRATE);
    /* parse rc-file */
    readRC(&tty_data);
    /* activate signal handlers */
    signal(SIGINT, sigintP);
    signal(SIGCHLD, sigchldP);
    /* register closeAll() function to be called on normal termination */
    /* atexit(closeAll); */
    /* process command line arguments */
#ifdef HAVE_GETOPT_LONG
    while ((optret = getopt_long(argc, argv, OPTSTR, longopts, NULL)) != -1) {
#else
    while ((optret = getopt(argc, argv, OPTSTR)) != -1) {
#endif
        switch (optret) {
            case 'b':
                tty_data.dspbytes = TRUE;
                break;
            case 'n':
                tty_data.nolock = TRUE;
                break;
            case 's':
                tty_data.maxSamples = atoi(optarg);
                break;
            case 't':
                tty_data.tstamp = TRUE;
                break;
            case 'v':
                tty_data.verbose = TRUE;
                break;
            case 'V':
		copyright();
		return 0;
            case 'h':
            case '?':
            default :
                usage();
                return -1;
        }
    }
    if (optind < argc) {
	   if (!(serialPort->device_name = malloc(PORTNAMELEN))) fatalError(MEMFAIL);
	   ptr1 = argv[optind];
	   ptr2 = serialPort->device_name;
	   while((*ptr1 == '/' || isalnum(*ptr1))
                && ptr2 - serialPort->device_name < PORTNAMELEN - 1) *ptr2++ = *ptr1++;
	   *ptr2 = 0;
    }
    if (!serialPort->device_name || !serialPort->device_name[0]) {
        usage();
        return -1;
    }
    /* create pipes */
    if ( (pipe(tty_data.cstatpipefd) < 0) ||
         (pipe(tty_data.portpipefd)  < 0) )
    {
        perror(PIPEFAIL);
        return -1;
    }
    /* fork child process */
    switch (pid = fork()) {
    case 0:
        /* child process */
        /* close child status read pipe and port write pipe */
        close(tty_data.cstatpipefd[RPIPE]);
        close(tty_data.portpipefd[WPIPE]);
        signal(SIGINT, sigintC);
        pipeReader(&tty_data);  /*child proc lives entirely within pipeReader*/
        closeAll();
        break;
    case -1:
        /* fork() failed */
        perror(FORKFAIL);
        return -1;
    default:
        /* parent process */
        /* close child status write pipe and port read pipe */
        close(tty_data.cstatpipefd[WPIPE]);
        close(tty_data.portpipefd[RPIPE]);
        break;
    }
    /* lock port */
    if (!tty_data.nolock && dev_setlock(serialPort->device_name) == -1) {
        /* couldn't lock the device */
        return -1;
    }

    /* set raw mode on port */
    serport_set_raw_mode( serialPort );
    
    /* try to open port */
    serport_open( serialPort );
    if (tty_data.verbose) {
        printf("Opened port: %s\n", serialPort->device_name);
    }
    tty_data.portraw = TRUE;

    /* start listening to the child and port */
    maxfd  = max(tty_data.cstatpipefd[RPIPE], serialPort->file_des);
    while (TRUE) {
        FD_ZERO(&rset);
        FD_SET(serialPort->file_des, &rset);
        FD_SET(tty_data.cstatpipefd[RPIPE], &rset);
        if (select(maxfd + 1, &rset, NULL, NULL, NULL) < 0) {
            perror(SELFAIL);
            return -1;
        }
        if (FD_ISSET(serialPort->file_des, &rset)) {
            /* data coming from device */
            writeDataFromPort(serialPort->file_des, tty_data.portpipefd[WPIPE]);
        }
        if (FD_ISSET(tty_data.cstatpipefd[RPIPE], &rset)) {
            /* data coming from child - means time to quit */
            if (tty_data.verbose)
                printf("* DONE *\n");
            break;
        }
    }
    /* closeAll() */
    return 1;
}
