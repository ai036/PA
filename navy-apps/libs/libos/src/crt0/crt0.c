#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  printf("call_main\n");
  printf("argc=%d\n",*args);
  int argc=*args;
  args+=1;
  char* argv[argc];
  for(int i=0; i<argc; i++)
    {
      argv[i]=(char*)*args;
      printf("%s\n",argv[i]);
      args++;
    }

  char *empty[] =  {NULL };
  environ = empty;
  exit(main(argc, argv, empty));
  assert(0);
}
