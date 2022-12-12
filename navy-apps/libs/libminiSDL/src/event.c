#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  ev->type=SDL_KEYUP;
  ev->key.keysym.sym=SDLK_NONE;
  char buf[100];
  char type[10];
  char key[10];

  NDL_PollEvent(buf,20);
  sscanf(buf,"%s %s",type,key);
  if(type[1]=='u')
    return 0;
  else if(type[1]=='d')
    ev->type = SDL_KEYDOWN;

  switch(strlen(key))
  {
    case 1:
      char c=key[0];
      if(c>='1'&&c<='9')
        ev-> key.keysym.sym= 15+c-'1';
      else if(c=='0')
        ev->key.keysym.sym= SDLK_0;
      else if(c=='J')
        ev->key.keysym.sym=SDLK_J;
      else if(c=='K')
        ev->key.keysym.sym=SDLK_K;
      else if(c=='G')
        ev->key.keysym.sym=SDLK_G;
      break;
    case 2:
      ev->key.keysym.sym=SDLK_UP;
      break;
    case 4:
      if(key[0]=='D')
        ev->key.keysym.sym=SDLK_DOWN;
      else if(key[0]=='L')
        ev->key.keysym.sym=SDLK_LEFT;
      else if(key[0]=='R')
        ev->key.keysym.sym=SDLK_RIGHT;
      else
        ev->key.keysym.sym=SDLK_NONE;
      break;
    default:
      ev->key.keysym.sym=SDLK_NONE;
  }
  
  if(ev->type=SDL_KEYDOWN)
    printf("type:%s key:%s\n",type,key);
  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  event->type=SDL_KEYUP;
  event->key.keysym.sym=SDLK_NONE;
  char buf[100];
  char type[10];
  char key[10];
  while(event->type==SDL_KEYUP){
  NDL_PollEvent(buf,20);
  sscanf(buf,"%s %s",type,key);
  if(type[1]=='d')
    event->type = SDL_KEYDOWN;
//  else if(type[1]=='u')
//    event->type = SDL_KEYUP;

  switch(strlen(key))
  {
    case 1:
      char c=key[0];
      if(c>='1'&&c<='9')
        event-> key.keysym.sym= 15+c-'1';
      else if(c=='0')
        event->key.keysym.sym= SDLK_0;
      else if(c=='J')
        event->key.keysym.sym=SDLK_J;
      else if(c=='K')
        event->key.keysym.sym=SDLK_K;
      else if(c=='G')
        event->key.keysym.sym=SDLK_G;
      break;
    case 2:
      event->key.keysym.sym=SDLK_UP;
      break;
    case 4:
      if(key[0]=='D')
        event->key.keysym.sym=SDLK_DOWN;
      else if(key[0]=='L')
        event->key.keysym.sym=SDLK_LEFT;
      else if(key[0]=='R')
        event->key.keysym.sym=SDLK_RIGHT;
      else
        event->key.keysym.sym=SDLK_NONE;
      break;
    default:
      event->key.keysym.sym=SDLK_NONE;
  }
  }
  if(event->type=SDL_KEYDOWN)
    printf("type:%s key:%s\n",type,key);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
