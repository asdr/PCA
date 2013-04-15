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

#ifndef __PCA_TRANSACTION_H_
#define __PCA_TRANSACTION_H_

//64K
#define MIN_TRANSACTION_LENGTH 8192 //(1<<16)/2^3
//128K
#define MAX_TRANSACTION_LENGTH 16384 //(1<<17)/2^3
//32BITS
#define KEY_SIZE (32)
//4BITS
#define PARTITION_COUNT (4)

typedef struct _transaction TRANSACTION;

struct _transaction {
  char type;
  int length;

  char* plain_text;
  char* cipher_text;

  int decrypted;

  int key_partition_count;
};


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
TRANSACTION* create_transaction ( );

/****
 * create_transaction
 *
 * Allocated memory of given transaction is released.
 *
 * @param transaction - the transaction itself
 *
*****/
void destroy_transaction( TRANSACTION* transaction );

#endif
