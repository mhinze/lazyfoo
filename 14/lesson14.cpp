#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 20;

SDL_Surface *screen = NULL;
SDL_Surface *background = NULL;
SDL_Surface *message = NULL;
TTF_Font *font = NULL;
SDL_Event event;
SDL_Color textColor = {255,255,255};

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

   SDL_WM_SetCaption("advanced timing", NULL);

   return true;
}

bool load_files()
{
   background = load_image("background.png");
   if (background == NULL)
   {
      return 1;
   }

   font = TTF_OpenFont("lazy.ttf", 28);
   if (font == NULL)
   {
      return 1;
   }

   return true;
}

void clean_up()
{
   SDL_FreeSurface(background);

   TTF_CloseFont(font);
   
   TTF_Quit();
   SDL_Quit();
}

class Timer 
{
   private:
      Uint8 start_ticks;
   public:
     Uint8 get_ticks();
     void start();
};

void Timer::start() 
{
   start_ticks = SDL_GetTicks();
}

Uint8 Timer::get_ticks()
{
   return (SDL_GetTicks() - start_ticks);
}


int main(int argc, char* args[])
{
   int frame = 0;
   bool cap = true;
   Timer fps;

   bool quit = false;

   if (init() == false)
   {
      return 1;
   }
   
   if (load_files() == false)
   {
      return 1;
   }

   message = TTF_RenderText_Solid(font, "Testing Frame Rate", textColor);

   while(quit == false)
   {
      fps.start();
      while (SDL_PollEvent(&event))
      {
         if (event.type == SDL_KEYDOWN)
         {
            if (event.key.keysym.sym == SDLK_RETURN)
            {
               cap = (!cap);
            }
         }
         else if (event.type == SDL_QUIT)
         {
            quit = true;
         }
      }

      apply_surface(0, 0, background, screen);
      apply_surface((SCREEN_WIDTH - message->w)/2, ((SCREEN_HEIGHT + message->h * 2)/FRAMES_PER_SECOND) * (frame % FRAMES_PER_SECOND) - message->h, message, screen);

      if (SDL_Flip(screen) == -1)
      {
         return 1;
      }
      frame++;
      if ((cap == true) && (fps.get_ticks() < 1000 / FRAMES_PER_SECOND))
      {
         SDL_Delay((1000/FRAMES_PER_SECOND) - fps.get_ticks());
      }
   }
   clean_up();

   return 0;
}
