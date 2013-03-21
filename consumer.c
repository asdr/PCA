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
#include "consumer.h"
#include "logger.h"

void consume_transaction( SHAREDBUFFER* shared_buffer ) {
  TRANSACTION* transaction = NULL;
  int delay = 0; //in seconds
  char message[100];

  srand( time(NULL) );

  if ( !shared_buffer )
    {
      log_event( "Unable to produce transaction; shared buffer doesn't exist." );
      return;
    }

  // wait if shared buffer is empty
  sem_wait( &(shared_buffer->full_sem) );
  sem_wait( &(shared_buffer->mutex) );

  // find the transaction to process
  // choose the last transaction for now
  transaction = &(shared_buffer->transactions[shared_buffer->transaction_count - 1]);
  shared_buffer->transaction_count -= 1;

  sem_post( &(shared_buffer->mutex) );
  sem_post( &(shared_buffer->empty_sem) );
  sprintf( message,
           "Transaction(type[%c]) count: %d->%d.",
           transaction->type,
           shared_buffer->transaction_count+1,
           shared_buffer->transaction_count );
  log_event( message );

  // process the transaction -- delay
  delay = rand() % 4; // 3 seconds at most
  sprintf( message,
           "Consumer processes transaction(type[%c]) in %d second(s).",
           transaction->type,
           delay );
  log_event( message );

}

int main ( int argc, char** argv ) {
  int consumer_lifetime = 60;
  SHAREDBUFFER* shared_buffer;
  int logfd = log_open_file( NULL );

  if ( logfd < 1 )
    {
      printf("");
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

  while(1)
    {
      consume_transaction( shared_buffer );
    }

  return EXIT_SUCCESS;
}
