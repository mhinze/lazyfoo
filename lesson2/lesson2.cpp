#include "SDL/SDL.h"
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *message = NULL;
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;

SDL_Surface *load_image(std::string filename)
{
   SDL_Surface* loadedImage = NULL;
   SDL_Surface* optimizedImage = NULL;
   if (loadedImage != NULL)
   {
      optimizedImage = SDL_DisplayFormat(loadedImage);
      SDL_FreeSurface(loadedImage);
   }
   return optimizedImage;
}

void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination)
{
   SDL_Rect offset;
   offset.x = x;
   offset.y = y;
   SDL_BlitSurface(source, NULL, destination, &offset);
}

int main(int arg, char* args[])
{
   if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
   {
      return 1;
   }
   
   screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

   SDL_WM_SetCaption("Hello World", NULL);

   message = load_image("hello2.bmp");

    SDL_Surface* hello = NULL;
   SDL_Surface* screen = NULL;

   SDL_Init(SDL_INIT_EVERYTHING);

   screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);

   hello = SDL_LoadBMP("hello.bmp");

   SDL_BlitSurface(hello, NULL, screen, NULL);

   SDL_Flip(screen);

   SDL_Delay(20000);

   SDL_FreeSurface(hello);

   SDL_Quit();

   return 0;
}
