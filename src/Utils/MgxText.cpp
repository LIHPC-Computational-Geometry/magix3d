/**
 * \file		MgxText.cpp
 * \author		Charles PIGNEROL
 * \date		12/01/2014
 */

#include "Utils/MgxText.h"
#include "Utils/Common.h"

#include <TkUtil/Exception.h>

#include <sstream>


using namespace TkUtil;
using namespace std;


namespace Mgx3D
{

namespace Utils
{

MgxText::MgxText ( )
{
	MGX_FORBIDDEN ("MgxText constructor is not allowed.")
}   // MgxText::MgxText


MgxText::MgxText (const MgxText&)
{
	MGX_FORBIDDEN ("MgxText copy constructor is not allowed.")
}   // MgxText::MgxText


MgxText& MgxText::operator = (const MgxText&)
{
	MGX_FORBIDDEN ("MgxText assignment operator is not allowed.")
	return *this;
}   // MgxText::operator =


MgxText::~MgxText ( )
{
	MGX_FORBIDDEN ("MgxText destructor is not allowed.")
}   // MgxText::~MgxText


string MgxText::stringToMultilineString (const string& str, char end)
{
	ostringstream	stream;

	for (string::const_iterator it = str.begin ( ); str.end ( ) != it; it++)
	{
		stream << *it;
		if (*it == end)
			stream << "\n";
	}	// for (string::const_iterator it = str.begin ( ); ...

	return stream.str ( );
}	// MgxText::stringToMultilineString


}	// namespace Utils

}	// namespace Mgx3D
