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

int bitXor(int x, int y)
{
  int a = x & y;
  int b = ~x & ~y;
  int z = ~a & ~b;
  return z;
}

char* encrypt_alg1 ( char* plain_text, char* key ) {
  int i=0, k=0, len1 = strlen(plain_text), len2 = strlen(key);
  char * ct = (char*) malloc(sizeof(char)*(len1+1));
  for (; i<len1; ++i)
    {
      for (; k<len2; ++k) {
        if ( k ==0 )
          ct[i] = bitXor(plain_text[i], key[k]);
        else
          ct[i] = bitXor(ct[i], bitXor(plain_text[i], key[k]));
      }
    }

  ct[len1] = '\n';
  return ct;
}

char* encrypt_alg2 ( char* plain_text, char* key ) {

  int i=0, k=0, len1 = strlen(plain_text), len2 = strlen(key);
  char * ct = (char*) malloc(sizeof(char)*(len1+1));
  for (; i<len1; ++i)
    {
      for (; k<len2; ++k) {
        if ( k ==0 )
          ct[i] = bitXor(plain_text[i], key[k]);
        else
          ct[i] = bitXor(ct[i], bitXor(plain_text[i], key[k]));
      }
    }

  ct[len1] = '\n';
  return ct;
}

char* encrypt_alg3 ( char* plain_text, char* key ) {

  int i=0, k=0, len1 = strlen(plain_text), len2 = strlen(key);
  char * ct = (char*) malloc(sizeof(char)*(len1+1));
  for (; i<len1; ++i)
    {
      for (; k<len2; ++k) {
        if ( k ==0 )
          ct[i] = bitXor(plain_text[i], key[k]);
        else
          ct[i] = bitXor(ct[i], bitXor(plain_text[i], key[k]));
      }
    }

  ct[len1] = '\n';
  return ct;
}

char* decrypt_alg1 ( char* cipher_text, char* key ) {
  return cipher_text;
}

char* decrypt_alg2 ( char* cipher_text, char* key ) {
  return cipher_text;
}

char* decrypt_alg3 ( char* cipher_text, char* key ) {
  return cipher_text;
}
