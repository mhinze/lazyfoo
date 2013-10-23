#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <iostream>

using std::cout;

const int FRAMES_PER_SECOND = 40;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

const int DOT_WIDTH = 20;
const int DOT_HEIGHT = 20;

const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

SDL_Surface *dot = NULL;
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;

SDL_Event event;

SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

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

class Dot 
{
   private:
      int x, y;
      int xVel, yVel;
   public:
      Dot();
      void handle_input();
      void move();
      void show();
      void set_camera();
};

Dot::Dot() 
{
   x = 0;
   y = 0;
   xVel = 0;
   yVel = 0;
}

void Dot::handle_input() 
{
   if (event.type == SDL_KEYDOWN)
   {
      switch(event.key.keysym.sym)
      {
         case SDLK_UP:
            yVel -= DOT_HEIGHT / 2;
            break;
         case SDLK_DOWN:
            yVel += DOT_HEIGHT / 2;
            break;
         case SDLK_LEFT:
            xVel -= DOT_WIDTH / 2;
            break;
         case SDLK_RIGHT:
            xVel += DOT_WIDTH / 2;
            break;
         default: break;
      }
   }
   else if (event.type == SDL_KEYUP)
   {
      switch(event.key.keysym.sym)
      {
         case SDLK_UP:
            yVel += DOT_HEIGHT / 2;
            break;
         case SDLK_DOWN:
            yVel -= DOT_HEIGHT / 2;
            break;
         case SDLK_LEFT:
            xVel += DOT_WIDTH / 2;
            break;
         case SDLK_RIGHT:
            xVel -= DOT_WIDTH / 2;
            break;
         default: 
            break;
      }
   }
}

void Dot::show() 
{
   apply_surface(x - camera.x, y - camera.y, dot, screen);
}

void Dot::move()
{
   x += xVel;
   if ((x < 0) || (x + DOT_WIDTH > LEVEL_WIDTH))
   {
      x -= xVel;
   }
   y += yVel;
   if ((y < 0) || (y + DOT_HEIGHT > LEVEL_HEIGHT))
   {
      y -= yVel;
   }
}

void Dot::set_camera()
{
   camera.x = (x + DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
   camera.y = (y + DOT_HEIGHT / 2) - SCREEN_WIDTH / 2;

   if (camera.x < 0)
   {
      camera.x = 0;
   }
   if (camera.y < 0)
   {
      camera.y = 0;
   }
   if (camera.x > LEVEL_WIDTH - camera.w)
   {
      camera.x = LEVEL_WIDTH - camera.w;
   }
   if (camera.y > LEVEL_HEIGHT - camera.h)
   {
      camera.y = LEVEL_HEIGHT - camera.h;
   }
}

bool init()
{
   if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
   {
      return false;
   }

   screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

   if (screen == NULL)
   {
      return false;
   }

   SDL_WM_SetCaption("animation", NULL);

   return true;
}

bool load_files()
{
   dot = load_image("dot.bmp");
   if (dot == NULL)
   {
      return false;
   }
   background = load_image("bg.png");
   if (background == NULL)
   {
      return false;
   }
   
   return true;
}

void clean_up()
{
   SDL_FreeSurface(dot);
   SDL_FreeSurface(background);
   
   SDL_Quit();
}

int main(int argc, char* args[])
{
   bool quit = false;
   int bgX = 0, bgY = 0;

   if (init() == false)
   {
      return 1;
   }
   
   if (load_files() == false)
   {
      return 1;
   }
    
   Timer fps;

   while(quit == false)
   {
      fps.start();

      while (SDL_PollEvent(&event))
      {
         if (event.type == SDL_QUIT)
         {
            quit = true;
         }
      }

      bgX -= 2;

      if (bgX <= -background->w)
      {
         bgX = 0;
      }

      apply_surface(bgX, bgY, background, screen);
      apply_surface(bgX + background->w, bgY, background, screen);
      apply_surface(310, 230, dot, screen);

      if (SDL_Flip(screen) == -1)
      {
         return 1;
      }

      if (fps.get_ticks() < 1000 / FRAMES_PER_SECOND)
      {
         SDL_Delay((1000/FRAMES_PER_SECOND) - fps.get_ticks());
      }
   }
   clean_up();
   return 0;
}
