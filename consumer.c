#include "consumer.h"

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

int main ( int argc, char** argv ) {



}
