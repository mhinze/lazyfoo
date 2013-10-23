#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>
#include <iostream>

const int FRAMES_PER_SECOND = 40;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
TTF_Font *font = NULL;
SDL_Surface *message = NULL;
SDL_Color textColor = {255, 255, 255};


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

class StringInput
{
   private:
      std::string str;
      SDL_Surface *text;
   public:
      StringInput();
      ~StringInput();
      void handle_input();
      void show_centered();
};

StringInput::StringInput()
{
   str = "";
   text = NULL;
   SDL_EnableUNICODE(SDL_ENABLE);
}

StringInput::~StringInput()
{
   SDL_FreeSurface(text);
   SDL_EnableUNICODE(SDL_DISABLE);
}

void StringInput::handle_input()
{
   if (event.type == SDL_KEYDOWN)
   {
      std::string temp = str;
      if (str.length() <= 16)
      {
         Uint16 unichar = event.key.keysym.unicode;

         if (unichar == (Uint16)' ')
         {
            str += (char)unichar;
         }
         else if ((unichar >= (Uint16)'0') && (unichar <= (Uint16)'9'))
         {
            str += (char)unichar;
         }
         else if ((unichar >= (Uint16)'A') && (unichar <= (Uint16)'Z'))
         {
            str += (char)unichar;
         }
         else if ((unichar >= (Uint16)'a') && (unichar <= (Uint16)'z'))
         {
            str += (char)unichar;
         }
         if ((event.key.keysym.sym == SDLK_BACKSPACE) && (str.length() != 0))
         {
            str.erase(str.length() - 1);
         }
         if (str != temp)
         {
            SDL_FreeSurface(text);
            text = TTF_RenderText_Solid(font, str.c_str(), textColor);
         }
      }
   }
}

void StringInput::show_centered() 
{
   if (text != NULL)
   {
      apply_surface((SCREEN_WIDTH - text->w) / 2, (SCREEN_HEIGHT - text->h) / 2, text, screen);
   }
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

   SDL_WM_SetCaption("text input", NULL);

   return true;
}

bool load_files()
{
   background = load_image("background.png");
   if (background == NULL)
   {
      return false;
   }

   font = TTF_OpenFont("lazy.ttf", 28);
   if (font == NULL)
   {
      return false;
   }

   return true;
}

void clean_up()
{
   SDL_FreeSurface(background);
   
   SDL_Quit();
}

int main(int argc, char* args[])
{
   bool quit = false;

   bool nameEntered = false;


   if (init() == false)
   {
      return 1;
   }
   
   StringInput name;

   if (load_files() == false)
   {
      return 1;
   }

   message = TTF_RenderText_Solid(font, "New High Score! Enter Name:", textColor);


   while(quit == false)
   {

      while (SDL_PollEvent(&event))
      {
         if (nameEntered == false)
         {
            name.handle_input();

            if ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_RETURN))
            {
               nameEntered = true;
               SDL_FreeSurface(message);
               message = TTF_RenderText_Solid(font, "Rank: 1st", textColor);
            }
         }
         
         if (event.type == SDL_QUIT)
         {
            quit = true;
         }
      }

      apply_surface(0, 0, background, screen);
      apply_surface((SCREEN_WIDTH - message->w)/2,((SCREEN_HEIGHT/2) - message->h)/2, message, screen);
      name.show_centered();

      if (SDL_Flip(screen) == -1)
      {
         return 1;
      }

   }
   clean_up();
   return 0;
}
