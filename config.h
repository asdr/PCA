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

#ifndef __PCA_CONFIG_H_
#define __PCA_CONFIG_H_

#define DEFAULT_CONFIG_FILE_PATH "pca.conf"
#define MAX_KEY_LENGTH 100
#define MAX_VALUE_LENGTH 500
#define MAX_PAIR_COUNT 50

typedef struct _configuration_pair PAIR;
typedef struct _configuration CONFIG;

struct _configuration_pair {
  char key[ MAX_KEY_LENGTH ];
  char value[ MAX_VALUE_LENGTH ];
};

struct _configuration {
  int pair_count;
  PAIR pairs[ MAX_PAIR_COUNT ]; // maximum 50 configuration entries is allowed
};


// function declarations

/****
 * load_config_file
 *
 * Reads the contents of the configuration file
 *   and loads a data structure as key-value pairs.
 *   Configuration file is defined hardcoded.
 *   (TODO: File path can be get from main arguments.)
 *
 * @param no parameter
 *
 * @return the data structure that holds the configuration
 *   key-value pairs
 *
****/
CONFIG* load_config_file ( void );

/****
 * destroy_config
 *
 * Releases the allocated memory space for configuration
 *   data structure to operating system.
 *
 * @param config - pointer of configuration data structure
 *
*****/
void    destroy_config ( CONFIG* config );


/****
 * read_configuration
 *
 * Reads a configuration value for a corresponding key.
 *
 * @param config        - pointer of configuration data structure
 * @param key           - configuration key of desired value
 * @param default_value - default value in case of a nonexistant value
 *
 * @return corresponding value of given key, or the given default_value
 *   in case the value doesn't exist.
 *
*****/
char*   read_configuration ( CONFIG* config, char* key, char default_value[] );

#endif
