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
#include <semaphore.h>
#include "logger.h"
#include "producer.h"
#include "random.h"

extern sem_t* __pca_global_empty_sem;
extern sem_t* __pca_global_full_sem;
extern sem_t* __pca_global_mutex;

void produce_transaction ( SHAREDBUFFER* shared_buffer ) {
  TRANSACTION* transaction = NULL;
  char message[100];
  char type=0;
  long random_value;

  if ( !shared_buffer )
    {
      //log_event( "Unable to produce transaction; shared buffer doesn't exist." );
      return;
    }

  // create a transaction of type "A"
  random_get_value( &random_value );
  type = random_value % 26 + 'A';
  sprintf(message, "%c", type);
  transaction = create_transaction( message );
  transaction->type = type;

  if ( !transaction )
    {
      log_event( "Unable to produce transaction; transaction doesn't exist." );
      return;
    }

  // wait if shared buffer is full
  sem_wait( __pca_global_empty_sem );
  sem_wait( __pca_global_mutex );

  strcpy(shared_buffer->transactions[shared_buffer->transaction_count].data, transaction->data);
  shared_buffer->transaction_count = shared_buffer->transaction_count + 1;

  sprintf( message,
           "Producer: t[%c] %3d->%3d.",
           transaction->type,
           shared_buffer->transaction_count-1,
           shared_buffer->transaction_count );

  log_event( message );

  sem_post( __pca_global_mutex );
  sem_post( __pca_global_full_sem );

  free(transaction);
}

int main ( int argc, char** argv ) {
  int producer_lifetime = 20; // 20 seconds default producer lifetime value
  struct timeval start_time;
  struct timeval current_time;
  int logfd;
  SHAREDBUFFER* shared_buffer;

  // get shared memory area
  shared_buffer = get_shared_buffer();

  if ( shared_buffer == NULL )
    {
      //log_event( "Unable to acquire shared buffer." );
      //log_close_file();
      return EXIT_FAILURE;
    }

  logfd = log_open_file( NULL );

  if ( logfd < 1 )
    {
      return EXIT_FAILURE;
    }

  if ( argc > 1 )
    {
      producer_lifetime = atoi( argv[1] );
    }

  random_open();
  /*
  // get shared memory area
  shared_buffer = get_shared_buffer();

  if ( shared_buffer == NULL )
    {
      log_event( "Unable to acquire shared buffer." );
      log_close_file();
      return EXIT_FAILURE;
    }
  */

  log_event( "Producer process started." );
  keep_track_of_child_process( shared_buffer );

  gettimeofday( &start_time, NULL );
  do
    {
      produce_transaction( shared_buffer );
      gettimeofday( &current_time, NULL );
    }
  while ( (current_time.tv_sec - start_time.tv_sec) <= producer_lifetime  );

  random_close();
  close_shared_buffer( shared_buffer );
  log_close_file( );

  return EXIT_SUCCESS;
}
