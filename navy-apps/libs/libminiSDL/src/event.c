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
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[100];
  NDL_PollEvent(buf,20);
  char type[10];
  char key[10];
  sscanf(buf,"%s %s",type,key);
  if(strcmp(type,"kd")==0)
    event->type = SDL_KEYDOWN;
  else if(strcmp(type,"ku")==0)
    event->type = SDL_KEYUP;

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
      event->key.keysym.sym=SDLK_DOWN;
      break;
    default:
      event->key.keysym.sym=SDLK_NONE;
  }

  printf("type:%s key:%s\n",type,key);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
