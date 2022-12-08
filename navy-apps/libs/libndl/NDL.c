#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
static int canvas_x, canvas_y;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint32_t time=tv.tv_sec*1000+tv.tv_usec/1000;
  return time;
}

int NDL_PollEvent(char *buf, int len) {
  int fd=open("/dev/events",0,0);
  int count=read(fd,buf,len);
  if(count>0)
    return 1;
  return 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  if(*w==0 && *h==0)
    {canvas_w=screen_w;
     canvas_h=screen_h;
     *w=screen_w;
     *h=screen_h;
     }
  else 
  {
    assert(*w<=screen_w && *h<=screen_h);
    canvas_w=*w;
    canvas_h=*h;
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int fd=open("/dev/fb",0,0);
  size_t offset, len;
  printf("x: %d y: %d\n",x,y);
  canvas_x=(screen_w-canvas_w)/2;
  canvas_y=(screen_h-canvas_h)/2;
  printf("canvas_x: %d, y: %d\n",canvas_x,canvas_y);
  printf("width:%d height:%d\n",canvas_w,canvas_h);
  offset = canvas_y*screen_w+canvas_x+ y*canvas_w+x;
  for(int i=0;i<h;i++)
  {
    lseek(fd,offset,SEEK_SET);
    write(fd,pixels,w);
    pixels+=w;
    offset+=screen_w*4;
  }
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  int fd=open("/proc/dispinfo",0,0);
  char buf[100];
  read(fd, buf,100);
  close(fd);
  char b[100];//对文件进行格式化处理，删掉空格
  int end=0;
  for(int i=0;i<100;i++) 
    if(buf[i]!=' '&&buf[i]!='\0')
      b[end++]=buf[i];
  b[end]='\0';
  
  char *str=strtok(b,"\n");
  char key[50];
  int value;
  sscanf(str,"%[a-zA-Z]:%d", key, &value);
  if(strcmp(key,"WIDTH")==0)
    screen_w=value;
  else if(strcmp(key,"HEIGHT")==0)
    screen_h=value;
  
  str=strtok(NULL,"\n");
  sscanf(str,"%[a-zA-Z]:%d", key, &value);
  if(strcmp(key,"WIDTH")==0)
    screen_w=value;
  else if(strcmp(key,"HEIGHT")==0)
    screen_h=value;
  printf("WIDTH: %d, HEIGHT: %d\n",screen_w,screen_h);
  
  return 0;
}

void NDL_Quit() {
}
