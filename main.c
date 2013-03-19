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
  int main_process_lifetime = 0;
  int producer_count = 0;
  int producer_lifetime = 0;
  int consumer_count = 0;
  int child_exit_status = 0;
  int child_exit_pid;
  int child_pid;
  int i = 0;
  char message[150];

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
  producer_lifetime = atoi(read_configuration(config, "producer_lifetime", "10"));
  main_process_lifetime = atoi(read_configuration(config, "main_process_lifetime", "40"));

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
      child_pid = fork();
      if ( child_pid == 0 ) // producer process
        {
          struct timeval start_time;
          struct timeval current_time;

          log_event( "Producer process started." );
          keep_track_of_child_process( shared_buffer );

          gettimeofday( &start_time, NULL );
          do
            {
              produce_transaction( shared_buffer );
              gettimeofday( &current_time, NULL );
            }
          while ( (current_time.tv_sec - start_time.tv_sec) <= producer_lifetime  );

          exit(0);
        }
    }

  // start consumer processes
  // consumer processes run until a SIGKILL signal
  // there is no specific entry in the requirements that
  // when a consumer process should end.
  for ( i=0; i<consumer_count; ++i)
    {
      child_pid = fork();
      if ( child_pid == 0 ) // consumer process
        {
          log_event( "Consumer process started." );
          keep_track_of_child_process( shared_buffer );

          while(1)
            {
              consume_transaction( shared_buffer );
            }

          // there is no coming here
          //exit(0);
        }
    }

  // start controller process
  // controller process checks elapsed time
  // once in every 5 seconds
  // if total execution time is above a specified time (in seconds)
  // signals all child processes except itself, to force exit
  child_pid = fork();
  if ( child_pid == 0 ) //controller process
    {
      struct timeval start_time;
      struct timeval current_time;

      log_event( "Controller process started." );

      gettimeofday( &start_time, NULL );
      do
        {
          sleep( 5 ); // sleep 5 seconds
          gettimeofday( &current_time, NULL );
        }
      while ( (current_time.tv_sec - start_time.tv_sec) <= main_process_lifetime  );

      kill_all_child_processes( shared_buffer );

      exit(0);
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
  destroy_config( config );

  log_event( "Main process is being closed." );

  return EXIT_SUCCESS;
}
