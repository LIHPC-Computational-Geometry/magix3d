/*----------------------------------------------------------------------------*/
/*
 * \file Landmark.cpp
 *
 *  \author Charles Pignerol
 *
 *  \date 2 sept. 2016
 */
/*----------------------------------------------------------------------------*/
#include "Utils/Landmark.h"
#include "Utils/Common.h"
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
std::string Landmark::toString(const Landmark::kind& k)
{
	if (k == maillage)
		return "maillage";
	else if (k == chambre)
		return "chambre";
	else if (k == undefined)
		return "indéfini";

	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, toString avec Landmark::kind non prévu", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::string Landmark::getScriptCommand(const Landmark::kind& k)
{
	TkUtil::UTF8String o (TkUtil::Charset::UTF_8);

	if (k == undefined)
		throw TkUtil::Exception(TkUtil::UTF8String ("pas de ligne de commande pour le cas indéfini", TkUtil::Charset::UTF_8));
	else if (k == maillage)
		o << getMgx3DAlias() << ".Landmark.maillage";
	else if (k == chambre)
		o << getMgx3DAlias() << ".Landmark.chambre";
	else
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, getScriptCommand avec Landmark::kind non prévu",  TkUtil::Charset::UTF_8));

	return o.iso ( );
}
/*----------------------------------------------------------------------------*/
Landmark::kind Landmark::toLandmark(const std::string& name)
{
	if (name == "maillage")
		return maillage;
	else if (name == "chambre")
		return chambre;
	else if (name == "indéfini" || name == "undefined")
		return undefined;
	else
		throw TkUtil::Exception(TkUtil::UTF8String ("Landmark de type non reconnu", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

