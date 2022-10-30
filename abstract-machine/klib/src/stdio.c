#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

//将十进制数num转换为base进制的字符串
int convert(char *out,int end,int num,int base)
{ 
  char dict[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
  char output[50];
  int index=0;
  if(num==0)
    output[index++]='0';
  else if(num==-2147483648)  //最小负数特殊情况
    {
      out[end++]='-';
      output[index++]=dict[-(num%base)];
      num/=base;
      num=-num;
    }
  else if(num<0)
    {
      out[end++]='-';
      num=-num;
    }
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
  char buf[1024];
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
  int arg_int=0;
  char *arg_str=NULL;
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
          end=convert(out,end,arg_int,10);
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
//  panic("Not implemented");
  va_list args;
  va_start(args, fmt);
  size_t len=strlen(fmt);
  int end=0;
  int arg_int=0;
  char *arg_str=NULL;
  for(int i=0;i<len;i++)
    {
      if(fmt[i]!='%')
        out[end++]=fmt[i];
      else if(i+1<len)//后面是需要格式化输出的内容
      {
        switch(fmt[i+1])
        { 
          case 's':
          arg_str=va_arg(args,char*);
          end=cat(out,arg_str,end);
          i++;
          break;
          
          case 'd':
          arg_int=va_arg(args,int);
          end=convert(out,end,arg_int,10);
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

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
