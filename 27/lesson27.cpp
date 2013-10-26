#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using std::cout;

const int FRAMES_PER_SECOND = 20;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *front = NULL;
SDL_Surface *back = NULL;
SDL_Surface *screen = NULL;
TTF_Font *font = NULL;
SDL_Event event;

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

class Timer
{
   private:
      int startTicks;
      int pausedTicks;
      bool paused;
      bool started;
   public:
      Timer();
      void start();
      void stop();
      void pause();
      void unpause();
      int get_ticks();
      bool is_started();
      bool is_paused();
};

Timer::Timer()
{
   startTicks = 0;
   pausedTicks = 0;
   paused = false;
   started = false;
}

void Timer::start()
{
   started = true;
   paused = false;
   startTicks = SDL_GetTicks();
}

void Timer::stop()
{
   started = false;
   paused = false;
}

int Timer::get_ticks()
{
   if(started == true)
   {
      if (paused == true)
      {
         return pausedTicks;
      }
      else
      {
         return SDL_GetTicks() - startTicks;
      }
   }
   return 0;
}

void Timer::pause()
{
   if ((started == true) && (paused == false))
   {
      paused = true;
      pausedTicks = SDL_GetTicks() - startTicks;
   }
}

void Timer::unpause()
{
   if (paused == true)
   {
      paused = false;
      startTicks = SDL_GetTicks() - pausedTicks;
      pausedTicks = 0;
   }
}

bool Timer::is_started()
{
   return started;
}

bool Timer::is_paused()
{
   return paused;
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

   SDL_WM_SetCaption("27", NULL);

   return true;
}

bool load_files()
{
   back = load_image("fadein.png");
   front = load_image("fadeout.png");
   if (back == NULL || front == NULL)
   {
      return 1;
   }

   return true;
}

void clean_up()
{
   SDL_FreeSurface(back);;
   SDL_FreeSurface(front);;
   TTF_Quit();
   SDL_Quit();
}

int main(int argc, char* args[])
{
   bool quit = false;

   int alpha = SDL_ALPHA_OPAQUE;
   Timer fps;

   if (init() == false)
   {
      return 1;
   }
   
   if (load_files() == false)
   {
      return 1;
   }
     
   while(quit == false)
   {
      fps.start();


   
      Uint8 *keystates = SDL_GetKeyState(NULL);
      if(keystates[SDLK_UP])
      {
         if (alpha < SDL_ALPHA_OPAQUE)
         {
            alpha += 5;
         }
      }
      if (keystates[SDLK_DOWN])
      {
         if (alpha > SDL_ALPHA_TRANSPARENT)
         {
            alpha -= 5;
         }
      }

      SDL_SetAlpha(front, SDL_SRCALPHA, alpha);
      apply_surface(0, 0, back, screen);
      apply_surface(0, 0, front, screen);

      if (SDL_Flip(screen) == -1)
      {
         return 1;
      }

      if (fps.get_ticks() < 1000 / FRAMES_PER_SECOND)
      {
         SDL_Delay((1000/FRAMES_PER_SECOND) - fps.get_ticks());
      }

      while (SDL_PollEvent(&event))
      {
         if (event.type == SDL_QUIT)
         {
            quit = true;
         }

      }
   }
   clean_up();
   return 0;
}
