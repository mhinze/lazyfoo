#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>
#include <sstream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *screen = NULL;
SDL_Surface *image = NULL;
TTF_Font *font = NULL;
SDL_Event event;
SDL_Color textColor = {255, 255, 255};

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

   image = load_image("testing.png");
   if (image == NULL)
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

   if (init() == false)
   {
      return 1;
   }
   
   if (load_files() == false)
   {
      return 1;
   }
     
   int frame = 0;
   Timer fps;
   Timer update;

   update.start();
   fps.start();

   while(quit == false)
   {
      while (SDL_PollEvent(&event))
      {
         if (event.type == SDL_QUIT)
         {
            quit = true;
         }
      }

      apply_surface(0, 0, image, screen);

      if (SDL_Flip(screen) == -1)
      {
         return 1;
      }
      frame++;
      if (update.get_ticks() > 1000)
      {
         std::stringstream caption;
         caption << "Average Frames Per Second: " << frame / (fps.get_ticks() / 1000.f);
         SDL_WM_SetCaption(caption.str().c_str(), NULL);
         update.start();
      }
   }
   clean_up();
   return 0;
}
