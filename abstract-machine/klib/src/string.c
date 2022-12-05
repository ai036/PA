#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t count=0;
  while(s[count]!='\0')
    count++;
  return count;
}

char *strcpy(char *dst, const char *src) {
  char* c=dst;
  size_t len=strlen(src);
  for(int i=0;i<len;i++)
    dst[i]=src[i];
  dst[len]='\0';
  return c;
}

char *strncpy(char *dst, const char *src, size_t n) {
  if(strlen(src)<n)
    assert(0);
  char *c=dst;
  for(int i=0;i<n;i++)
    dst[i]=src[i];
  dst[n]='\0';
  return c;
}

char *strcat(char *dst, const char *src) {
  size_t len1=strlen(dst);
  size_t len2=strlen(src);
  char* c=dst;
  for(int i=0;i<=len2;i++)
    dst[len1+i]=src[i];
  return c;
}

int strcmp(const char *s1, const char *s2) {
  assert(s1!=NULL&&s2!=NULL);
  while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
  return *s1-*s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  assert(s1!=NULL&&s2!=NULL);
  while(--n && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
  return *s1-*s2;
}

void *memset(void *s, int c, size_t n) {
  char* p=(char *)s;
  while(n--)
    {
      *p=(char)c;
      p++;
    }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  assert(dst!=NULL&&src!=NULL);
  char* d=(char*)dst;
  char* s=(char*)src;
  if(s<=d || s+n<=d)
    {
      while(n--)
        {
          *d=*s;
          d++;
          s++;
        }
    }
  else
    {
      while(n--)
      {
        *(d+n-1)=*(s+n-1);
      }
    }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  char *char_out = (char *)out;
  const char *char_in = (const char *)in;
  for (int i = 0; i < n; ++i){
    char_out[i] = char_in[i];
  }

  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  assert(s1!=NULL&&s2!=NULL);
  char* p1=(char*)s1;
  char* p2=(char*)s2;
  while(--n && *p1 && (*p1 == *p2))
    {
        p1++;
        p2++;
    }
  return *p1-*p2;
}

#endif
//经测试通过了string测试，但可能还存在bug