#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "random.h"
#include <signal.h>

void signal_handler ( int s )
{
  if (s == SIGINT)
    {
      printf("asdr\n");
    }
}

int main (void)
{
  /*int i=0;
  char * text;
  random_open();
  text = random_generate_text(1);
  printf("length: %ld\n", strlen(text));
  printf("str: %s\n", text);
  */

  signal( SIGINT, signal_handler );

  while (1)
    {
      sleep(3);
      printf("loop\n");
    }

  //  random_close();
  return 0;
}
