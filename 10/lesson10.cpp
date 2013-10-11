#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *screen = NULL;
SDL_Surface *background = NULL;
TTF_Font *font = NULL;
SDL_Event event;
SDL_Color textColor = {0, 0, 0};
SDL_Surface *up = NULL;
SDL_Surface *down = NULL;
SDL_Surface *left = NULL;
SDL_Surface *right = NULL;

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

   SDL_WM_SetCaption("keystates test", NULL);

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
   SDL_FreeSurface(up);
   SDL_FreeSurface(down);
   SDL_FreeSurface(left);
   SDL_FreeSurface(right);

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

   up = TTF_RenderText_Solid(font, "Up", textColor);
   down = TTF_RenderText_Solid(font, "Down", textColor);
   left = TTF_RenderText_Solid(font, "Left", textColor);
   right = TTF_RenderText_Solid(font, "Right", textColor);
   
   while(quit == false)
   {
      while (SDL_PollEvent(&event))
      {
         if (event.type == SDL_QUIT)
         {
            quit = true;
         }
      }


      apply_surface(0, 0, background, screen);
      Uint8 *keystates = SDL_GetKeyState(NULL);
      
      if (keystates[SDLK_UP])
      {
         apply_surface((SCREEN_WIDTH - up->w) / 2, (SCREEN_HEIGHT / 2 - up->h) / 2, up, screen);
      }
      if (keystates[SDLK_DOWN])
      {
         apply_surface((SCREEN_WIDTH - down->w) / 2, (SCREEN_HEIGHT / 2 - down->h) / 2 + (SCREEN_HEIGHT / 2), down, screen);
      }
      if (keystates[SDLK_LEFT])
      {
         apply_surface((SCREEN_WIDTH / 2 - left->w) / 2, (SCREEN_HEIGHT - left->h) / 2, left, screen);
      }
      if (keystates[SDLK_RIGHT])
      {
         apply_surface((SCREEN_WIDTH /  2 - right->w) / 2 + (SCREEN_WIDTH / 2), (SCREEN_HEIGHT - right->h) / 2, right, screen);
      }
     
      if (SDL_Flip(screen) == -1)
      {
         return 1;
      }
   }
   clean_up();

   return 0;
}
