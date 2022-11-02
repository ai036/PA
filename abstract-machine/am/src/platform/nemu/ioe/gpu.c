#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
#define HEIGHT_ADDR (VGACTL_ADDR + 2)
#define WIDTH_ADDR (VGACTL_ADDR)

void __am_gpu_init() {

}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = inw(WIDTH_ADDR), .height = inw(HEIGHT_ADDR),
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;//(x,y)像素坐标，(w,h)像素宽高
  int W=inw(WIDTH_ADDR);
  uint32_t *pixel= (uint32_t *)ctl->pixels;
  if(h==0||w==0)
    return;
  fb=fb+y*W+x;
  for (int i = 0; i < h; i ++)
    for(int j=0;j<w;j++)
//      outl(FB_ADDR+(y*W+x+i*w+j)*4,pixel[0]);
        fb[i*w+j]=pixel[i*w+j];

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
