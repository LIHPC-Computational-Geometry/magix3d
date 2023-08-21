/*
 * CommandGeomCopy.cpp
 *
 *  Created on: 19 juin 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/CommandGeomCopy.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomFuseImplementation.h"
#include "Geom/Curve.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {

class GeomVertex;
//class Curve;
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
	Geom::GeomManager& gm = getContext().getLocalGeomManager();

	std::vector<Volume*> volumes = gm.getVolumesObj();
	for (uint i=0; i<volumes.size(); i++)
		m_entities.push_back(volumes[i]);

	std::vector<Surface*> surfaces = gm.getSurfacesObj();
	for (uint i=0; i<surfaces.size(); i++)
		m_entities.push_back(surfaces[i]);

	std::vector<Curve*> curves = gm.getCurvesObj();
	for (uint i=0; i<curves.size(); i++)
		m_entities.push_back(curves[i]);

	std::vector<Vertex*> vertices = gm.getVerticesObj();
	for (uint i=0; i<vertices.size(); i++)
		m_entities.push_back(vertices[i]);

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
std::vector<GeomEntity*>& CommandGeomCopy::getRemovedEntities()
{
    return m_removed_entities;
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*>& CommandGeomCopy::getNewEntities()
{
    return m_new_entities;
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*> CommandGeomCopy::getKeepedEntities()
{
    std::vector<GeomEntity*> vec;
    for(int i=0;i<4;i++)
        vec.insert(vec.end(),m_ref_entities[i].begin(),m_ref_entities[i].end());

    return vec;
}
/*----------------------------------------------------------------------------*/
std::vector<GeomEntity*>& CommandGeomCopy::getMovedEntities()
{
   return m_moved_entities;
}
/*----------------------------------------------------------------------------*/
std::list<GeomEntity*>& CommandGeomCopy::getRefEntities(const int dim)
{
    return m_ref_entities[dim];
}
/*----------------------------------------------------------------------------*/
std::list<GeomEntity*>& CommandGeomCopy::getAdjEntities(const int dim)
{
    return m_adj_entities;
}
/*----------------------------------------------------------------------------*/
std::map<GeomEntity*,std::vector<GeomEntity*> >& CommandGeomCopy::
getReplacedEntities()
{
   return m_replaced_entities;
}
/*----------------------------------------------------------------------------*/
std::map<GeomEntity*, GeomEntity* >& CommandGeomCopy::
getRef2NewEntities()
{
    return m_ref_to_new_entities;
}
/*----------------------------------------------------------------------------*/
void CommandGeomCopy::internalSpecificPreExecute()
{
    /* On doit faire attention de copier aussi les entités de dimension
     * inférieure. Et cela une seule fois!!
     */
    m_ref_entities.resize(4);
    for(unsigned int i=0;i<m_entities.size();i++){
          GeomEntity* e = m_entities[i];
          m_ref_entities[e->getDim()].push_back(e);
          if(e->getDim()>0){
                 std::vector<Vertex*> vertices;
                 e->get(vertices);
                 for(unsigned int i=0;i<vertices.size();i++)
                 {
                     GeomEntity *v = vertices[i];
                     m_ref_entities[0].push_back(v);
                 }
             }
             if(e->getDim()>1){
                 std::vector<Curve*> curves;
                 e->get(curves);
                 for(unsigned int i=0;i<curves.size();i++)
                 {
                     GeomEntity *c = curves[i];
                     m_ref_entities[1].push_back(c);
                 }
             }
             if(e->getDim()>2){
                 std::vector<Surface*> surfs;
                 e->get(surfs);
                 for(unsigned int i=0;i<surfs.size();i++)
                 {
                     GeomEntity *f = surfs[i];
                     m_ref_entities[2].push_back(f);
                 }
             }
      }

      // conservation des entités de références de manière unique
      for(int i=0;i<4;i++){
          m_ref_entities[i].sort(Utils::Entity::compareEntity);
          m_ref_entities[i].unique();
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
        GeomEntity* ge = *it;
        std::vector<Surface*> surfaces;
        std::vector<Curve*> curves;
        std::vector<Vertex*> vertices;
        ge->get(surfaces);
        ge->get(curves);
        ge->get(vertices);

        GeomEntity* new_ge = m_correspondance[3][ge];
        for(unsigned int i=0;i<surfaces.size();i++){
            GeomEntity* new_surf = m_correspondance[2][surfaces[i]];
            new_ge->add(new_surf);
            new_surf->add(new_ge);
        }
        for(unsigned int i=0;i<curves.size();i++){
            GeomEntity* new_curv = m_correspondance[1][curves[i]];
            new_ge->add(new_curv);
            new_curv->add(new_ge);
        }
        for(unsigned int i=0;i<vertices.size();i++){
            GeomEntity* new_vert= m_correspondance[0][vertices[i]];
            new_ge->add(new_vert);
            new_vert->add(new_ge);
        }
    }
    //pour les surfaces
    it = m_ref_entities[2].begin();
    for(;it!=m_ref_entities[2].end();it++)
    {
        GeomEntity* ge = *it;
        std::vector<Curve*> curves;
        std::vector<Vertex*> vertices;
        ge->get(curves);
        ge->get(vertices);

        GeomEntity* new_ge = m_correspondance[2][ge];

        for(unsigned int i=0;i<curves.size();i++){
            GeomEntity* new_curv = m_correspondance[1][curves[i]];
            new_ge->add(new_curv);
            new_curv->add(new_ge);
        }
        for(unsigned int i=0;i<vertices.size();i++){
            GeomEntity* new_vert= m_correspondance[0][vertices[i]];
            new_ge->add(new_vert);
            new_vert->add(new_ge);
        }
    }
    //pour les courbes
    it = m_ref_entities[1].begin();
    for(;it!=m_ref_entities[1].end();it++)
    {
        GeomEntity* ge = *it;
        std::vector<Vertex*> vertices;
        ge->get(vertices);

        GeomEntity* new_ge = m_correspondance[1][ge];

        for(unsigned int i=0;i<vertices.size();i++){
            GeomEntity* new_vert= m_correspondance[0][vertices[i]];
            new_ge->add(new_vert);
            new_vert->add(new_ge);
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
