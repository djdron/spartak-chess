/*
Spartak Chess based on stockfish engine.
Copyright (C) 2010 djdron

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef USE_SDL

#include <SDL.h>
#include "../game.h"
#include "../ui/dialog.h"

static bool sdl_inited = false;
static SDL_Surface* screen = NULL;
static SDL_Surface* offscreen = NULL;

static bool Init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        return false;
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("Spartak Chess (Stockfish)", NULL);
    sdl_inited = true;
    screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
    if(!screen)
        return false;
	offscreen = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 16,
						screen->format->Rmask,
						screen->format->Gmask,
						screen->format->Bmask,
						screen->format->Amask);
	if(!offscreen)
		return false;
    return true;
}
static void Done()
{
	if(offscreen)
		SDL_FreeSurface(offscreen);
	if(screen)
		SDL_FreeSurface(screen);
	if(sdl_inited)
		SDL_Quit();
}
inline word BGR565(byte r, byte g, byte b) { return (((r&~7) << 8)|((g&~3) << 3)|(b >> 3)); }
static void UpdateScreen(eGame* game)
{
	SDL_LockSurface(offscreen);
	eRGBA* data = game->Desktop().Buffer();
	word* scr = (word*)offscreen->pixels;
	for(int y = 0; y < 240; ++y)
	{
		for(int x = 0; x < 320; ++x)
		{
			eRGBA c(*data++);
			*scr++ = BGR565(c.r, c.g, c.b);
		}
	}
	SDL_UnlockSurface(offscreen);
	SDL_BlitSurface(offscreen, NULL, screen, NULL);
	SDL_Flip(screen);
}

int main(int argc, char* argv[])
{
    if(!Init())
    {
		Done();
        return 1;
    }
	eGame* game = new eGame;
    bool quit = false;
    while(!quit)
    {
    	SDL_Event e;
    	while(SDL_PollEvent(&e))
    	{
			if(e.type == SDL_QUIT)
				quit = true;
			else if(e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym)
				{
				case SDLK_LEFT:		game->Command('l');		break;
				case SDLK_RIGHT:	game->Command('r');		break;
				case SDLK_UP:		game->Command('u');		break;
				case SDLK_DOWN:		game->Command('d');		break;
				case SDLK_LCTRL:	game->Command('a');		break;
				case SDLK_LALT:		game->Command('b');		break;
				case SDLK_TAB:		game->Command('n');		break;
				case SDLK_BACKSPACE:game->Command('g');		break;
				case SDLK_ESCAPE:	game->Command('f');		break;
				default: break;
				}
			}
    	}
    	game->Update();
    	UpdateScreen(game);
    }
    Done();
    return 0;
}

#endif//USE_SDL
