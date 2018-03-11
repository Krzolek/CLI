#include <stdio.h>
#include "CliMod/CliMod.h"
#include <stdbool.h>

/*
 *
 *
 */
int main(int argc, char const *argv[]) {
  while(1)
  {
     bool ack = true;
    char buf[100];
    int test;
    if(fgets(buf,sizeof(buf),stdin))
    {
      test = 0;
      while (buf[test] && test < 100)
      {
        test++;
      }

      Cli_PutString((uint16_t)test,buf);
      //printf("test: %d, buf: %s", test, buf);
    }

  }
  return 0;
}
