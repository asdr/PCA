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

#ifndef __PCA_LOGGER_H_
#define __PCA_LOGGER_H_

#define DEFAULT_LOG_FILE_PATH "pca.log"
#define MAX_LOG_LINE_LENGTH 20000


/****
 * log_event
 *
 * Logs given messsage to a pre-defined log file with a current
 *   time and process id. The format of a log entry is as follows:
 *   YEAR/MONTH/DAY HOUR:MIN:SEC.MICROSEC [PID:PID] - message
 *
 * @param message - the message which is wanted to be logged.
 *
*****/
void log_event ( char* message );

/****
 * log_open_file
 *
 * Opens log log file in shared mode. And places the file descriptor
 *   into a global named: __pca_global_log_fd
 *
*****/
int log_open_file ( char* file_path );

/****
 * log_close_file
 *
 * Closes the open log file. The log file file descripter is read
 *   from global named: __pca_global_log_fd
 *
 * @param message - the message which is wanted to be logged.
 *
*****/
void log_close_file ( );

#endif
