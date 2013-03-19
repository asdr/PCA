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
#include "config.h"
#include "logger.h"

CONFIG* load_config_file  ( void ) {
  FILE* configFile = fopen(DEFAULT_CONFIG_FILE_PATH, "rt");
  CONFIG* config = NULL;
  char line[ MAX_KEY_LENGTH + MAX_VALUE_LENGTH + 2 ];

  if ( !configFile )
    {
      // cannot open config file
      log_event( "Unable to open configuration file." );
      return NULL;
    }

  // initialize data structure
  config = (CONFIG*) malloc(sizeof(CONFIG));
  config->pair_count = 0;

  while ( fgets ( line, sizeof line, configFile ) != NULL )
    {
      int i = 0;
      PAIR* pair = &(config->pairs[ config->pair_count ]);
      char * str = strtok(line, "=");
      char message[ 1000 ];

      while ( str != NULL )
        {
          if (i%2 == 0 )
            {
              strcpy( pair->key, str);
            }
          else
            {
              // copy the value except the trailing NewLine character
              strncpy( pair->value, str, strlen(str) - 1);
              sprintf( message, "k( %s ) = v( %s )", pair->key, pair->value );
              log_event( message );
            }

          i += 1;
          str = strtok (NULL, "=");
        }

      config->pair_count += 1;
    }

  // close configuration file
  fclose(configFile);

  log_event( "Configuration data loaded." );

  return config;
}

void destroy_config ( CONFIG* config ) {
  free(config);

  log_event( "Configuration data released." );
}

char* read_configuration ( CONFIG* config, char* key, char default_value[] ) {
    int i = 0;

    if ( !config || !key )
      return default_value;

    for ( ; i<config->pair_count; ++i ) {
      PAIR* pair = &(config->pairs[i]);
      if ( strcmp( key, pair->key ) == 0 )
        return pair->value;
    }

    return default_value;
}
