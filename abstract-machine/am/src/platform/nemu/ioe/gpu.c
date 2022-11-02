#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
#define HEIGHT_ADDR (VGACTL_ADDR)
#define WIDTH_ADDR (VGACTL_ADDR + 2)
int W,H;
void __am_gpu_init() {
  W=inw(WIDTH_ADDR);
  H=inw(HEIGHT_ADDR);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = W, .height = H,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;//(x,y)像素坐标，(w,h)像素宽高
  uint32_t *pixel= (uint32_t *)ctl->pixels;
  fb=fb+y*W+x;
  for (int i = 0; i < h; i ++)
    for(int j=0;j<w;j++)
      fb[i*W+j]=pixel[i*w+j];
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
