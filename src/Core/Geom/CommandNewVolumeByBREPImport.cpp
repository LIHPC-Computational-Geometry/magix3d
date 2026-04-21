/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewVolumeByBREPImport.h"
#include "Geom/EntityFactory.h"
#include "Geom/Volume.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TopoDS.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewVolumeByBREPImport::CommandNewVolumeByBREPImport(Internal::Context& c,
                             const TopoDS_Shape& shape)
:CommandCreateGeom(c, "Création d'un volume"), m_shape(shape)
{}
/*----------------------------------------------------------------------------*/
CommandNewVolumeByBREPImport::
~CommandNewVolumeByBREPImport()
{}
/*----------------------------------------------------------------------------*/
void CommandNewVolumeByBREPImport::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandNewVolumeByBREPImport::execute pour la commande " << getName ( )
		    << " de nom unique " << getUniqueName ( ) << ".";

    Volume* v;
    if (m_shape.ShapeType() == TopAbs_SOLID) {
        v = EntityFactory(getContext()).newOCCVolume(TopoDS::Solid(m_shape));
    } else if (m_shape.ShapeType() == TopAbs_SHELL) {
        v = EntityFactory(getContext()).newOCCVolume(TopoDS::Shell(m_shape));
    }
    m_createdEntities.push_back(v);

    // stockage dans le manager géom
    split(v);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création du volume "<<v->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
