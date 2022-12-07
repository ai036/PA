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
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  {"/dev/events",0, 0, events_read, invalid_write},
  {"/dev/fb",0, 0,invalid_read, invalid_write},
  {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

void get_name(int fd,char *buf)
{
  buf=file_table[fd].name;
}

#define file_num sizeof(file_table)/sizeof(Finfo)

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[4].size=io_read(AM_GPU_CONFIG).width*io_read(AM_GPU_CONFIG).height*4;
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

size_t fs_read(int fd, void *buf, size_t len)
{
  assert(file_table[fd].open_offset <= file_table[fd].size);
  size_t ret=0;
  if(file_table[fd].read)
    ret=file_table[fd].read(buf, 0,len);
  else
    {ret=ramdisk_read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);
    }

    file_table[fd].open_offset+=len;

  return ret;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  assert(file_table[fd].open_offset <= file_table[fd].size);
  size_t ret=0;
  if(file_table[fd].write)
    ret=file_table[fd].write(buf, 0, len);
  else
    ret=ramdisk_write(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);

  if(file_table[fd].open_offset+len<=file_table[fd].size)
    file_table[fd].open_offset+=len;
  else
    {ret=file_table[fd].size-file_table[fd].open_offset;
     file_table[fd].open_offset=file_table[fd].size;
     }
  return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  switch(whence){
    case SEEK_SET:file_table[fd].open_offset = offset;break;
    case SEEK_CUR:file_table[fd].open_offset += offset;break;
    case SEEK_END:
    assert(offset==0);
    file_table[fd].open_offset = file_table[fd].size+offset;break;
  }
  return file_table[fd].open_offset;//这里不能返回0
}

int fs_close(int fd)
{
  return 0;
}
