#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

//将十进制数num转换为base进制的字符串
int convert(char *out,int end,uint64_t num,int base)
{ 
  char dict[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
  char output[50];
  int index=0;
  if(num==0)
    output[index++]='0';
  while(num>0)
    {
      int bit=num%base;
      output[index++]=dict[bit];
      num/=base;
    }
  for(int i=index-1;i>=0;i--)
  {
    out[end++]=output[i];
  }
  return end;
}

int cat(char *out, char *src,int end)
{
  while(*src!='\0')
    out[end++]=*src++;
  out[end]='\0';
  return end;
}

int printf(const char *fmt, ...) {
//  panic("Not implemented");
  char buf[4096];
  va_list args;
  va_start(args, fmt);
  vsprintf(buf,fmt, args);
  int i=0;
  while(buf[i]!='\0') 
    putch(buf[i++]);
  return i;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
//  panic("Not implemented");
  size_t len=strlen(fmt);
  int end=0;
  int64_t arg_int=0;
  char *arg_str=NULL;
  char c;
  uint64_t unum64;
  uint32_t unum32;
  for(int i=0;i<len;i++)
    {
      if(fmt[i]!='%')
        out[end++]=fmt[i];
      else if(i+1<len)//后面是需要格式化输出的内容
      {
        switch(fmt[i+1])
        { 
          case 's':
          arg_str=va_arg(ap,char*);
          end=cat(out,arg_str,end);
          i++;
          break;
          
          case 'd':
          arg_int=va_arg(ap,int);
          if(arg_int<0)
          {
            out[end++]='-';
            arg_int=-arg_int;
           }
          end=convert(out,end,arg_int,10);
          i++;
          break;
          
          case 'c':
          c = (char)va_arg(ap, int);
          out[end++]=c;
          i++;
          break;

          case 'u':
          unum64=va_arg(ap, uint64_t);
          end=convert(out,end,unum64,10);
          i++;
          break;

          case 'p':
            unum32 = va_arg(ap, uint32_t);
            out[end++]='0';
            out[end++]='x';
            end=convert(out,end,unum32,16);
            i++;
            break;
    
        }
      }
      else
        out[end++]=fmt[i];
    }
  out[end]='\0';
  return end;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int end=vsprintf(out, fmt, args);
  return end;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int end=vsnprintf(out, n, fmt, args);
  return end;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  int end=vsprintf(out,fmt,ap);
  out[n]='\0';
  if(end>n)
    return n;
  else
    return end;
}

#endif
