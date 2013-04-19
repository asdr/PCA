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
#include <semaphore.h>
#include <signal.h>
#include "consumer.h"
#include "logger.h"
#include "random.h"

extern sem_t* __pca_global_empty_sem;
extern sem_t* __pca_global_full_sem;
extern sem_t* __pca_global_mutex;

int __pca_global_got_signal = 0;
int __pca_global_incs = 0;

void signal_handler( int signo ) {
  if ( signo == SIGUSR1 )
    {
      log_event( "consumer got signal." );
      // we got the signal
      __pca_global_got_signal = 1;
    }
  /*else if ( signo == SIGTERM )
    {
      log_event( "SIGTERM" );
      random_close();
      log_close_file();
      kill(getpid(), SIGKILL);
      }*/
}

void consume_transaction( SHAREDBUFFER* shared_buffer ) {
  TRANSACTION* transaction = NULL;
  int delay = 0; //in seconds
  char message[100];
  long random_value;
  int tc;
  int key_found = 0;
  int value;
  char key[(KEY_SIZE/8) + 1];

  if ( !shared_buffer )
    {
      log_event( "Unable to produce transaction; shared buffer doesn't exist." );
      return;
    }

  sem_getvalue(&(shared_buffer->full_sem), &value);
  sprintf(message, "0-->%d", value);
  log_event(message);

      // wait if shared buffer is empty
      sem_wait( __pca_global_full_sem );
                log_event("0.0");
      sem_wait( __pca_global_mutex );

      ++__pca_global_incs;

      // find the transaction to process
      // choose the last transaction for now
      //tc = shared_buffer->transaction_count - 1;
      tc = MAX_TRANSACTION_COUNT;
      do
        {
          transaction = &(shared_buffer->transactions[ tc-- ]);
        }
      while( (transaction->ready != 9 || transaction->decrypted == 1) && tc >=0 );

      log_event("1");

      if ( tc >= 0 )
        {
          log_event("2");
          sprintf(message, "attack [%d:%d]", tc+1, transaction->key_partition_count);
          log_event( message );

          //get just one partition at a time
          transaction->key_partition_count -= 1;
        }

      --__pca_global_incs;

      sem_post( __pca_global_mutex );
      sem_post( __pca_global_full_sem );
      //sem_post( __pca_global_empty_sem );

          log_event("3");

      //now attack on the transaction
      //outside the critical section
      //because, while a consumer process attacks on one partition
      //another will attack on another partititon 'till the key
      //is found
      attack_on_transaction(transaction, key);

      // wait if shared buffer is empty
      //sem_wait( __pca_global_full_sem );
      sem_wait( __pca_global_mutex );

      ++__pca_global_incs;


          log_event("4");

      // got result
      if ( key[0] != '\0' )
        {

          log_event("5");
          transaction->decrypted = 1;
          shared_buffer->transaction_count -= 1;

          sprintf(message, "Key found: %s", key);
          log_event(message);

        }
      else if (key[1] == '\0')
        {

          log_event("6");
          transaction->decrypted = 1;
          shared_buffer->transaction_count -= 1;
          log_event("key not found");

        }

          log_event("7");
      sem_post( __pca_global_mutex );
      //sem_post( __pca_global_empty_sem );

      --__pca_global_incs;

          log_event("8");
}

int main ( int argc, char** argv ) {
  int consumer_lifetime = 60;
  SHAREDBUFFER* shared_buffer;
  int logfd;

  random_open();

  logfd = log_open_file( NULL );

  if ( logfd < 1 )
    {
      printf("Unable to open log file.");
      return EXIT_FAILURE;
    }

  if ( argc > 1 )
    {
      consumer_lifetime = atoi( argv[1] );
    }

  shared_buffer = get_shared_buffer( );

  if ( !shared_buffer )
    {
      log_event( "Unable to produce transaction; shared buffer doesn't exist." );
      return;
    }

  log_event( "Consumer process started." );
  keep_track_of_child_process( shared_buffer );

  signal( SIGUSR1, signal_handler );

  while( __pca_global_got_signal == 0 || shared_buffer->transaction_count > 0)
    {
      consume_transaction( shared_buffer );
    }

 SAFE_EXIT:
  random_close();
  log_close_file();

  return EXIT_SUCCESS;
}
