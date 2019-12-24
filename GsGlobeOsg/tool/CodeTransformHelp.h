#ifndef _CODEHELP_H
#define _CODEHELP_H
 
#include <string>

using namespace std;
#include <utility.h>
class CodeHelp
{
public:

	static string  String_To_UTF8 (const std::string & str)
	{
        return GeoStar::Utility::GsEncoding::ToUtf8(str.c_str()).c_str();
	}

	static string UTF8_To_string(const std::string & str) 
	{ 
        return GeoStar::Utility::GsEncoding::ToLocal(str.c_str()).c_str();
	} 
};

#endif
