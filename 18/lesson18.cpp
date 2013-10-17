#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

using std::cout;

const int FRAMES_PER_SECOND = 60;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
const int SQUARE_HEIGHT = 20; 
const int SQUARE_WIDTH = 20; 
const int DOT_HEIGHT = 20;
const int DOT_WIDTH = 20;

SDL_Surface *screen = NULL;
SDL_Surface *square = NULL;
SDL_Surface *dot = NULL;

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

class Dot
{
   private:
      int x, y;
      std::vector<SDL_Rect> box;
      int xVel, yVel;
      void shift_boxes();
   public:
      Dot(int X, int Y);
      void handle_input();
      void move(std::vector<SDL_Rect> &rects);
      void show();
      std::vector<SDL_Rect> &get_rects();
};

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

bool check_collision(std::vector<SDL_Rect> &A, std::vector<SDL_Rect> &B)
{
   int leftA, leftB;
   int rightA, rightB;
   int topA, topB;
   int bottomA, bottomB;

   for (int Abox = 0; Abox < A.size(); Abox++)
   {
      leftA = A[Abox].x;
      rightA = A[Abox].x + A[Abox].w;
      topA = A[Abox].y;
      bottomA = A[Abox].y;
       
      for(int Bbox = 0; Bbox < B.size(); Bbox++)
      {
         leftB = B[Bbox].x;
         rightB = B[Bbox].x + B[Bbox].w;
         topB = B[Bbox].y;
         bottomB = B[Bbox].y + B[Bbox].h;

         if (((bottomA <= topB) || (topA >= bottomB) || (rightA <= leftB) || (leftA >= rightB)) == false)
         {
            return true;
         }
      }
   }

   return false;
}

Dot::Dot(int X, int Y)
{
   x = X;
   y = Y;

   xVel = 0;
   yVel = 0;

   box.resize(11);

   box[0].w = 6;
   box[0].h = 1;

   box[1].w = 10;
   box[1].h = 1;

   box[2].w = 14;
   box[2].h = 1;

   box[3].w = 16;
   box[3].h = 2;

   box[4].w = 18;
   box[4].h = 2;

   box[5].w = 20;
   box[5].h = 6;

   box[6].w = 18;
   box[6].h = 2;

   box[7].w = 16;
   box[7].h = 2;

   box[8].w = 14;
   box[8].h = 1;

   box[9].w = 10;
   box[9].h = 1;

   box[10].w = 6;
   box[10].h = 1;

   shift_boxes();
}

void Dot::shift_boxes()
{
   // row offset
   int r = 0;
   // go through dot's collision boxes;
   for(int set = 0; set < box.size(); set++)
   {
      // center the box
      box[set].x = x + (DOT_WIDTH - box[set].w) / 2;
      // set collision box at its row offset
      box[set].y = y + r;
      // move the row offset down the height of the collision box
      r += box[set].h;
   }
}

void Dot::handle_input() 
{
   if (event.type == SDL_KEYDOWN)
   {
      switch(event.key.keysym.sym)
      {
         case SDLK_UP:
            yVel -= 1;
            break;
         case SDLK_DOWN:
            yVel += 1;
            break;
         case SDLK_LEFT:
            xVel -= 1;
            break;
         case SDLK_RIGHT:
            xVel += 1;
            break;
         default: break;
      }
   }
   else if (event.type == SDL_KEYUP)
   {
      switch(event.key.keysym.sym)
      {
         case SDLK_UP:
            yVel += 1;
            break;
         case SDLK_DOWN:
            yVel -= 1;
            break;
         case SDLK_LEFT:
            xVel += 1;
            break;
         case SDLK_RIGHT:
            xVel -= 1;
            break;
         default: 
            break;
      }
   }
}

void Dot::move(std::vector<SDL_Rect> &rects)
{
   x += xVel;
   shift_boxes();

   if ((x < 0) || (x + DOT_WIDTH > SCREEN_WIDTH) || (check_collision(box, rects)))
   {
      x -= xVel;
      shift_boxes();
   }

   y += yVel;

   shift_boxes();

   if ((y < 0) || (y + DOT_HEIGHT > SCREEN_HEIGHT) | (check_collision(box, rects)))
   {
      y -= yVel;
      shift_boxes();
   }
}

void Dot::show()
{
   apply_surface(x, y, dot, screen);
}

std::vector<SDL_Rect> &Dot::get_rects()
{
   return box;
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
   dot = load_image("dot.bmp");
   if (dot == NULL)
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

   Dot myDot(0, 0), otherDot(20, 20);

   while(quit == false)
   {
      fps.start();
      while (SDL_PollEvent(&event))
      {
         myDot.handle_input();

         if (event.type == SDL_QUIT)
         {
            quit = true;
         }
      }

      myDot.move(otherDot.get_rects());

      SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));

      otherDot.show();
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
   clean_up();
   return 0;
}
