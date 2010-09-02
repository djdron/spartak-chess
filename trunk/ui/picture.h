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

#ifndef __PICTURE_H__
#define __PICTURE_H__

#include "dialog.h"

#pragma once

class ePicture : public eDialog
{
public:
	ePicture(const char* name, const ePoint2& size);
	virtual void Paint(eBufferRGBA& buf);

protected:
	eBufferRGBA buffer;
};

#endif//__PICTURE_H__
