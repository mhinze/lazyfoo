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

   message = load_image("hello.bmp");
   backgroud = load_image("background.bmp");
   
   apply_surface(0, 0, backgroud, screen);

   apply_surface(320, 0, background, screen);
   apply_surface(0, 240, background, screen);
   apply_surface(320, 240, background, screen);
   apply_surface(180, 140, message, screen);

   if (SDL_Flip(screen) == -1)
   {
      return 1;
   }
   
   SDL_Delay(2000);

   SDL_FreeSurface(message);
   SDL_FreeSurface(backgroud);
   SDL_Quit();
   return 0;
}
