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
const int DOT_WIDTH = 10; 
const int DOT_HEIGHT = 10; 

SDL_Surface *screen = NULL;
SDL_Surface *dot = NULL;
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

      void set_x(int X);
      void set_y(int Y);
      int get_x();
      int get_y();
};

int Dot::get_x()
{
   return x;
}

int Dot::get_y()
{
   return y;
}

void Dot::set_x(int X)
{
   x = X;
}

void Dot::set_y(int Y)
{
   y = Y;
}

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
         case SDLK_UP: yVel -= DOT_HEIGHT /2; break;
         case SDLK_DOWN: yVel += DOT_HEIGHT / 2; break;
         case SDLK_LEFT: xVel -= DOT_WIDTH / 2; break;
         case SDLK_RIGHT: xVel += DOT_WIDTH / 2; break;
      }
   }
   if (event.type == SDL_KEYUP)
   {
      switch(event.key.keysym.sym)
      {
         case SDLK_UP: yVel += DOT_HEIGHT / 2; break;
         case SDLK_DOWN: yVel -= DOT_HEIGHT /2; break;
         case SDLK_LEFT: xVel += DOT_WIDTH /2; break;
         case SDLK_RIGHT: xVel -= DOT_WIDTH /2; break;
      }
   }
}

void Dot::move()
{
   x += xVel;

   if ( (x<0) || (x+DOT_WIDTH>SCREEN_WIDTH))
   {
      x -= xVel;
   }
   y += yVel;
   if ((y < 0) || (y + DOT_HEIGHT > SCREEN_HEIGHT))
   {
      y -= yVel;
   }
}

void Dot::show()
{
   cout << "Showing dot at x: " << x << " y: " <<  y << "\n";
   apply_surface(x, y, dot, screen);
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

   SDL_WM_SetCaption("Save Games", NULL);

   return true;
}

bool load_files(Dot &thisDot, Uint32 &bg)
{
   dot = load_image("dot.png");
   if (dot == NULL)
   {
      return 1;
   }

   std::ifstream load("game_save");

   if (load != NULL)
   {
      int offset;
      std::string level;
      load >> offset;
      thisDot.set_x(offset);
      load >> offset;
      thisDot.set_y(offset);

      if ((thisDot.get_x() < 0) || (thisDot.get_x() > SCREEN_WIDTH - DOT_WIDTH))
      {
         return false;
      }

      if ((thisDot.get_y() < 0) || (thisDot.get_y() > SCREEN_HEIGHT - DOT_HEIGHT))
      {
         return false;
      }

      load.ignore();

      getline(load, level);

      if (load.fail() == true)
      {
         return false;
      }
      
      if (level == "White Level")
      {
         bg = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
      }
      else if (level == "Red Level")
      {
         bg = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
      }
      else if (level == "Green Level")
      {
         bg = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
      }
      else if (level == "Blue Level")
      {
         bg = SDL_MapRGB(screen->format, 0x00, 0x00, 0xFF);
      }
      load.close();
   }
   return true;
}

void clean_up(Dot &thisDot, Uint32 &bg)
{
   SDL_FreeSurface(dot);;
   std::ofstream save("game_save");

   save << thisDot.get_x();
   save << " ";
   save << thisDot.get_y();
   save << "\n";
   Uint8 r, g, b;
   SDL_GetRGB(bg, screen->format, &r, &g, &b);

   if ((r == 0xFF) && (g == 0xFF) && (b == 0xFF))
   {
      save << "White Level";
   }
   else if (r == 0xFF)
   {
      save << "Red Level";
   }
   else if (g == 0xFF)
   {
      save << "Green Level";
   }
   else if (b == 0xFF)
   {
      save << "Blue Level";
   }

   save.close();
   SDL_Quit();
}

int main(int argc, char* args[])
{
   bool quit = false;

   if (init() == false)
   {
      return 1;
   }
   
   Dot myDot;
   Uint32 background = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
   Timer fps;

   if (load_files(myDot, background) == false)
   {
      return 1;
   }
     
   while(quit == false)
   {
      fps.start();
      while (SDL_PollEvent(&event))
      {
         myDot.handle_input();      

         if (event.type == SDL_KEYDOWN)
         {
            switch(event.key.keysym.sym)
            {
               case SDLK_1: background = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF); break;
               case SDLK_2: background = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00); break;
               case SDLK_3: background = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00); break;
               case SDLK_4: background = SDL_MapRGB(screen->format, 0x00, 0x00, 0xFF); break;
            }
         }

         if (event.type == SDL_QUIT)
         {
            quit = true;
         }
      }

      myDot.move();

      SDL_FillRect(screen, &screen->clip_rect, background);

      myDot.show();

      if (SDL_Flip(screen) == -1)
      {
         return 1;
      }

      if (fps.get_ticks() < 1000 / FRAMES_PER_SECOND)
      {
         SDL_Delay((1000/FRAMES_PER_SECOND) - fps.get_ticks());
      }
   }
   clean_up(myDot, background);
   return 0;
}
