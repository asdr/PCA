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
#include <time.h>
#include <sys/time.h>
#include "shared_buffer.h"
#include "logger.h"

int main ( int argc, char** argv ) {
  int logfd = log_open_file( NULL );
  int main_process_lifetime = 30; //in seconds
  SHAREDBUFFER* shared_buffer;
  struct timeval start_time;
  struct timeval current_time;

  if ( logfd < 1 )
    {
      printf("Unable to open log file.");
      return EXIT_FAILURE;
    }

  shared_buffer = get_shared_buffer();

  if ( !shared_buffer )
    {
      log_event( "Unable to get shared buffer." );
      log_close_file();
      return EXIT_FAILURE;
    }

  if ( argc > 1 )
    {
      main_process_lifetime = atoi(argv[1]);
    }

  log_event( "Controller process started." );

  gettimeofday( &start_time, NULL );
  do
    {
      sleep( 5 ); // sleep 5 seconds
      gettimeofday( &current_time, NULL );
    }
  while ( (current_time.tv_sec - start_time.tv_sec) <= main_process_lifetime  );

  kill_all_child_processes( shared_buffer );

  close_shared_buffer( shared_buffer );
  log_close_file( );

  return EXIT_SUCCESS;
}
