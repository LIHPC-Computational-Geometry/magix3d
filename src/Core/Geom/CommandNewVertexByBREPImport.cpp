/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewVertexByBREPImport.h"
#include "Geom/EntityFactory.h"
#include "Geom/Vertex.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <BRep_Tool.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewVertexByBREPImport::CommandNewVertexByBREPImport(Internal::Context& c,
                             const TopoDS_Vertex& shape)
:CommandCreateGeom(c, "Création d'un sommet"), m_vertexShape(shape)
{}
/*----------------------------------------------------------------------------*/
CommandNewVertexByBREPImport::
~CommandNewVertexByBREPImport()
{}
/*----------------------------------------------------------------------------*/
void CommandNewVertexByBREPImport::
internalExecute()
{
    gp_Pnt point = BRep_Tool::Pnt(m_vertexShape);
    double dx = point.X();
    double dy = point.Y();
    double dz = point.Z();
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandNewVertexByBREPImport::execute pour la commande " << getName ( )
		    << " de nom unique " << getUniqueName ( )
//		    << std::ios_base::fixed << TkUtil::setprecision (8)
		    << ". P ("
	        << Utils::Math::MgxNumeric::userRepresentation (dx) << ", "
		    << Utils::Math::MgxNumeric::userRepresentation (dy) << ", "
		    << Utils::Math::MgxNumeric::userRepresentation (dz) << ").";

    Vertex* v= EntityFactory(getContext()).newOCCVertex(m_vertexShape);
    m_createdEntities.push_back(v);

    // stockage dans le manager géom
    split(v);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création du sommet "<<v->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
