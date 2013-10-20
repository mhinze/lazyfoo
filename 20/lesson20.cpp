#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <iostream>

using std::cout;

const int FRAMES_PER_SECOND = 10;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
const int FOO_WIDTH = 64;
const int FOO_HEIGHT = 205;
const int FOO_RIGHT = 0;
const int FOO_LEFT = 1;

SDL_Surface *foo = NULL;
SDL_Surface *screen = NULL;

SDL_Rect clipsRight[4];
SDL_Rect clipsLeft[4];

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

class Foo 
{
   private:
      int offSet;
      int velocity;
      int frame;
      int status;
   public:
      Foo();
      void handle_events();
      void move();
      void show();
};

void set_clips()
{
   clipsRight[0].x = 0;
   clipsRight[0].y = 0;
   clipsRight[0].w = FOO_WIDTH;
   clipsRight[0].h = FOO_HEIGHT;
   
   clipsRight[1].x = FOO_WIDTH;
   clipsRight[1].y = 0;
   clipsRight[1].w = FOO_WIDTH;
   clipsRight[1].h = FOO_HEIGHT;

   clipsRight[2].x = FOO_WIDTH * 2;
   clipsRight[2].y = 0;
   clipsRight[2].w = FOO_WIDTH;
   clipsRight[2].h = FOO_HEIGHT;

   clipsRight[3].x = FOO_WIDTH * 3;
   clipsRight[3].y = 0;
   clipsRight[3].w = FOO_WIDTH;
   clipsRight[3].h = FOO_HEIGHT;

   clipsLeft[0].x = 0;
   clipsLeft[0].y = FOO_HEIGHT;
   clipsLeft[0].w = FOO_WIDTH;
   clipsLeft[0].h = FOO_HEIGHT;

   clipsLeft[1].x = FOO_WIDTH;
   clipsLeft[1].y = FOO_HEIGHT;
   clipsLeft[1].w = FOO_WIDTH;
   clipsLeft[1].h = FOO_HEIGHT;

   clipsLeft[2].x = FOO_WIDTH * 2;
   clipsLeft[2].y = FOO_HEIGHT;
   clipsLeft[2].w = FOO_WIDTH;
   clipsLeft[2].h = FOO_HEIGHT;

   clipsLeft[3].x = FOO_WIDTH * 3;
   clipsLeft[3].y = FOO_HEIGHT;
   clipsLeft[3].w = FOO_WIDTH;
   clipsLeft[3].h = FOO_HEIGHT;
}

Foo::Foo()
{
   offSet = 0;
   velocity = 0;
   frame = 0;
   status = FOO_RIGHT;
}

void Foo::move()
{
   offSet += velocity;
   if ((offSet < 0) || (offSet + FOO_WIDTH > SCREEN_WIDTH))
   {
      offSet -= velocity;
   }
}

void Foo::show()
{
   if (velocity < 0) // moving left
   {
      status = FOO_LEFT;
      frame++;
   }
   else if (velocity > 0) // moving right
   {
      status = FOO_RIGHT;
      frame++;
   }
   else // standing
   {
      frame = 0;
   }

   if (frame >= 4)
   {
      frame = 0;
   }
   if (status == FOO_RIGHT)
   {
      apply_surface(offSet, SCREEN_HEIGHT - FOO_HEIGHT, foo, screen, &clipsRight[frame]);
   }
   else if (status == FOO_LEFT)
   {
      apply_surface(offSet, SCREEN_HEIGHT - FOO_HEIGHT, foo, screen, &clipsLeft[frame]);
   }
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

void Foo::handle_events()
{
      int v = FOO_WIDTH / 4;
   if (event.type == SDL_KEYDOWN)
   {
      
      switch(event.key.keysym.sym)
      {
         case SDLK_LEFT: 
           velocity -= v;
           break;
         case SDLK_RIGHT:
           velocity += v;
           break;
         default:
           break;
      }
   }
   if (event.type == SDL_KEYUP)
   {
      switch(event.key.keysym.sym)
      {
         case SDLK_LEFT: 
           velocity += v;
           break;
         case SDLK_RIGHT:
           velocity -= v;
           break;
         default:
           break;
      }
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
   foo = load_image("foo.png");
   if (foo == NULL)
   {
      return 1;
   }

   return true;
}

void clean_up()
{
   SDL_FreeSurface(foo);;
   
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
    
   set_clips();
   Timer fps;
   Foo walk;

   while(quit == false)
   {
      fps.start();
      while (SDL_PollEvent(&event))
      {
         walk.handle_events();

         if (event.type == SDL_QUIT)
         {
            quit = true;
         }
      }

      walk.move();

      SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));

      walk.show();

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
