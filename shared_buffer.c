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
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include "shared_buffer.h"
#include "logger.h"

SHAREDBUFFER* create_shared_buffer( void ) {
  int smfd;
  SHAREDBUFFER* shared_buffer;

  smfd = shm_open( SHM_NAME, O_RDWR | O_CREAT, 0600 );
  ftruncate( smfd, sizeof(SHAREDBUFFER) );
  shared_buffer = (SHAREDBUFFER*) mmap( NULL,
                                        sizeof(SHAREDBUFFER),
                                        PROT_READ | PROT_WRITE,
                                        MAP_SHARED,
                                        smfd,
                                        0 );

  if ( !shared_buffer )
    {
      // cannot malloc shared buffer
      log_event( "Unable to create shared buffer." );
      return NULL;
    }

  shared_buffer->smfd = smfd;
  shared_buffer->transaction_count = 0;

  sem_init( &(shared_buffer->empty_sem), 1, MAX_TRANSACTION_COUNT );
  sem_init( &(shared_buffer->full_sem), 1, 0 );
  sem_init( &(shared_buffer->mutex), 1, 1 );

  shared_buffer->child_process_count = 0;

  log_event( "Shared buffer is created and initialized." );

  return shared_buffer;
}

void destroy_shared_buffer( SHAREDBUFFER* shared_buffer ) {
  int smfd;
  if ( !shared_buffer )
    {
      log_event( "Unable to destroy shared_buffer, which doesn't exist." );
      return;
    }

  smfd = shared_buffer->smfd;

  sem_destroy( &(shared_buffer->empty_sem) );
  sem_destroy( &(shared_buffer->full_sem) );
  sem_destroy( &(shared_buffer->mutex) );

  // Unmap the object
  munmap(shared_buffer, sizeof(SHAREDBUFFER));
  // Close the shared memory object handle
  close(smfd);
  // Remove the shared memory object
  shm_unlink(SHM_NAME);

  log_event( "Shared buffer is destroyed." );
}

TRANSACTION* create_transaction( char* data ) {
  TRANSACTION* transaction = (TRANSACTION*) malloc( sizeof(TRANSACTION) );
  int delay = 0; // in seconds
  char message[150];

  if ( !transaction )
    {
      log_event( "Unable to create transaction." );
      return NULL;
    }

  // set data of transaction
  strcpy(transaction->data, data);

  // simulate random delay for creation time
  delay = rand() % 4; // at most delay for 3 seconds
  sleep( delay );

  sprintf( message, "Transaction created in %d seconds.", delay );
  log_event( message );
  return transaction;
}

void consume_transaction( SHAREDBUFFER* shared_buffer ) {
  TRANSACTION* transaction = NULL;
  int delay = 0; //in seconds
  char message[100];

  if ( !shared_buffer )
    {
      log_event( "Unable to produce transaction; shared buffer doesn't exist." );
      return;
    }

  // wait if shared buffer is empty
  sem_wait( &(shared_buffer->full_sem) );
  sem_wait( &(shared_buffer->mutex) );

  log_event( "Inside mutex." );
  // find the transaction to process
  // choose the last transaction for now
  transaction = &(shared_buffer->transactions[shared_buffer->transaction_count - 1]);
  shared_buffer->transaction_count -= 1;

  sem_post( &(shared_buffer->mutex) );
  sem_post( &(shared_buffer->empty_sem) );
  sprintf( message,
           "Outside mutex with transaction(type[%c]) count: %d.",
           transaction->type,
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

void keep_track_of_child_process( SHAREDBUFFER* shared_buffer ) {
  if ( !shared_buffer )
    {
      log_event( "Unable to record child process; there is no shared buffer." );
      return;
    }

  sem_wait( &(shared_buffer->mutex) );
  shared_buffer->child_processes[shared_buffer->child_process_count++] = getpid();
  sem_post( &(shared_buffer->mutex) );
}

void kill_all_child_processes( SHAREDBUFFER* shared_buffer ) {
  int k;

  if ( !shared_buffer )
    {
      log_event( "Unable to record child process; there is no shared buffer." );
      return;
    }

  sem_wait( &(shared_buffer->mutex) );

  for ( k=0; k<shared_buffer->child_process_count; ++k)
    {
      kill(shared_buffer->child_processes[k], 9); // send signal 9 SIGKILL
    }

  sem_post( &(shared_buffer->mutex) );

}
