#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;    //函数指针
  WriteFn write;   //函数指针
  size_t open_offset; //已经打开文件的偏移量
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define file_num sizeof(file_table)/sizeof(Finfo)

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode)
{
  for(int i=0;i<file_num;i++)
    if(strcmp(pathname,file_table[i].name)==0)
      {
        file_table[i].open_offset =0;
        return i;
      }
  assert(0);
}

size_t fs_read(int fd, void *buf, size_t len){
  Finfo *info = &file_table[fd];
  size_t real_len;
  
  //assert(info->open_offset + len <= info->size);
  if (info->read){
    real_len = info->read(buf, info->open_offset, len);
    info->open_offset += real_len;
  }else {
    real_len = info->open_offset + len <= info->size ?
    len : info->size - info->open_offset;
    ramdisk_read(buf, info->disk_offset + info->open_offset, real_len);
    info->open_offset += real_len;
  }

  return real_len;
}

size_t fs_write(int fd, const void *buf, size_t len){
  //TODO: STDOUT添加支持
  Finfo *info = &file_table[fd];
  size_t real_len;
  
  if (info->write){
    real_len = info->write(buf, info->open_offset, len);
    info->open_offset += real_len;
  }else {
    assert(info->open_offset + len <= info->size);
    ramdisk_write(buf, info->disk_offset + info->open_offset, len);
    real_len = len;
    info->open_offset += len;
  }

  return real_len;
}

size_t fs_lseek(int fd, size_t offset, int whence){
  Finfo *info = &file_table[fd];

  switch(whence){
    case SEEK_CUR:
      assert(info->open_offset + offset <= info->size);
      info->open_offset += offset;
      break;

    case SEEK_SET:
      assert(offset <= info->size);
      info->open_offset = offset;
      break;

    case SEEK_END:
      assert(offset <= info->size);
      info->open_offset = info->size + offset;
      break;

    default:
      assert(0);
  }

  return info->open_offset;
}

int fs_close(int fd)
{
  return 0;
}
