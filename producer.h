#ifndef __PCA_PRODUCER_H_
#define __PCA_PRODUCER_H_

#include "shared_buffer.h"

/****
 * produce_transaction
 *
 * Creates a transaction and insert it into shared area while
 *   using semaphores for race conditions. Waits if the shared
 *   area is full.
 *
 * @param shared_buffer - address of SHAREDBUFFER data structure.
 *
*****/
void produce_transaction ( SHAREDBUFFER* shared_buffer );

/* main program */
int main( ing argc, char** argv );


#endif
