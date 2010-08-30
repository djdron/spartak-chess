#ifndef __IO_H__
#define __IO_H__

#pragma once

namespace xIo
{
void ResourcePath(const char* _path);
const char* Resource(const char* _r);
}
//namespace xIo

namespace xLog
{
bool Open();
void LOG(const char* str);
}
//namespace xLog

using xLog::LOG;

#endif//__IO_H__
