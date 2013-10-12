#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>
#include <sstream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *info = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *seconds = NULL;
TTF_Font *font = NULL;
SDL_Event event;
SDL_Color textColor = {255, 255, 255};

void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL)
{
   SDL_Rect offset;
   offset.x = x;
   offset.y = y;
   SDL_BlitSurface(source, clip, destination, &offset);
}

SDL_Surface *load_image(std::string filename)
{
   SDL_Surface* loadedImage = NULL;
   SDL_Surface* optimizedImage = NULL;
   loadedImage = IMG_Load(filename.c_str());
   if (loadedImage != NULL)
   {
      optimizedImage = SDL_DisplayFormat(loadedImage);
      SDL_FreeSurface(loadedImage);
      Uint32 colorkey = SDL_MapRGB(optimizedImage->format, 0, 0xFF, 0xFF);
      SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY, colorkey);
   }
   return optimizedImage;
}

bool init()
{
   if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
   {
      return false;
   }

   if (TTF_Init() == -1)
   {
      return false;
   }

   screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

   if (screen == NULL)
   {
      return false;
   }

   SDL_WM_SetCaption("timing", NULL);

   return true;
}

bool load_files()
{
   font = TTF_OpenFont("lazy.ttf", 28);
   if (font == NULL)
   {
      return 1;
   }

   return true;
}

void clean_up()
{
   TTF_CloseFont(font);
   
   TTF_Quit();
   SDL_Quit();
}

int main(int argc, char* args[])
{
   bool quit = false;
   Uint32 start = 0;
   bool running = true;

   if (init() == false)
   {
      return 1;
   }
   
   if (load_files() == false)
   {
      return 1;
   }
     
   start = SDL_GetTicks();
   info = TTF_RenderText_Solid(font, "Press s", textColor);

   while(quit == false)
   {
      while (SDL_PollEvent(&event))
      {
         if (event.type == SDL_QUIT)
         {
            quit = true;
         }
         if (event.type == SDL_KEYDOWN)
         {
            if (event.key.keysym.sym == SDLK_s)
            {
               if (running == true)
               {
                  // stop the timer
                  running = false;
                  start = 0;
               }
               else
               {
                  // start the timer
                  running = true;
                  start = SDL_GetTicks();
               }
            }
         }
      }

      SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0x00, 0x00, 0x00 ) );
     apply_surface((SCREEN_WIDTH - info->w) /2, 100, info, screen);
     
         if (running == true)
         {
            std::stringstream time;
            time << "Timer: " << SDL_GetTicks() - start;

            seconds = TTF_RenderText_Solid(font, time.str().c_str(), textColor);

            apply_surface((SCREEN_WIDTH, seconds->w) / 2, 50, seconds, screen);

            SDL_FreeSurface(seconds);
         }
         if (SDL_Flip(screen) == -1)
         {
            return 1;
      }
   }

   clean_up();
   return 0;
}
