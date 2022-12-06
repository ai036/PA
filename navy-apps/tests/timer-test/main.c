#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <stdint.h>
#include <NDL.h>

int main(){
  printf("Start test. %d\n", sizeof(struct timeval));

  struct timeval tv;
  int ret=gettimeofday(&tv,NULL);

  uint32_t time;
  uint32_t msec = 500;
  printf("%u\n",time);
  while (1) {
    while(time < msec) {
  	ret=gettimeofday(&tv,NULL);
  	time=tv.tv_sec*1000+tv.tv_usec/1000;
  	printf("%u\n",time);
    };
    printf("time pass 0.5s\n");

    msec += 500;
  }

  NDL_Quit();
}
