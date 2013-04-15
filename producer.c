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
#include <signal.h>
#include "logger.h"
#include "producer.h"
#include "random.h"

extern sem_t* __pca_global_empty_sem;
extern sem_t* __pca_global_full_sem;
extern sem_t* __pca_global_mutex;

int __pca_global_got_signal = 0;
int __pca_global_incs = 0;

void signal_handler( int signo ) {
  if ( signo == SIGUSR1 )
    {
      log_event( "producer got signal." );
      // we got the signal
      __pca_global_got_signal = 1;
      kill ( getpid(), SIGTERM );
    }
}

void produce_transaction ( SHAREDBUFFER* shared_buffer ) {
  TRANSACTION* transaction = NULL;
  char message[20000];
  char type=0;
  long random_value;
  int tc;

  if ( !shared_buffer )
    {
      //log_event( "Unable to produce transaction; shared buffer doesn't exist." );
      return;
    }

  // create a transaction
  transaction = create_transaction( );

  if ( !transaction )
    {
      log_event( "Unable to produce transaction; transaction doesn't exist." );
      return;
    }

  if ( __pca_global_got_signal == 0 )
    {

      // wait if shared buffer is full
      sem_wait( __pca_global_empty_sem );
      sem_wait( __pca_global_mutex );

      ++__pca_global_incs;

      //strcpy( shared_buffer->transactions[shared_buffer->transaction_count].data,
      //          transaction->data );

      tc = shared_buffer->transaction_count;

      shared_buffer->transactions[tc].type = transaction->type;
      shared_buffer->transactions[tc].length = transaction->length;

      shared_buffer->transactions[tc].plain_text =
        (char*) malloc( sizeof(char)*(transaction->length)/2 );

      strcpy( shared_buffer->transactions[tc].plain_text,
              transaction->plain_text );

      shared_buffer->transactions[tc].cipher_text =
        (char*) malloc( sizeof(char)*(transaction->length)/2 );

      strcpy( shared_buffer->transactions[tc].cipher_text,
              transaction->cipher_text );

      shared_buffer->transactions[tc].key_partition_count = transaction->key_partition_count;
      shared_buffer->transaction_count = tc + 1;

      sprintf(message, "tC: %d", shared_buffer->transaction_count);
      log_event( message );

      sem_post( __pca_global_mutex );
      sem_post( __pca_global_full_sem );

      --__pca_global_incs;
    }

  destroy_transaction( transaction );
}

int main ( int argc, char** argv ) {
  int producer_lifetime = 20; // 20 seconds default producer lifetime value
  struct timeval start_time;
  struct timeval current_time;
  int logfd;
  SHAREDBUFFER* shared_buffer;

  random_open();

  logfd = log_open_file( NULL );

  if ( logfd < 1 )
    {
      return EXIT_FAILURE;
    }

  if ( argc > 1 )
    {
      producer_lifetime = atoi( argv[1] );
    }

  // get shared memory area
  shared_buffer = get_shared_buffer();

  if ( shared_buffer == NULL )
    {
      log_event( "Unable to acquire shared buffer." );
      log_close_file();
      return EXIT_FAILURE;
    }

  log_event( "Producer process started." );
  keep_track_of_child_process( shared_buffer );

  signal( SIGUSR1, signal_handler );

  gettimeofday( &start_time, NULL );
  do
    {
      produce_transaction( shared_buffer );
      if ( __pca_global_got_signal == 1 )
        break;
      gettimeofday( &current_time, NULL );
    }
  while ( (current_time.tv_sec - start_time.tv_sec) <= producer_lifetime  );

 SAFE_EXIT:
  random_close();
  close_shared_buffer( shared_buffer );
  log_close_file( );

  return EXIT_SUCCESS;
}
