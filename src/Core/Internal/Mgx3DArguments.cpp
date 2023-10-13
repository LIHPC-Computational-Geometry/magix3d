/**
 * Accès aux arguments de ligne de commande traités par Magix3D et ses applications dérivées.
 */

#include "Internal/Mgx3DArguments.h"


using namespace std;


namespace Mgx3D
{

namespace Internal
{

vector<string> mgx3dAllowedArgs ( )
{
	vector<string>	args;

	// Arguments GSCC :
	args.push_back ("--std");
	args.push_back ("--new");
	args.push_back ("--old");
	args.push_back ("--version");
	args.push_back ("-debuger");
	args.push_back ("-variant");

	// Arguments configurations Magix3D :
	args.push_back ("-defaultConfig");
	args.push_back ("-userConfig");
	args.push_back ("-userConfigPatchs");
	args.push_back ("-outCharset");
	args.push_back ("--outCharset");
	args.push_back ("-outCharsetRef");
	args.push_back ("--outCharsetRef");
	args.push_back ("-desiredFrameRate");
	args.push_back ("--desiredFrameRate");
	args.push_back ("-stillFrameRate");
	args.push_back ("--stillFrameRate");
	args.push_back ("-graphicalWindowHeight");
	args.push_back ("--graphicalWindowHeight");
	args.push_back ("-graphicalWindowWidth");
	args.push_back ("--graphicalWindowWidth");
	args.push_back ("--debug");

	// Arguments IHM Magix3D :
	args.push_back ("-help");
	args.push_back ("--help");
	args.push_back ("-helpURL");
	args.push_back ("-wikiURL");
	args.push_back ("-qualifURL");
	args.push_back ("-userManual");
	args.push_back ("-docViewer");
	args.push_back ("-script");
	args.push_back ("--script");

	// Arguments noyau Magix3D :
	args.push_back ("-useOCAF");

	// Arguments Magix3D batch :
	args.push_back ("-coordTranslate");
	args.push_back ("--coordTranslate");

	return args;
}	// mgx3dAllowedArgs

}	// namespace Internal

}	// namespace Mgx3D


