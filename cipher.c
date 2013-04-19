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

#include <stdlib.h>
#include <string.h>
#include "cipher.h"

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

int bitXor(int x, int y)
{
  int a = x & y;
  int b = ~x & ~y;
  int z = ~a & ~b;

  return z;
}

char* encrypt_alg1 ( char* plain_text, char* key ) {
  int i=0, k=0, len1 = strlen(plain_text), len2 = strlen(key);
  char * ct = (char*) malloc(sizeof(char) * (len1 + 1));

  for ( i=0; i<len1; ++i)
    {
      for ( k=0; k<len2; ++k) {
        if ( k ==0 )
          ct[i] = __pca_global_possible_chars[(plain_text[i] + (key[k] % 62)) % 62];
        else
          ct[i] = __pca_global_possible_chars[(ct[i] + (plain_text[i] + (key[k] % 62)) % 62) % 62];
      }
    }

  ct[len1] = '\0';
  return ct;
}

char* encrypt_alg2 ( char* plain_text, char* key ) {
  int i=0, k=0, len1 = strlen(plain_text), len2 = strlen(key);
  char * ct = (char*) malloc(sizeof(char) * (len1 + 1));

  for ( i=0; i<len1; ++i)
    {
      for ( k=0; k<len2; ++k) {
        if ( k ==0 )
          ct[i] = __pca_global_possible_chars[(10 + plain_text[i] + (key[k] % 62)) % 62];
        else
          ct[i] = __pca_global_possible_chars[(ct[i] + (10 + plain_text[i] + (key[k] % 62)) % 62) % 62];
      }
    }

  ct[len1] = '\0';
  return ct;
}

char* encrypt_alg3 ( char* plain_text, char* key ) {
  int i=0, k=0, len1 = strlen(plain_text), len2 = strlen(key);
  char * ct = (char*) malloc(sizeof(char) * (len1 + 1));

  for ( i=0; i<len1; ++i)
    {
      for ( k=0; k<len2; ++k) {
        if ( k ==0 )
          ct[i] = __pca_global_possible_chars[(30 + plain_text[i] + (key[k] % 62)) % 62];
        else
          ct[i] = __pca_global_possible_chars[(ct[i] + (30 + plain_text[i] + (key[k] % 62)) % 62) % 62];
      }
    }

  ct[len1] = '\0';
  return ct;
}

void attack_on_transaction( TRANSACTION* transaction, char* output) {
  char *cipher_text;
  int i=0, j=0;
  char key[(KEY_SIZE/8) + 1];

  int l1_start = 0;
  int l1_end = 32;
  int l2_start = 0;
  int l2_end = 32;

  switch ( transaction->key_partition_count )
    {
    case 1:
      l1_start = 0;
      l1_end = 32;
      l2_start = 0;
      l2_end = 32;

      break;
    case 2:
      l1_start = 0;
      l1_end = 32;
      l2_start = 32;
      l2_end = 62;

      break;
    case 3:
      l1_start = 32;
      l1_end = 62;
      l2_start = 0;
      l2_end = 32;

      break;
    case 4:
      l1_start = 32;
      l1_end = 62;
      l2_start = 32;
      l2_end = 62;

      break;

    default:
      output[0] = '\0';
      output[1] = '\0';
      return;
    }


  output[0] = '\0';
  for ( i=l1_start; i<l1_end; ++i)
    {
      key[0] = __pca_global_possible_chars[i];
      for ( j=l2_start; j<l2_end; ++j)
        {

          key[1] = __pca_global_possible_chars[j];
          key[2] = '\0';

          switch ( transaction->type )
            {
            case 0:
              cipher_text = encrypt_alg1(transaction->plain_text, key);
              break;
            case 1:
              cipher_text = encrypt_alg2(transaction->plain_text, key);
              break;
            case 2:
              cipher_text = encrypt_alg3(transaction->plain_text, key);
              break;
            default:
              log_event( "unrecognized transaction type" );
            }

          if ( strcmp(transaction->cipher_text, cipher_text) == 0 )
            {
              strcpy(output, key);
              break;
            }

        }

      if ( output[0] != '\0' )
        break;
    }
}
