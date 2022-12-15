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

void parse_key(char *key, SDL_Event *ev)
{
    switch(strlen(key))
  {
    case 1:
      char c=key[0];
      if(c>='1'&&c<='9')
        ev-> key.keysym.sym= 15+c-'1';
      else if(c=='0')
        ev->key.keysym.sym= SDLK_0;
      else 
        switch(c)
        {
          case 'Q':ev->key.keysym.sym= SDLK_Q;break;
          case 'W':ev->key.keysym.sym= SDLK_W;break;
          case 'E':ev->key.keysym.sym= SDLK_E;break;
          case 'R':ev->key.keysym.sym= SDLK_R;break;
          case 'T':ev->key.keysym.sym= SDLK_T;break;
          case 'Y':ev->key.keysym.sym= SDLK_Y;break;
          case 'U':ev->key.keysym.sym= SDLK_U;break;
          case 'I':ev->key.keysym.sym= SDLK_I;break;
          case 'O':ev->key.keysym.sym= SDLK_O;break;
          case 'P':ev->key.keysym.sym= SDLK_P;break;
          case 'A':ev->key.keysym.sym= SDLK_A;break;
          case 'S':ev->key.keysym.sym= SDLK_S;break;
          case 'D':ev->key.keysym.sym= SDLK_D;break;
          case 'F':ev->key.keysym.sym= SDLK_F;break;
          case 'G':ev->key.keysym.sym= SDLK_G;break;
          case 'H':ev->key.keysym.sym= SDLK_H;break;
          case 'J':ev->key.keysym.sym= SDLK_J;break;
          case 'K':ev->key.keysym.sym= SDLK_K;break;
          case 'L':ev->key.keysym.sym= SDLK_L;break;
          case 'Z':ev->key.keysym.sym= SDLK_Z;break;
          case 'X':ev->key.keysym.sym= SDLK_X;break;
          case 'C':ev->key.keysym.sym= SDLK_C;break;
          case 'V':ev->key.keysym.sym= SDLK_V;break;
          case 'B':ev->key.keysym.sym= SDLK_B;break;
          case 'N':ev->key.keysym.sym= SDLK_N;break;
          case 'M':ev->key.keysym.sym= SDLK_M;break;
        }
      break;
    case 2:
      if(key[0]=='F')
        ev->key.keysym.sym=SDLK_F1+(key[1]-'1');
      else
        ev->key.keysym.sym=SDLK_UP;
      break;
    case 3:
      switch(key[2])
      {
        case '0':ev->key.keysym.sym=SDLK_F10;break;
        case '1':ev->key.keysym.sym=SDLK_F11;break;
        case '2':ev->key.keysym.sym=SDLK_F12;break;
        case 'B':ev->key.keysym.sym=SDLK_TAB;break;
        case 'D':ev->key.keysym.sym=SDLK_END;break;
      }
      break;
    case 4:
      switch(key[0])
      {
        case 'L': if(key[1]=='A') ev->key.keysym.sym=SDLK_LALT;
                else ev->key.keysym.sym=SDLK_LEFT;break;
        case 'R': ev->key.keysym.sym=SDLK_RALT;break;
        case 'D': ev->key.keysym.sym=SDLK_DOWN;break;
        case 'H': ev->key.keysym.sym=SDLK_HOME;break;
        case 'N': ev->key.keysym.sym=SDLK_NONE;break;
      }
      break;
    case 5:
      switch(key[0])
      {
        case 'G': ev->key.keysym.sym=SDLK_GRAVE;break;
        case 'M': ev->key.keysym.sym=SDLK_MINUS;break;
        case 'C': ev->key.keysym.sym=SDLK_COMMA;break;
        case 'S': if(key[1]=='L') ev->key.keysym.sym=SDLK_SLASH;
                else ev->key.keysym.sym=SDLK_SPACE;break;
        case 'L': ev->key.keysym.sym=SDLK_LCTRL;break;
        case 'R': if(key[1]=='C') ev->key.keysym.sym=SDLK_RCTRL;
                else ev->key.keysym.sym=SDLK_RIGHT;break;
      }
      break;
    case 6:
      switch(key[0])
      {
        case 'E': if(key[1]=='S') ev->key.keysym.sym=SDLK_ESCAPE;
                else ev->key.keysym.sym=SDLK_EQUALS;break;
        case 'R': if(key[1]=='S') ev->key.keysym.sym=SDLK_RSHIFT;
                else ev->key.keysym.sym=SDLK_RETURN;break;
        case 'L': ev->key.keysym.sym=SDLK_LSHIFT;break;
        case 'P': if(key[1]=='E') ev->key.keysym.sym=SDLK_PERIOD;
                else ev->key.keysym.sym=SDLK_PAGEUP;break;
        case 'I': ev->key.keysym.sym=SDLK_INSERT;break;
        case 'D': ev->key.keysym.sym=SDLK_DELETE;break;
      }
      break;
    case 8:
      switch (key[0])
      {
      case 'C': ev->key.keysym.sym=SDLK_CAPSLOCK;break;
      case 'P': ev->key.keysym.sym=SDLK_PAGEDOWN;break;
      }
      break;
    case 9:
      switch (key[8])
      {
        case 'E': ev->key.keysym.sym=SDLK_BACKSPACE;break;
        case 'H': ev->key.keysym.sym=SDLK_BACKSLASH;break;
        case 'N': ev->key.keysym.sym=SDLK_SEMICOLON;break;
      }
      break;
    case 10:
      ev->key.keysym.sym=SDLK_APOSTROPHE;break;
    case 11:
      if(key[0]=='L') ev->key.keysym.sym=SDLK_LEFTBRACKET;
      else ev->key.keysym.sym=SDLK_APPLICATION;break;
    case 12:
      ev->key.keysym.sym=SDLK_RIGHTBRACKET;break;
    default:
      ev->key.keysym.sym=SDLK_NONE;
  }
}

int SDL_PollEvent(SDL_Event *ev) {
  ev->key.keysym.sym=SDLK_NONE;
  char buf[30];
  char type[10];
  char key[15];

  int ret=NDL_PollEvent(buf,25);
  if(!ret)return 0;
  sscanf(buf,"%s %s",type,key);
  parse_key(key,ev);
  if(type[1]=='u'){
    ev->type=SDL_KEYUP;
  }
  else if(type[1]=='d')
    ev->type = SDL_KEYDOWN;
  else
    return 0;

  printf("type:%s key:%s\n",type,key);
  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  printf("Waiting for\n");
  event->key.keysym.sym=SDLK_NONE;
  char buf[30];
  char type[10];
  char key[15];
  int ret=NDL_PollEvent(buf,25);
  while(1){
  ret=NDL_PollEvent(buf,25);
  if(ret)
    {sscanf(buf,"%s %s",type,key);
     parse_key(key,event);
    if(type[1]=='u')
      event->type = SDL_KEYUP;
    else if(type[1]=='d')
      event->type = SDL_KEYDOWN;
    break;
    }
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
