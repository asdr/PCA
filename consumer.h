#ifndef __PCA_CONSUMER_H_
#define __PCA_CONSUMER_H_

/****
 * consume_transaction
 *
 * Gets a transaction from shared area, Waits if the shared area
 *   is empty.
 *
 * @param shared_buffer - address of SHAREDBUFFER data structure.
 *
*****/
void consume_transaction ( SHAREDBUFFER* shared_buffer );

/* main program */
int main ( int argc, char** argv );

#endif
