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
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "config.h"
#include "logger.h"
#include "shared_buffer.h"

int main ( int argc, char** argv ) {
  SHAREDBUFFER* shared_buffer;
  CONFIG* config;
  char main_process_lifetime[15];
  int producer_count = 0;
  char producer_lifetime[15];
  int consumer_count = 0;
  int child_exit_status = 0;
  int child_exit_pid;
  int child_pid;
  int i = 0;
  int logfd;
  char message[150];

  logfd = log_open_file( NULL );
  if ( !logfd )
    {
      printf("Unable to open log file.\n");
      return EXIT_FAILURE;
    }

  srand( (unsigned) time( NULL ) );

  log_event( "--------------------------------" );
  log_event( "Main process started." );

  // first of all load configuration file
  config = load_config_file();

  if ( !config )
    {
      log_event( "Unable to access configuration." );
      return EXIT_FAILURE;
    }

  // read values from configuration
  producer_count = atoi(read_configuration(config, "producer_count", "5"));
  consumer_count = atoi(read_configuration(config, "consumer_count", "2"));
  strcpy(producer_lifetime, read_configuration(config, "producer_lifetime", "10"));
  strcpy(main_process_lifetime, read_configuration(config, "main_process_lifetime", "40"));

  destroy_config( config );

  // initialize shared buffer
  shared_buffer = create_shared_buffer();

  if ( !shared_buffer )
    {
      log_event( "Unable to initialize shared buffer." );
      destroy_config( config );
      return EXIT_FAILURE;
    }

  // create and start producer processes
  // producer processes run for a specified lifetime ( in seconds )
  // which is read from configuration
  for ( i=0; i<producer_count; ++i)
    {
      child_pid = vfork();
      if ( child_pid == 0 ) // producer process
        {
          execl("producer", "producer", producer_lifetime, NULL);
        }
    }

  // start consumer processes
  // consumer processes run until a SIGKILL signal
  // there is no specific entry in the requirements that
  // when a consumer process should end.
  for ( i=0; i<consumer_count; ++i)
    {
      child_pid = vfork();
      if ( child_pid == 0 ) // consumer process
        {
          execl("consumer", "consumer", NULL);
        }
    }

  // start controller process
  // controller process checks elapsed time
  // once in every 5 seconds
  // if total execution time is above a specified time (in seconds)
  // signals all child processes except itself, to force exit
  child_pid = vfork();
  if ( child_pid == 0 ) //controller process
    {
      execl("controller", "controller", main_process_lifetime, NULL);
    }

  // in order to catch all child processes exits
  // we need a common wait for all them
  // here exitting processes is cought with pid and exit status
  // wait childs to exit
  while( (child_exit_pid = wait(&child_exit_status)) > 0 )
    {
      sprintf( message, "Process [PID:%d] exitid with status: %d", child_exit_pid, child_exit_status );
      log_event( message );
    }

  // relase allocated data structures to OS
  destroy_shared_buffer( shared_buffer );

  log_event( "Main process is being closed." );

  log_close_file(  );

  return EXIT_SUCCESS;
}
