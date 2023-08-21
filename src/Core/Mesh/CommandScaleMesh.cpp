/*----------------------------------------------------------------------------*/
/*
 * \file CommandScaleMesh.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/2/2016
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/CommandScaleMesh.h"

#include "Utils/Common.h"
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
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandScaleMesh::
CommandScaleMesh(Internal::Context& c,
		const double& facteur,
        const Utils::Math::Point& pcentre)
:CommandTransformMesh(c, "Homothétie de tout le maillage")
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
	comments << "Homothétie de tout le maillage";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandScaleMesh::
CommandScaleMesh(Internal::Context& c,
		const double factorX,
		const double factorY,
		const double factorZ)
:CommandTransformMesh(c, "Homothétie de tout le maillage")
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
	comments << "Homothétie de tout le maillage";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandScaleMesh::
~CommandScaleMesh()
{
}
/*----------------------------------------------------------------------------*/
void CommandScaleMesh::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandScaleMesh::execute pour la commande " << getName ( )
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
    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandScaleMesh::
internalUndo()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandScaleMesh::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    if (m_isHomogene){
    	// création de l'opérateur d'homothétie via OCC
    	gp_Trsf transf;
    	transf.SetScale(gp_Pnt(m_center.getX(), m_center.getY(), m_center.getZ()), 1.0/m_factor);

    	// applique la transformation
    	transform(&transf);
    }
    else {
        // création de l'opérateur d'homothétie via OCC
        gp_GTrsf transf;
        transf.SetValue(1,1, 1.0/m_factorX);
        transf.SetValue(2,2, 1.0/m_factorY);
        transf.SetValue(3,3, 1.0/m_factorZ);

        // applique la transformation
        transform(&transf);
    }

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
