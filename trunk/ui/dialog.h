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

#ifndef __DIALOG_H__
#define __DIALOG_H__

#include "ui.h"
#include <string>
#include <vector>

#pragma once

class eDialog;
typedef std::vector<eDialog*> eDialogs;

class eDialog
{
public:
	virtual ~eDialog() {}
	virtual void Create() {}
	virtual void Destroy();
	virtual void Paint(eBufferRGBA& buf);
	virtual bool Command(char cmd) { return false; }
	void Insert(eDialog* child) { childs.push_back(child); }
	void Remove(eDialog* child);

	eRect bound;
	std::string text;
	eDialogs childs;

	static eFont* font;
};

class eDesktop : public eDialog
{
public:
	eDesktop();
	virtual ~eDesktop();
	void 	Update();
	void	Clear() { buffer.Clear(); }
	eRGBA*	Buffer() const { return buffer.Data(); }
	eDialog* Focus() const { return focus; }
	void	Focus(eDialog* f) { focus = f; }
	bool	Command(char cmd) { if(!focus) return false; return focus->Command(cmd); }

protected:
	eDialog* focus;
	eBufferRGBA	buffer;
};

#endif//__DIALOG_H__
