#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>

using std::cout;

const int FRAMES_PER_SECOND = 60;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
const int SQUARE_HEIGHT = 20; 
const int SQUARE_WIDTH = 20; 

SDL_Surface *screen = NULL;
SDL_Surface *square = NULL;
SDL_Rect wall;
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

class Square
{
   private:
      SDL_Rect box;
      int xVel, yVel;
   public:
      Square();
      void handle_input();
      void move();
      void show();
};

bool check_collision(SDL_Rect A, SDL_Rect B)
{
   int leftA, leftB;
   int rightA, rightB;
   int topA, topB;
   int bottomA, bottomB;

   // calc the sides of rect A
   leftA = A.x;
   rightA = A.x + A.w;
   topA = A.y;
   bottomA = A.y + A.h;

   // calc the sides of rect B
   leftB = B.x;
   rightB = B.x + B.w;
   topB = B.y;
   bottomB = B.y + B.h;

   // if any of the sides from A are outside B
   if (bottomA <= topB)
   {
      return false;
   }

   if (topA >= bottomB)
   {
      return false;
   }

   if (rightA <= leftB)
   {
      return false;
   }

   if (leftA >= rightB)
   {
      return false;
   }

   return true;
}

Square::Square()
{
   box.x = 0;
   box.y = 0;
   box.w = SQUARE_WIDTH;
   box.h = SQUARE_HEIGHT;
   xVel = 0;
   yVel = 0;
}

void Square::move()
{
   box.x += xVel;
   // if the square went too far to the left or right or collided with the wall
   if ((box.x < 0) || (box.x + SQUARE_WIDTH > SCREEN_WIDTH) || (check_collision(box, wall)))
   {
      // move back
      box.x -= xVel;
   }

   box.y += yVel;

   // if the square went too far up or down or collided with wall
   if ((box.y < 0) || (box.y + SQUARE_HEIGHT > SCREEN_HEIGHT) || (check_collision(box, wall)))
   {
      // move back
      box.y -= yVel;
   }
}

void Square::handle_input()
{
   if (event.type == SDL_KEYDOWN)
   {
      switch(event.key.keysym.sym)
      {
         case SDLK_UP:
           yVel -= SQUARE_HEIGHT / 2; 
           break;
         case SDLK_DOWN:
           yVel += SQUARE_HEIGHT / 2;
           break;
         case SDLK_LEFT: 
           xVel -= SQUARE_WIDTH / 2;
           break;
         case SDLK_RIGHT:
           xVel += SQUARE_WIDTH / 2;
           break;
         default:
           break;
      }
   }
   if (event.type == SDL_KEYUP)
   {
      switch(event.key.keysym.sym)
      {
         case SDLK_UP:
           yVel += SQUARE_HEIGHT / 2; 
           break;
         case SDLK_DOWN:
           yVel -= SQUARE_HEIGHT / 2;
           break;
         case SDLK_LEFT: 
           xVel += SQUARE_WIDTH / 2;
           break;
         case SDLK_RIGHT:
           xVel -= SQUARE_WIDTH / 2;
           break;
         default:
           break;
      }
   }
}

void Square::show()
{
   apply_surface(box.x, box.y, square, screen);
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

   SDL_WM_SetCaption("Square collision", NULL);

   return true;
}

bool load_files()
{
   square = load_image("square.bmp");
   if (square == NULL)
   {
      return 1;
   }

   return true;
}

void clean_up()
{
   SDL_FreeSurface(square);;
   
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
     
   Timer fps;
   Square mySquare;

   wall.x = 300;
   wall.y = 40;
   wall.w = 40;
   wall.h = 400;
   
   while(quit == false)
   {
      fps.start();
      while (SDL_PollEvent(&event))
      {
         mySquare.handle_input();      

         if (event.type == SDL_QUIT)
         {
            quit = true;
         }
      }

      mySquare.move();

      SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
      SDL_FillRect(screen, &wall, SDL_MapRGB(screen->format, 0x77, 0x77, 0x77));

      mySquare.show();

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
