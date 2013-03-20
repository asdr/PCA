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

#ifndef __PCA_SHARED_BUFFER_H_
#define __PCA_SHARED_BUFFER_H_

#include <semaphore.h>

#define TRANSACTION_SIZE 250
#define MAX_TRANSACTION_COUNT 2000
#define SHM_NAME "/tmp/pca_shm"
#define MAX_PROCESS_COUNT 100

typedef struct _shared_buffer SHAREDBUFFER;
typedef union _transaction TRANSACTION;

union _transaction {
  char type;
  char data[ TRANSACTION_SIZE ];
};

struct _shared_buffer {
  int smfd;
  sem_t empty_sem;
  sem_t full_sem;
  sem_t mutex;

  int transaction_count;
  TRANSACTION transactions[ MAX_TRANSACTION_COUNT ];

  int child_processes[ MAX_PROCESS_COUNT ];
  int child_process_count;
};


/****
 * create_shared_buffer
 *
 * Aloocates memory from shared memory area for processes.
 * Initializes SHAREDBUFFER data structure, which contains
 * semaphores for concurrency, array for transaction and
 * array for child processes which is going to access
 * this area.
 *
 * @param no parameter
 *
 * @return address of the starting point of created shared
 *   memory area.
 *
*****/
SHAREDBUFFER* create_shared_buffer ( void );


/** 0  **/
SHAREDBUFFER* get_shared_buffer ( void );


/** 1  **/
void close_shared_buffer ( SHAREDBUFFER* shared_buffer );


/****
 * create_transaction
 *
 * Allocates a momory for TRANSACTION data structure and
 *   initializes it with given data. The creation time is
 *   simulated as random delay.
 *
 * @param data - the data of the transaction; the first byte of
 *   data is the type of it.
 *
 * @return address of newly created TRANSACTION data structure.
*****/
TRANSACTION* create_transaction ( char* data );

/****
 * destroy_shared_buffer
 *
 * Releases the given SHAREDBUFFER data structure witl all of inner
 *   data structures, semaphores and files.
 *
 * @param shared_buffer - address of SHAREDBUFFER data structure.
 *
*****/
void destroy_shared_buffer ( SHAREDBUFFER* shared_buffer );

/****
 * keep_track_of_child_process
 *
 * Records child itself with its pid into shared area for
 *  further use.
 *
 * @param shared_buffer - address of SHAREDBUFFER data structure.
 *
*****/
void keep_track_of_child_process ( SHAREDBUFFER* shared_buffer );


/****
 * kill_all_child_processes
 *
 * Sends SIGKILL signal to all known producer
 *   and consumer processes and force them to exit.
 *
 * @param shared_buffer - address of SHAREDBUFFER data structure.
 *
*****/
void kill_all_child_processes( SHAREDBUFFER* shared_buffer );


#endif
