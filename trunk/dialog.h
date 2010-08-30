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
	eRGBA*	Buffer() const { return buffer.Data(); }
	eDialog* Focus() const { return focus; }
	void	Focus(eDialog* f) { focus = f; }
	bool	Command(char cmd) { if(!focus) return false; return focus->Command(cmd); }
protected:
	eDialog* focus;
	eBufferRGBA	buffer;
};

#endif//__DIALOG_H__
