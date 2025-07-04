/*----------------------------------------------------------------------------*/
#include "Geom/CommandJoinEntities.h"
#include "Geom/GeomManager.h"
#include "Geom/IncidentGeomEntitiesVisitor.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_JOIN
CommandJoinEntities::
CommandJoinEntities(Internal::Context& c, std::string titreCmd, std::vector<GeomEntity*>& e)
: CommandEditGeom(c, titreCmd)
, m_entities(e)
{
    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << titreCmd;
	for (uint i=0; i<m_entities.size() && i<5; i++)
			if (m_entities[i])
				comments << " " << m_entities[i]->getName();
	if (m_entities.size()>5)
		comments << " ... ";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandJoinEntities::~CommandJoinEntities()
{
}
/*----------------------------------------------------------------------------*/
void CommandJoinEntities::init(std::vector<GeomEntity*>& es)
{
    // pour chacune des entités passées en argument, on ajoute les entités de
    // dimension inférieure et supérieure ou égale en référence.
    for(unsigned int i=0;i<es.size();i++){
        GeomEntity* esi = es[i];
        //m_init_entities.push_back(esi);
        addReference(esi);
        addDownIncidentReference(esi);
        addUpIncidentReference(esi);
    }

    // une entité a pu être ajoutée plusieurs fois, on élimine maintenant les
    // occurrences multiples
    for(int i=0;i<4;i++){
        m_ref_entities[i].sort(Utils::Entity::compareEntity);
        m_ref_entities[i].unique();
    }
}
/*----------------------------------------------------------------------------*/
void CommandJoinEntities::internalSpecificPreExecute()
{
    validate();
    init(m_entities);
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*>& CommandJoinEntities::getRemovedEntities()
{
	return m_removedEntities;
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*>& CommandJoinEntities::getNewEntities()
{
	return m_newEntities;
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*> CommandJoinEntities::getKeepedEntities()
{
	std::vector<GeomEntity*> e;
    e.insert(e.end(),m_toKeepVolumes.begin(),m_toKeepVolumes.end());
    e.insert(e.end(),m_toKeepSurfaces.begin(),m_toKeepSurfaces.end());
    e.insert(e.end(),m_toKeepCurves.begin(),m_toKeepCurves.end());
    e.insert(e.end(),m_toKeepVertices.begin(),m_toKeepVertices.end());

	return e;
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*>& CommandJoinEntities::getMovedEntities()
{
	return m_movedEntities;
}
/*----------------------------------------------------------------------------*/
std::list<GeomEntity*>& CommandJoinEntities::getRefEntities(const int dim)
{
    return m_ref_entities[dim];
}
/*----------------------------------------------------------------------------*/
std::map<GeomEntity*,std::vector<GeomEntity*> >& CommandJoinEntities::getReplacedEntities()
{
    return m_replacedEntities;
}
/*----------------------------------------------------------------------------*/
void CommandJoinEntities::
addReference(GeomEntity* e)
{
    m_ref_entities[e->getDim()].push_back(e);
#ifdef _DEBUG_JOIN
    std::cout<<"addReference "<<e->getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandJoinEntities::
addDownIncidentReference(GeomEntity* e)
{
    GetDownIncidentGeomEntitiesVisitor v;
    e->accept(v);
    for (auto ei : v.get())
        addReference(ei);
}
/*----------------------------------------------------------------------------*/
void CommandJoinEntities::
addUpIncidentReference(GeomEntity* e)
{
    GetUpIncidentGeomEntitiesVisitor v;
    e->accept(v);
    for (auto ei : v.get())
        addReference(ei);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
