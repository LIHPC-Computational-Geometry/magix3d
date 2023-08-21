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
	args.push_back ("-userManual");
	args.push_back ("-userManualViewer");
	args.push_back ("-pythonAPIURL");
	args.push_back ("-qualifURL");
	args.push_back ("-script");
	args.push_back ("--script");

	// Arguments noyau Magix3D :
	args.push_back ("-useOCAF");

	// Arguments Magix3D batch :
	args.push_back ("-coordTranslate");
	args.push_back ("--coordTranslate");

	// Arguments parallélisme Magix3D :
	args.push_back ("-nbprocs");
	args.push_back ("--nbprocs");

	// Arguments client/serveur Magix3D :
	args.push_back ("-host");
	args.push_back ("--host");
	args.push_back ("-port");
	args.push_back ("--port");
	args.push_back ("-renderingPort");
	args.push_back ("--renderingPort");
	args.push_back ("-displays");
	args.push_back ("--displays");
	args.push_back ("-onScreenWindow");
	args.push_back ("--onScreenWindow");

	return args;
}	// mgx3dAllowedArgs

}	// namespace Internal

}	// namespace Mgx3D


