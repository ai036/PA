#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <stdint.h>
#include <NDL.h>

int main(){
  printf("Start test. %d\n", sizeof(struct timeval));

  struct timeval tv;
  int ret=gettimeofday(&tv,NULL);

  uint32_t time=NDL_GetTicks();
  uint32_t msec = 500;
  printf("%u\n",time);
  while (1) {
    while(time < msec) {
      uint32_t time=NDL_GetTicks();
    };
    printf("time pass 0.5s\n");

    msec += 500;
  }

  NDL_Quit();
}
