#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

namespace xIo
{

static string resource_path;
void ResourcePath(const char* _path)
{
	string path = _path;
	string::size_type p = path.rfind('/');
	if(p == string::npos)
		p = path.rfind('\\');
	if(p != string::npos)
		resource_path = path.substr(0, p + 1);
}
static string resource;
const char* Resource(const char* _r)
{
	resource = resource_path + _r;
	return resource.c_str();
}

}
//namespace xIo

namespace xLog
{

static FILE* log_file = NULL;

void Close()
{
	if(log_file)
	{
		fclose(log_file);
		log_file = NULL;
	}
}

bool Open()
{
	FILE* f = fopen(xIo::Resource("chess.log"), "w");
	if(!f)
		return false;
	log_file = f;
	atexit(Close);
	return true;
}

void LOG(const string& str)
{
	if(xLog::log_file)
		fputs(str.c_str(), xLog::log_file);
}

void LOG(const char* str)
{
	if(xLog::log_file)
		fputs(str, xLog::log_file);
}

}
//namespace xLog
