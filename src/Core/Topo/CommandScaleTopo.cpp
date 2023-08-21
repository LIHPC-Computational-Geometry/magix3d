/*----------------------------------------------------------------------------*/
/*
 * \file CommandScaleTopo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2/10/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandScaleTopo.h"

#include "Utils/Common.h"
#include "Topo/Block.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
// OCC
#include <gp_Trsf.hxx>
#include <gp_GTrsf.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandScaleTopo::
CommandScaleTopo(Internal::Context& c,
		std::vector<TopoEntity*>& ve,
		const double& facteur,
        const Utils::Math::Point& pcentre)
:CommandTransformTopo(c, "Homothétie d'une topologie seule", ve)
, m_factor(facteur)
, m_isHomogene(true)
, m_factorX(0)
, m_factorY(0)
, m_factorZ(0)
, m_center(pcentre)
{
    if (Utils::Math::MgxNumeric::isNearlyZero(m_factor))
        throw TkUtil::Exception (TkUtil::UTF8String ("L'homothétie ne peut se faire qu'avec un facteur non nul", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Homothétie de la topologie seule ";
	for (uint i=0; i<ve.size() && i<5; i++)
		comments << " " << ve[i]->getName();
	if (ve.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandScaleTopo::
CommandScaleTopo(Internal::Context& c,
		CommandDuplicateTopo* cmd,
		const double& facteur,
        const Utils::Math::Point& pcentre,
		bool all_topo)
:CommandTransformTopo(c, "Homothétie d'une copie de la topologie", cmd, all_topo)
, m_factor(facteur)
, m_isHomogene(true)
, m_factorX(0)
, m_factorY(0)
, m_factorZ(0)
, m_center(pcentre)
{
    if (Utils::Math::MgxNumeric::isNearlyZero(m_factor))
        throw TkUtil::Exception (TkUtil::UTF8String ("L'homothétie ne peut se faire qu'avec un facteur non nul", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandScaleTopo::
CommandScaleTopo(Internal::Context& c,
		const double& facteur,
        const Utils::Math::Point& pcentre)
:CommandTransformTopo(c, "Homothétie de tout")
, m_factor(facteur)
, m_isHomogene(true)
, m_factorX(0)
, m_factorY(0)
, m_factorZ(0)
, m_center(pcentre)
{
    if (Utils::Math::MgxNumeric::isNearlyZero(m_factor))
        throw TkUtil::Exception (TkUtil::UTF8String ("L'homothétie ne peut se faire qu'avec un facteur non nul", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Homothétie de toute la topologie";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandScaleTopo::
CommandScaleTopo(Internal::Context& c,
		std::vector<TopoEntity*>& ve,
        const double factorX,
        const double factorY,
        const double factorZ)
:CommandTransformTopo(c, "Homothétie d'une topologie seule", ve)
, m_factor(0)
, m_isHomogene(false)
, m_factorX(factorX)
, m_factorY(factorY)
, m_factorZ(factorZ)
, m_center()
{
	if (Utils::Math::MgxNumeric::isNearlyZero(m_factorX))
		throw TkUtil::Exception (TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul suivant X", TkUtil::Charset::UTF_8));
	if (Utils::Math::MgxNumeric::isNearlyZero(m_factorY))
		throw TkUtil::Exception (TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul suivant Y", TkUtil::Charset::UTF_8));
	if (Utils::Math::MgxNumeric::isNearlyZero(m_factorZ))
		throw TkUtil::Exception (TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul suivant Z", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Homothétie de la topologie seule ";
	for (uint i=0; i<ve.size() && i<5; i++)
		comments << " " << ve[i]->getName();
	if (ve.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandScaleTopo::
CommandScaleTopo(Internal::Context& c,
		CommandDuplicateTopo* cmd,
        const double factorX,
        const double factorY,
        const double factorZ,
		bool all_topo)
:CommandTransformTopo(c, "Homothétie d'une copie de la topologie", cmd, all_topo)
, m_factor(0)
, m_isHomogene(false)
, m_factorX(factorX)
, m_factorY(factorY)
, m_factorZ(factorZ)
, m_center()
{
	if (Utils::Math::MgxNumeric::isNearlyZero(m_factorX))
		throw TkUtil::Exception (TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul suivant X", TkUtil::Charset::UTF_8));
	if (Utils::Math::MgxNumeric::isNearlyZero(m_factorY))
		throw TkUtil::Exception (TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul suivant Y", TkUtil::Charset::UTF_8));
	if (Utils::Math::MgxNumeric::isNearlyZero(m_factorZ))
		throw TkUtil::Exception (TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul suivant Z", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
CommandScaleTopo::
CommandScaleTopo(Internal::Context& c,
        const double factorX,
        const double factorY,
        const double factorZ)
:CommandTransformTopo(c, "Homothétie de tout")
, m_factor(0)
, m_isHomogene(false)
, m_factorX(factorX)
, m_factorY(factorY)
, m_factorZ(factorZ)
, m_center()
{
	if (Utils::Math::MgxNumeric::isNearlyZero(m_factorX))
		throw TkUtil::Exception (TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul suivant X", TkUtil::Charset::UTF_8));
	if (Utils::Math::MgxNumeric::isNearlyZero(m_factorY))
		throw TkUtil::Exception (TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul suivant Y", TkUtil::Charset::UTF_8));
	if (Utils::Math::MgxNumeric::isNearlyZero(m_factorZ))
		throw TkUtil::Exception (TkUtil::UTF8String ("L'homothétie nécessite un facteur non nul suivant Z", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Homothétie de toute la topologie";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandScaleTopo::
~CommandScaleTopo()
{
}
/*----------------------------------------------------------------------------*/
void CommandScaleTopo::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandScaleTopo::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    if (m_isHomogene){
    	// création de l'opérateur d'homothétie via OCC
    	gp_Trsf transf;
    	transf.SetScale(gp_Pnt(m_center.getX(), m_center.getY(), m_center.getZ()), m_factor);

    	// applique la transformation
    	transform(&transf);
    }
    else {
        // création de l'opérateur d'homothétie via OCC
        gp_GTrsf transf;
        transf.SetValue(1,1, m_factorX);
        transf.SetValue(2,2, m_factorY);
        transf.SetValue(3,3, m_factorZ);

        // applique la transformation
        transform(&transf);
    }

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandScaleTopo::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
