#include "picture.h"

ePicture::ePicture(const char* name, const ePoint2& size) : buffer(size)
{
	ReadPNG("res/splash.png", buffer.Data(), size.x, size.y, 4);
}

void ePicture::Paint(eBufferRGBA& buf)
{
	for(int j = 0; j < buffer.Size().y; ++j)
		for(int i = 0; i < buffer.Size().x; ++i)
			buf[bound.beg + ePoint2(i, j)] = buffer[ePoint2(i, j)];
}
