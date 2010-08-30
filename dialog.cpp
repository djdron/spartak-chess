#include "dialog.h"
#include <algorithm>

eFont* eDialog::font = NULL;

void eDialog::Destroy()
{
	for(eDialogs::iterator i = childs.begin(); i != childs.end(); ++i)
	{
		(*i)->Destroy();
		delete *i;
	}
	childs.clear();
}

void eDialog::Paint(eBufferRGBA& buf)
{
	buf.Clear(bound);
	if(!text.empty())
		font->DrawText(buf, bound.beg, text.c_str());
	for(eDialogs::iterator i = childs.begin(); i != childs.end(); ++i)
	{
		(*i)->Paint(buf);
	}
}
void eDialog::Remove(eDialog* child)
{
	eDialogs::iterator i = std::find(childs.begin(), childs.end(), child);
	if(i != childs.end())
		childs.erase(i);
}

eDesktop::eDesktop() : focus(NULL), buffer(ePoint2(320, 240))
{
	font = new eFont;
}
eDesktop::~eDesktop()
{
	delete font;
}
void eDesktop::Update()
{
	Paint(buffer);
}
