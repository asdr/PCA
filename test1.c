#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "random.h"

int main (void)
{
  int i=0;
  char * text;
  random_open();
  text = random_generate_text(15);
  printf("length: %ld\n", strlen(text));
  printf("str: %s\n", text);

  text = random_generate_key(15*8);
  printf("length: %ld\n", strlen(text));
  printf("str: %s\n", text);

  for (;i<15;++i)
    {
      printf("%d ", text[i]);
    }

  printf("\n");
  random_close();
  return 0;
}
