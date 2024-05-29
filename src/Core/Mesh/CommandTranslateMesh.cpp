/*----------------------------------------------------------------------------*/
/*
 * \file CommandTranslateMesh.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/2/2016
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/CommandTranslateMesh.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
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
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandTranslateMesh::
CommandTranslateMesh(Internal::Context& c, const Vector& dp)
:CommandTransformMesh(c, "Translation de tout le maillage")
, m_vector(dp)
{
    if (Utils::Math::MgxNumeric::isNearlyZero(dp.abs2()))
        throw TkUtil::Exception (TkUtil::UTF8String ("La translation ne peut se faire avec un vecteur nul", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Translation de tout le maillage";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandTranslateMesh::
~CommandTranslateMesh()
{
}
/*----------------------------------------------------------------------------*/
void CommandTranslateMesh::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandTranslateMesh::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // création de l'opérateur de rotation via OCC
    gp_Trsf transf;
    gp_Vec vec(m_vector.getX(),m_vector.getY(),m_vector.getZ());
    transf.SetTranslation(vec);

    // applique la transformation
    transform(&transf);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandTranslateMesh::
internalUndo()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandTranslateMesh::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    // création de l'opérateur de rotation via OCC
    gp_Trsf transf;
    gp_Vec vec(-m_vector.getX(),-m_vector.getY(),-m_vector.getZ());
    transf.SetTranslation(vec);

    // applique la transformation
    transform(&transf);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
