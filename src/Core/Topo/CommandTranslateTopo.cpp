/*----------------------------------------------------------------------------*/
/*
 * \file CommandTranslateTopo.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 27/9/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandTranslateTopo.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Topo/Block.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
// OCC
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandTranslateTopo::
CommandTranslateTopo(Internal::Context& c, std::vector<TopoEntity*>& ve, const Vector& dp)
:CommandTransformTopo(c, "Translation d'une topologie seule", ve)
, m_vector(dp)
{
    if (Utils::Math::MgxNumeric::isNearlyZero(dp.abs2()))
        throw TkUtil::Exception (TkUtil::UTF8String ("La translation ne peut se faire avec un vecteur nul", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Translation de la topologie seule";
	for (uint i=0; i<ve.size() && i<5; i++)
		comments << " " << ve[i]->getName();
	if (ve.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandTranslateTopo::
CommandTranslateTopo(Internal::Context& c,
		CommandDuplicateTopo* cmd,
		const Vector& dp,
		bool all_topo)
:CommandTransformTopo(c, "Translation d'une copie de la topologie", cmd, all_topo)
, m_vector(dp)
{
    if (Utils::Math::MgxNumeric::isNearlyZero(dp.abs2()))
        throw TkUtil::Exception (TkUtil::UTF8String ("La translation ne peut se faire avec un vecteur nul", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandTranslateTopo::
CommandTranslateTopo(Internal::Context& c, const Vector& dp)
:CommandTransformTopo(c, "Translation de tout")
, m_vector(dp)
{
    if (Utils::Math::MgxNumeric::isNearlyZero(dp.abs2()))
        throw TkUtil::Exception (TkUtil::UTF8String ("La translation ne peut se faire avec un vecteur nul", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Translation de tout";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandTranslateTopo::
~CommandTranslateTopo()
{
}
/*----------------------------------------------------------------------------*/
void CommandTranslateTopo::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandTranslateTopo::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // création de l'opérateur de translation via OCC
    gp_Trsf transf;
    gp_Vec vec(m_vector.getX(),m_vector.getY(),m_vector.getZ());
    transf.SetTranslation(vec);

    // applique la transformation
    transform(&transf);

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandTranslateTopo::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
