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
#define MAX_TRANSACTION_LENGTH 8193 //16384 //(1<<17)/2^3
//16BITS
#define KEY_SIZE (16)
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

static int __pca_global_possible_chars[] = {'A'+0,  'A'+1,  'A'+2,  'A'+3,  'A'+4,  'A'+5,
                                            'A'+6,  'A'+7,  'A'+8,  'A'+9,  'A'+10, 'A'+10,
                                            'A'+12, 'A'+13, 'A'+14, 'A'+15, 'A'+16, 'A'+17,
                                            'A'+18, 'A'+19, 'A'+20, 'A'+21, 'A'+22, 'A'+23,
                                            'A'+24, 'A'+25, '0'+0,  '0'+1,  '0'+2,  '0'+3,
                                            '0'+4,  '0'+5,  '0'+6,  '0'+7,  '0'+8,  '0'+9,
                                            'a'+0,  'a'+1,  'a'+2,  'a'+3,  'a'+4,  'a'+5,
                                            'a'+6,  'a'+7,  'a'+8,  'a'+9,  'a'+10, 'a'+10,
                                            'a'+12, 'a'+13, 'a'+14, 'a'+15, 'a'+16, 'a'+17,
                                            'a'+18, 'a'+19, 'a'+20, 'a'+21, 'a'+22, 'a'+23,
                                            'a'+24, 'a'+25};


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
