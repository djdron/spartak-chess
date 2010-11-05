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
#ifdef SDL_KEYS_GP2X

#include <SDL.h>
#include "../game.h"

void ProcessEvent(eGame* game, const SDL_Event& e)
{
	switch(e.type)
	{
    case SDL_JOYBUTTONDOWN:
		switch(e.jbutton.button)
		{
		case GP2X_BUTTON_LEFT:         game->Command('l');             break;
		case GP2X_BUTTON_RIGHT:        game->Command('r');             break;
		case GP2X_BUTTON_UP:           game->Command('u');             break;
		case GP2X_BUTTON_DOWN:         game->Command('d');             break;
		case GP2X_BUTTON_B:            game->Command('a');             break;
		case GP2X_BUTTON_X:            game->Command('b');             break;
		case GP2X_BUTTON_L:            game->Command('n');             break;
		case GP2X_BUTTON_R:            game->Command('g');             break;
		case GP2X_BUTTON_START:        game->Command('f');             break;
		default: break;
		}
		break;
	default:
		break;
	}
}

#endif//SDL_KEYS_GP2X
#endif//USE_SDL
