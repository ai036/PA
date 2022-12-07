#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  int ret=0;
  char* start=(char*)buf;
  for(int i = 0; i < len; i++)
  {  putch(start[i]);
     ret++;
     }
  return ret;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  char *k;
  if (ev.keycode == AM_KEY_NONE) return 0;
  if (ev.keydown)
    k="kd ";
  else
    k="ku ";
  strcpy(buf,k);
  strcat(buf,keyname[ev.keycode]);
  printf("Got  (kbd): %s (%d) %s\n", keyname[ev.keycode], ev.keycode, ev.keydown ? "DOWN" : "UP");
  return len;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {

  int width=io_read(AM_GPU_CONFIG).width;
  int height=io_read(AM_GPU_CONFIG).height;
  printf("(%d,%d)\n",width,height);

  sprintf((char*)buf,"WIDTH:%d\nHEIGHT:%d",width,height);

  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
