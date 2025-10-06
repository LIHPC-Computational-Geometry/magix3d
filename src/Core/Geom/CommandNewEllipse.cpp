/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Geom/CommandNewEllipse.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewEllipse::
CommandNewEllipse(Internal::Context& c,
        Geom::Vertex* p1, Geom::Vertex* p2, Geom::Vertex* center,
        const std::string& groupName)
:CommandCreateWithOtherGeomEntities(c, "Création d'une ellipse", groupName),
 m_p1(p1), m_p2(p2), m_center(center)
{}
/*----------------------------------------------------------------------------*/
CommandNewEllipse::~CommandNewEllipse()
{}
/*----------------------------------------------------------------------------*/
void CommandNewEllipse::
internalExecute()
{
    TkUtil::UTF8String message (TkUtil::Charset::UTF_8);
    message << "CommandNewEllipse::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( )
            << std::ios_base::fixed << TkUtil::setprecision (8)
            << ". p1 " << *m_p1<<", p2 "
            << *m_p2<< ", center "<<*m_center<<".";

    // sauvegarde des relations (connections topologiques) avant modification
    m_memento_manager.saveMemento(m_p1);
    m_memento_manager.saveMemento(m_p2);
    m_memento_manager.saveMemento(m_center);

    Curve* c = EntityFactory(getContext()).newEllipse(m_p1,m_p2,m_center);
    m_createdEntities.push_back(c);

    // ajoute la courbe au groupe s'il y en a un de spécifié
    m_group_helper.addToGroup(m_group_name, c);

    //aucune creation de connections topologiques
    split(c);

    // information un peu plus parlante
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
    comments << "Création d'une ellipse "<<c->getName();
    setScriptComments(comments);
    setName(comments);

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewEllipse::
getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewObjects(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
