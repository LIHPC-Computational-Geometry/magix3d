/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Geom/CommandGeomCopy.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomFuseImplementation.h"
#include "Geom/Curve.h"
#include "Geom/EntityFactory.h"
#include "Geom/IncidentGeomEntitiesVisitor.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {

class GeomVertex;
class Surface;
class Volume;
/*----------------------------------------------------------------------------*/
CommandGeomCopy::
CommandGeomCopy(Internal::Context& c,
		std::vector<GeomEntity*>& e,
		const std::string& groupName)
: CommandEditGeom(c, "Copie", groupName)
, m_entities(e)
{
    m_adj_entities.clear();
    m_moved_entities.clear();
    m_removed_entities.clear();
    m_replaced_entities.clear();
    for(unsigned int i=0;i<4;i++)
    	m_correspondance[i].clear();
    validate();
    m_impl=0;
    updateDimensionGroup(m_entities);

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Copie de ";
	for (uint i=0; i<m_entities.size() && i<5; i++)
		comments << " " << m_entities[i]->getName();
	if (m_entities.size()>5)
		comments << " ... ";
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandGeomCopy::
CommandGeomCopy(Internal::Context& c,
                const std::string& groupName)
: CommandEditGeom(c, "Copie", groupName)
, m_entities()
{
	Geom::GeomManager& gm = getContext().getGeomManager();

	for (Volume* v : gm.getVolumesObj())
		m_entities.push_back(v);

	for (Surface* s : gm.getSurfacesObj())
		m_entities.push_back(s);

	for (Curve* c : gm.getCurvesObj())
		m_entities.push_back(c);

	for (Vertex* v : gm.getVerticesObj())
		m_entities.push_back(v);

    m_adj_entities.clear();
    m_moved_entities.clear();
    m_removed_entities.clear();
    m_replaced_entities.clear();
    for(unsigned int i=0;i<4;i++)
    	m_correspondance[i].clear();
    validate();
    m_impl=0;
    updateDimensionGroup(m_entities);
}
/*----------------------------------------------------------------------------*/
CommandGeomCopy::~CommandGeomCopy()
{}
/*----------------------------------------------------------------------------*/
void CommandGeomCopy::validate()
{}
/*----------------------------------------------------------------------------*/
const std::vector<GeomEntity*>& CommandGeomCopy::getRemovedEntities() const
{
    return m_removed_entities;
}
/*----------------------------------------------------------------------------*/
const std::vector<GeomEntity*>& CommandGeomCopy::getNewEntities() const
{
    return m_new_entities;
}
/*----------------------------------------------------------------------------*/
const std::vector<GeomEntity*> CommandGeomCopy::getKeepedEntities() const
{
    std::vector<GeomEntity*> vec;
    for(int i=0;i<4;i++)
        vec.insert(vec.end(),m_ref_entities[i].begin(),m_ref_entities[i].end());

    return vec;
}
/*----------------------------------------------------------------------------*/
const std::vector<GeomEntity*>& CommandGeomCopy::getMovedEntities() const
{
   return m_moved_entities;
}
/*----------------------------------------------------------------------------*/
const std::list<GeomEntity*>& CommandGeomCopy::getRefEntities(const int dim) const
{
    return m_ref_entities[dim];
}
/*----------------------------------------------------------------------------*/
const std::list<GeomEntity*>& CommandGeomCopy::getAdjEntities(const int dim) const
{
    return m_adj_entities;
}
/*----------------------------------------------------------------------------*/
const std::map<GeomEntity*,std::vector<GeomEntity*> >& CommandGeomCopy::getReplacedEntities() const
{
   return m_replaced_entities;
}
/*----------------------------------------------------------------------------*/
const std::map<GeomEntity*, GeomEntity*>& CommandGeomCopy::getRef2NewEntities() const
{
    return m_ref_to_new_entities;
}
/*----------------------------------------------------------------------------*/
void CommandGeomCopy::internalSpecificPreExecute()
{
    /* On doit faire attention de copier aussi les entités de dimension
     * inférieure. Et cela une seule fois!!
     */
    GetDownIncidentGeomEntitiesVisitor v;
    m_ref_entities.resize(4);
    for(unsigned int i=0;i<m_entities.size();i++){
          GeomEntity* e = m_entities[i];
          m_ref_entities[e->getDim()].push_back(e);
          e->accept(v);
    }

    for (auto ei : v.get())
        m_ref_entities[ei->getDim()].push_back(ei);
        
    for(int i=0;i<4;i++){
        // TODO A vérifier, peut-être inutile ?
        m_ref_entities[i].sort(Utils::Entity::compareEntity);
    }
}
/*----------------------------------------------------------------------------*/
void CommandGeomCopy::internalSpecificExecute()
{

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandGeomCopy::execute pour la commande " << getName ( )
                       << " de nom unique " << getUniqueName ( )
                       << std::ios_base::fixed << TkUtil::setprecision (8);

    for(unsigned int i=0;i<4;i++){
        std::list<GeomEntity*>::iterator it = m_ref_entities[i].begin();
        for(;it!=m_ref_entities[i].end();it++)
        {
            GeomEntity* ge = *it;
            GeomEntity* e= EntityFactory(getContext()).copy(ge);
            m_createdEntities.push_back(e);
            //pour que le GeomManager conserve l'ojet comme créé

           // store(e);

            m_correspondance[i].insert(std::pair<GeomEntity*,GeomEntity*>(ge,e));
        }
    }

    //une fois les entités copiées, on met a jour les relations topologiques
    //entre les nouvelles
    //pour les volumes
    std::list<GeomEntity*>::iterator it = m_ref_entities[3].begin();
    for(;it!=m_ref_entities[3].end();it++)
    {
        Volume* v = dynamic_cast<Volume*>(*it);
        auto surfaces = v->getSurfaces();

        Volume* new_v = dynamic_cast<Volume*>(m_correspondance[3][v]);
        for(unsigned int i=0;i<surfaces.size();i++){
            Surface* new_surf = dynamic_cast<Surface*>(m_correspondance[2][surfaces[i]]);
            new_v->add(new_surf);
            new_surf->add(new_v);
        }
    }

    //pour les surfaces
    it = m_ref_entities[2].begin();
    for(;it!=m_ref_entities[2].end();it++)
    {
        Surface* s = dynamic_cast<Surface*>(*it);
        auto curves = s->getCurves();

        Surface* new_s = dynamic_cast<Surface*>(m_correspondance[2][s]);
        for(unsigned int i=0;i<curves.size();i++){
            Curve* new_curv = dynamic_cast<Curve*>(m_correspondance[1][curves[i]]);
            new_s->add(new_curv);
            new_curv->add(new_s);
        }
    }

    //pour les courbes
    it = m_ref_entities[1].begin();
    for(;it!=m_ref_entities[1].end();it++)
    {
        Curve* c = dynamic_cast<Curve*>(*it);
        auto vertices = c->getVertices();

        Curve* new_c = dynamic_cast<Curve*>(m_correspondance[1][c]);
        for(unsigned int i=0;i<vertices.size();i++){
            Vertex* new_vert = dynamic_cast<Vertex*>(m_correspondance[0][vertices[i]]);
            new_c->add(new_vert);
            new_vert->add(new_c);
        }
    }

    //on finalise le traitement en remplissant les containers fournis en
    //attributs pour consultation ultérieure
    for(unsigned int i=0;i<4;i++){
        std::list<GeomEntity*>::iterator it = m_ref_entities[i].begin();
        std::map<GeomEntity*,GeomEntity*> i_map = m_correspondance[i];
        for(;it!=m_ref_entities[i].end();it++)
        {
            GeomEntity* from_entity = *it;
            GeomEntity* to_entity   = i_map[from_entity];
            m_new_entities.push_back(to_entity);
            m_ref_to_new_entities.insert(std::pair<GeomEntity*,GeomEntity*>(from_entity,to_entity));
        }
    }

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandGeomCopy::updateGroups()
{
    // copie des groupes des entités géométriques s'il n'y a pas de nom de group de spécifié
    if (getGroupName().empty()){

        for(unsigned int i=0;i<4;i++){
            std::list<GeomEntity*>::iterator it = m_ref_entities[i].begin();
            std::map<GeomEntity*,GeomEntity*> i_map = m_correspondance[i];
            for(;it!=m_ref_entities[i].end();it++)
            {
                GeomEntity* from_entity = *it;
                GeomEntity* to_entity   = i_map[from_entity];
                copyGroups(from_entity, to_entity);
            }
        }
    }
    else
    	CommandEditGeom::updateGroups();
}
/*----------------------------------------------------------------------------*/
const std::vector<GeomEntity*>& CommandGeomCopy::getEntities() const
{
	return m_entities;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
