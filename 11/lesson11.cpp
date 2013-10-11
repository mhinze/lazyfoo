#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *screen = NULL;
SDL_Surface *background = NULL;
TTF_Font *font = NULL;
SDL_Event event;
SDL_Color textColor = {0, 0, 0};

Mix_Music *music = NULL;
Mix_Chunk *scratch = NULL;
Mix_Chunk *high = NULL;
Mix_Chunk *med = NULL;
Mix_Chunk *low = NULL;

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

   SDL_WM_SetCaption("music", NULL);

   if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
   {
      return false;
   }

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

   music = Mix_LoadMUS("beat.wav");
   if (music == NULL)
   {
      return false;
   }

   scratch = Mix_LoadWAV("scratch.wav");
   high = Mix_LoadWAV("high.wav");
   med = Mix_LoadWAV("medium.wav");
   low = Mix_LoadWAV("low.wav");

   if ((scratch == NULL) || (high == NULL) || (med == NULL) || (low == NULL))
   {
      return false;
   }

   return true;
}

void clean_up()
{
   SDL_FreeSurface(background);

   Mix_FreeChunk(scratch);
   Mix_FreeChunk(high);
   Mix_FreeChunk(med);
   Mix_FreeChunk(low);
   Mix_FreeMusic(music);
   TTF_CloseFont(font);
   Mix_CloseAudio();
   
   TTF_Quit();
   SDL_Quit();
}

void play_sound(Mix_Chunk *sound)
{
   Mix_PlayChannel(-1, sound, 0);
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

   apply_surface(0, 0, background, screen);
     
   if (SDL_Flip(screen) == -1)
   {
      return 1;
   }
   while(quit == false)
   {
      while (SDL_PollEvent(&event))
      {
         if (event.type == SDL_KEYDOWN)
         {
            SDLKey k = event.key.keysym.sym;

            if (k == SDLK_1)
            {
               play_sound(scratch);
            }
            else if (k == SDLK_2)
            {
               play_sound(high);
            }
            else if (k == SDLK_3)
            {
               play_sound(med);
            }
            else if(k == SDLK_4)
            {
               play_sound(low);
            }
            else if (k == SDLK_9)
            {
               if (Mix_PlayingMusic() == 0)
               {
                  Mix_PlayMusic(music, -1);
               }
               else
               {
                  if (Mix_PausedMusic() == 1)
                  {
                     Mix_ResumeMusic();
                  }
                  else
                  {
                     Mix_PauseMusic();
                  }
               }
            }
            else if (k == SDLK_0)
            {
               Mix_HaltMusic();
            }
         }

         if (event.type == SDL_QUIT)
         {
            quit = true;
         }
      }


   }
   clean_up();

   return 0;
}
