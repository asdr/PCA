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
#include <fcntl.h>
#include <string.h>
#include "random.h"

int __pca_global_random_file = -1;

void random_open( void ) {
  if ( __pca_global_random_file == -1 )
    {
      __pca_global_random_file = open( PSEUDORANDOM_KEY_SOURCE, O_RDONLY );
    }
}

void random_get_value( long* random_value ) {
  if ( __pca_global_random_file == -1)
    {

      *random_value = rand();
    }
  else
    {
      read( __pca_global_random_file, random_value, sizeof(long) );
      if ( *random_value < 0 )
        *random_value *= -1;
      //additional ( not much readable ) math.abs value
      //*random_value = ((*random_value>>(((sizeof(long))<<3)-1)) ^ *random_value);
      //*random_value -= (*random_value>>(((sizeof(long))<<3)-1));
    }
}

char* random_generate_text( int length ) {
  int i=0, ch;
  long int random_value;
  char* buffer = (char*) malloc(sizeof(char) * (length+1));
  if ( buffer == NULL )
    return NULL;

  for ( ; i<length; ++i )
    {
      random_get_value( &random_value );

      // char or numeric
      if ( random_value % 2 == 0 )
        {
          random_get_value( &random_value );

          if ( random_value % 2 == 0 )
            {
              random_get_value( &random_value );

              ch = 'A' + random_value%26;
            }
          else
            {
              random_get_value( &random_value );

              ch = 'a' + random_value%26;
            }
        }
      else
        {
          random_get_value( &random_value );

          ch = '0' + random_value%10;
        }

      buffer[i] = ch;
    }

  buffer[length] = '\0';
  return buffer;
}

char* random_generate_key( int bit_length ) {
  int i=0, k=0, ch;
  long int random_value;
  char* buffer = (char*) malloc( sizeof(char) * (bit_length / 8) );
  if ( buffer == NULL )
    return NULL;

  for ( ; i<bit_length; ++i, k=i/8 )
    {
      random_get_value( &random_value );
      ch = random_value%2;
      buffer[k] |= (ch << i%8);
    }

  buffer[k+1] = '\0';
  return buffer;
}

void random_close() {
  close( __pca_global_random_file );
  __pca_global_random_file = -1;
}
