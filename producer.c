#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include "logger.h"
#include "producer.h"

void produce_transaction ( SHAREDBUFFER* shared_buffer ) {
  TRANSACTION* transaction = NULL;
  char message[100];
  if ( !shared_buffer )
    {
      log_event( "Unable to produce transaction; shared buffer doesn't exist." );
      return;
    }

  // create a transaction of type "A"
  transaction = create_transaction( "A" );

  if ( !transaction )
    {
      log_event( "Unable to produce transaction; transaction doesn't exist." );
      return;
    }

  // wait if shared buffer is full
  sem_wait( &(shared_buffer->empty_sem) );
  sem_wait( &(shared_buffer->mutex) );

  log_event( "Inside mutex." );
  strcpy(shared_buffer->transactions[shared_buffer->transaction_count].data, transaction->data);
  shared_buffer->transaction_count = shared_buffer->transaction_count + 1;

  sem_post( &(shared_buffer->mutex) );
  sem_post( &(shared_buffer->full_sem) );

  sprintf( message,
           "Outside mutex with transaction(type[%c]) count: %d.",
           transaction->type,
           shared_buffer->transaction_count );
  log_event( message );

  free(transaction);
}

int main ( int argc, char** argv ) {
  int producer_lifetime = 20; // 20 seconds default producer lifetime value
  struct timeval start_time;
  struct timeval current_time;
  int logfd;

  if ( logfd < 1 )
    {
      return EXIT_FAILURE;
    }

  if ( argc > 1 )
    {
      producer_lifetime = atoi( argv[1] );
    }

  log_event( "Producer process started." );
  keep_track_of_child_process( shared_buffer );

  gettimeofday( &start_time, NULL );
  do
    {
      produce_transaction( shared_buffer );
      gettimeofday( &current_time, NULL );
    }
  while ( (current_time.tv_sec - start_time.tv_sec) <= producer_lifetime  );

  return EXIT_SUCCESS;
}
