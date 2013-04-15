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
#include "transaction.h"
#include "logger.h"
#include "random.h"
#include "cipher.h"

TRANSACTION* create_transaction( ) {
  TRANSACTION* transaction = (TRANSACTION*) malloc( sizeof(TRANSACTION) );
  int type = 0; // 0 Alg1,
                // 1 Alg2,
                // 2 Alg3
  int length = 0; // in seconds
  char* plain_text;
  char* cipher_text;
  char* key;
  char message[20000];
  long random_value;

  if ( !transaction )
    {
      log_event( "Unable to create transaction." );
      return NULL;
    }

  // random cipher algorithm
  random_get_value( &random_value );

  type = random_value % 3;

  // length of random data at least 64K
  // the length has to be an even number in order to divide it by 2
  // first part of data is plain text
  // second part of data is cipher text
  random_get_value( &random_value );

  length = MIN_TRANSACTION_LENGTH +
    ( random_value % (MAX_TRANSACTION_LENGTH - MIN_TRANSACTION_LENGTH) );
  if ( length % 2 > 0 )
    {
      if ( length < MAX_TRANSACTION_LENGTH )
        ++length;
      else
        --length;
    }

  // generate a random key of KEY_SIZE bits long
  //key = random_generate_key( KEY_SIZE );
  key = random_generate_text( KEY_SIZE/8 );

  // half of the transaction data is plain_text
  plain_text = random_generate_text( length/2 );

  // half of the transaction data is cipher_text
  switch ( type )
    {
    case 0:
      cipher_text = encrypt_alg1( plain_text, key );
      break;
    case 1:
      cipher_text = encrypt_alg2( plain_text, key );
      break;
    case 2:
      cipher_text = encrypt_alg3( plain_text, key );
      break;
    }


  //  sprintf(message, "pt: %s", plain_text);
  //  log_event(message);

  //  sprintf(message, "ct: %s", cipher_text);
  //  log_event(message);

  transaction->type = type;
  transaction->length = length;
  transaction->plain_text = plain_text;
  transaction->cipher_text = cipher_text;
  transaction->decrypted = 0;
  transaction->key_partition_count = PARTITION_COUNT;

  return transaction;
}

void destroy_transaction( TRANSACTION* transaction ) {

  if ( transaction )
    {
      free( transaction->plain_text );
      free( transaction->cipher_text );
      free( transaction );
    }
}
