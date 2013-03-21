/*
 * Copyright (c) 2013, SERDAR GOKCEN <gokcen.serdar@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by "asdr".
 * 4. Neither the name of "asdr" nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SERDAR GOKCEN ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL SERDAR GOKCEN BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h> //getpid
#include "logger.h"

/* file descripter of log file */
int __pca_global_log_fd = -1;

int log_open_file ( char* file_path ) {
  int fd;

  if ( file_path != NULL )
    {
      fd = open( file_path, O_CREAT | O_WRONLY | O_APPEND );
    }
  else
    {
      fd = open( DEFAULT_LOG_FILE_PATH, O_CREAT | O_WRONLY | O_APPEND );
    }

  if (!fd)
    {
      // log file cannot be opened
      printf("log file cannot be opened.\n");
      return;
    }

  __pca_global_log_fd = fd;
  return fd;
}

void log_close_file ( ) {

  if ( __pca_global_log_fd > 0 )
    {
      close( __pca_global_log_fd );
    }
}

void log_event ( char* message ) {

  char log_line[ MAX_LOG_LINE_LENGTH ];
  struct tm * tm;
  struct timeval tmv;
  int bytes_written;

  // get current time in nanoseconds
  gettimeofday( &tmv, NULL );
  tm = localtime( &(tmv.tv_sec) );

  sprintf(log_line, "%d/%d/%d %d:%d:%d.%d [PID:%d] - %s \n",
          (tm->tm_year+1900), (tm->tm_mon+1), tm->tm_mday,
          tm->tm_hour, tm->tm_min, tm->tm_sec, (int)(tmv.tv_usec),
          getpid(), message);

  bytes_written = write( __pca_global_log_fd, log_line, strlen(log_line) );

  if ( bytes_written > 0 )
    {
      printf("%s", log_line);
    }
}
