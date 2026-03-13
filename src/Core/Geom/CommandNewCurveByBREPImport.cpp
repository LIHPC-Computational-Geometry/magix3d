/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewCurveByBREPImport.h"
#include "Geom/EntityFactory.h"
#include "Geom/Curve.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewCurveByBREPImport::CommandNewCurveByBREPImport(Internal::Context& c,
                             const std::vector<TopoDS_Edge>& shapes,
                             const Utils::Math::Point& extremaFirst, const Utils::Math::Point& extremaLast)
:CommandCreateGeom(c, "Création d'une courbe"), m_edgeShapes(shapes),
                   m_extremaFirst(extremaFirst), m_extremaLast(extremaLast)
{}
/*----------------------------------------------------------------------------*/
CommandNewCurveByBREPImport::
~CommandNewCurveByBREPImport()
{}
/*----------------------------------------------------------------------------*/
void CommandNewCurveByBREPImport::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandNewCurveByBREPImport::execute pour la commande " << getName ( )
		    << " de nom unique " << getUniqueName ( ) << ".";

    Curve *c;
    if (m_edgeShapes.size() == 1) {
        c = EntityFactory(getContext()).newOCCCurve(m_edgeShapes[0]);
    } else {
        c = EntityFactory(getContext()).newOCCCompositeCurve(m_edgeShapes, m_extremaFirst, m_extremaLast);
    }
    m_createdEntities.push_back(c);

    // stockage dans le manager géom
    split(c);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création de la courbe "<<c->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
