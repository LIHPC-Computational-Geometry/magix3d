/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewSurfaceByBREPImport.h"
#include "Geom/EntityFactory.h"
#include "Geom/Surface.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewSurfaceByBREPImport::CommandNewSurfaceByBREPImport(Internal::Context& c,
                             const std::vector<TopoDS_Face>& shapes)
:CommandCreateGeom(c, "Création d'une surface"), m_faceShapes(shapes)
{}
/*----------------------------------------------------------------------------*/
CommandNewSurfaceByBREPImport::
~CommandNewSurfaceByBREPImport()
{}
/*----------------------------------------------------------------------------*/
void CommandNewSurfaceByBREPImport::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandNewSurfaceByBREPImport::execute pour la commande " << getName ( )
		    << " de nom unique " << getUniqueName ( ) << ".";

    Surface *s;
    if (m_faceShapes.size() == 1) {
        s = EntityFactory(getContext()).newOCCSurface(m_faceShapes[0]);
    } else {
        s = EntityFactory(getContext()).newOCCCompositeSurface(m_faceShapes);
    }
    m_createdEntities.push_back(s);

    // stockage dans le manager géom
    split(s);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création de la surface "<<s->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
