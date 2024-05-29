/*----------------------------------------------------------------------------*/
/** \file Vertex.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 5/10/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <memory>
/*----------------------------------------------------------------------------*/
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/FacetedVertex.h"
#include "Group/Group0D.h"
#include "Geom/MementoGeomEntity.h"
#include "Internal/Context.h"
#include "Topo/Vertex.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <gp_Pnt.hxx>
#include <BRep_Tool.hxx>
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
const char* Vertex::typeNameGeomVertex = "GeomVertex";
/*----------------------------------------------------------------------------*/
Vertex::Vertex(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
        GeomProperty* gprop, GeomRepresentation* compProp)
: GeomEntity(ctx, prop, disp, gprop,compProp)
{
}
/*----------------------------------------------------------------------------*/
GeomEntity* Vertex::clone(Internal::Context& c)
{
    return new Vertex(c,
            c.newProperty(this->getType()),
            c.newDisplayProperties(this->getType()),
            new GeomProperty(),
            this->getComputationalProperty()->clone());
}
/*----------------------------------------------------------------------------*/
Vertex::~Vertex()
{}

/*----------------------------------------------------------------------------*/
void Vertex::setFromSpecificMemento(MementoGeomEntity& mem)
{
    m_curves = mem.getCurves();
    m_groups = mem.getGroups0D();
}
/*----------------------------------------------------------------------------*/
void Vertex::createSpecificMemento(MementoGeomEntity& mem)
{
    mem.setCurves(m_curves);
    mem.setGroups0D(m_groups);
}
/*----------------------------------------------------------------------------*/
void Vertex::getRefEntities(std::vector<GeomEntity*>& entities)
{
    entities.clear();
    entities.insert(entities.end(),m_curves.begin(),m_curves.end());
}
/*----------------------------------------------------------------------------*/
void Vertex::clearRefEntities(std::list<GeomEntity*>& vertices,
        std::list<GeomEntity*>& curves,
        std::list<GeomEntity*>& surfaces,
        std::list<GeomEntity*>& volumes)
{
    std::vector<Curve*> toRemoveC;
    for(unsigned int i=0;i<m_curves.size();i++){
        GeomEntity *e = m_curves[i];
        std::list<GeomEntity*>::iterator res = std::find(curves.begin(),curves.end(),e);
        if(res!=curves.end())
            toRemoveC.push_back(dynamic_cast<Curve*>(e));
    }

    for(unsigned int i=0;i<toRemoveC.size();i++)
        remove(toRemoveC[i]);
}
/*----------------------------------------------------------------------------*/
Mgx3D::Utils::SerializedRepresentation* Vertex::getDescription (bool alsoComputed) const
{
	std::unique_ptr<Mgx3D::Utils::SerializedRepresentation>	description (
											GeomEntity::getDescription (alsoComputed));
	CHECK_NULL_PTR_ERROR (description.get ( ))
	std::vector<double>	coords;
	coords.push_back (getX ( ));
	coords.push_back (getY ( ));
	coords.push_back (getZ ( ));
	Mgx3D::Utils::SerializedRepresentation::Property	coordsProp (
														"Coordonnées", coords);
	Mgx3D::Utils::SerializedRepresentation	propertyGeomDescription (
											"Propriété géométrique", coordsProp.getValue ( ));
	propertyGeomDescription.addProperty (coordsProp);

    // précision OpenCascade ou autre
	TkUtil::UTF8String	precStr (TkUtil::Charset::UTF_8);
    precStr<<getComputationalProperty()->getPrecision();

    propertyGeomDescription.addProperty (
    	        Utils::SerializedRepresentation::Property ("Précision", precStr.ascii()) );

    bool isFaceted = false;
    FacetedVertex* fv = dynamic_cast<FacetedVertex*>(getComputationalProperty());
    if (fv){
    	propertyGeomDescription.addProperty (
    	    		Utils::SerializedRepresentation::Property ("Type", std::string("facétisé")) );
    }

	description->addPropertiesSet (propertyGeomDescription);
	description->setSummary (coordsProp.getValue ( ));

	return description.release ( );
}
/*----------------------------------------------------------------------------*/
std::string Vertex::getSummary ( ) const
{
	std::vector<double>	coords;
	coords.push_back (getX ( ));
	coords.push_back (getY ( ));
	coords.push_back (getZ ( ));
	Mgx3D::Utils::SerializedRepresentation::Property	coordsProp (
														"Coordonnées", coords);
	return coordsProp.getValue ( );
}
/*----------------------------------------------------------------------------*/
double Vertex::computeArea() const
{
    return 0;
}
/*----------------------------------------------------------------------------*/
void Vertex::get(std::vector<Vertex*>& vertices) const
{
    std::list<Vertex*> l;
    vertices.clear();
    for(unsigned int i=0; i <m_curves.size();i++){
        Curve* c = m_curves[i];
        std::vector<Vertex*> local_vertices;
        c->get(local_vertices);
        l.insert(l.end(),local_vertices.begin(),local_vertices.end());
    }
    l.sort(Utils::Entity::compareEntity);
    l.unique();
    l.remove(const_cast<Vertex*>(this));
    vertices.insert(vertices.end(),l.begin(),l.end());
}
/*----------------------------------------------------------------------------*/
void Vertex::get(std::vector<Curve*>& curves) const
{
    curves.clear();
    curves.insert(curves.end(),m_curves.begin(),m_curves.end());
}
/*----------------------------------------------------------------------------*/
void Vertex::get(std::vector<Surface*>& surfaces) const
{
    std::list<Surface*> l;
    surfaces.clear();
    for(unsigned int i=0; i <m_curves.size();i++){
        Curve* c = m_curves[i];
        std::vector<Surface*> local_surfaces;
        c->get(local_surfaces);
        l.insert(l.end(),local_surfaces.begin(),local_surfaces.end());
    }
    l.sort(Utils::Entity::compareEntity);
    l.unique();

    surfaces.insert(surfaces.end(),l.begin(),l.end());
}
/*----------------------------------------------------------------------------*/
void Vertex::get(std::vector<Volume*>& volumes) const
{
    std::list<Volume*> l;
    volumes.clear();
    for(unsigned int i=0; i <m_curves.size();i++){
        Curve* c = m_curves[i];
        std::vector<Volume*> local_vol;
        c->get(local_vol);
        l.insert(l.end(),local_vol.begin(),local_vol.end());
    }
    l.sort(Utils::Entity::compareEntity);
    l.unique();

    volumes.insert(volumes.end(),l.begin(),l.end());
}
/*----------------------------------------------------------------------------*/
void Vertex::get(std::vector<Topo::Vertex*>& vertices)
{
	const std::vector<Topo::TopoEntity* >& topos = getRefTopo();

	for (std::vector<Topo::TopoEntity* >::const_iterator iter = topos.begin();
			iter != topos.end(); ++iter)
		if ((*iter)->getDim() == 0){
			Topo::Vertex* vertex = dynamic_cast<Topo::Vertex*>(*iter);
			if (vertex)
				vertices.push_back(vertex);
		}
}
/*----------------------------------------------------------------------------*/
void Vertex::project(Utils::Math::Point& P) const
{
    P = getCenteredPosition();
}
/*----------------------------------------------------------------------------*/
void Vertex::project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const
{
    P2 = getCenteredPosition();
}
/*----------------------------------------------------------------------------*/
void Vertex::add(Curve* c)
{
    m_curves.push_back(c);
}
/*----------------------------------------------------------------------------*/
void Vertex::remove(Curve* c)
{
    std::vector<Curve*>::iterator it= m_curves.begin();

    while(it!=m_curves.end() && *it!=c)
        it++;

    if(it!=m_curves.end())
        m_curves.erase(it);
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Vertex::getCenteredPosition() const
{
    OCCGeomRepresentation* rep =
            dynamic_cast<OCCGeomRepresentation*>(getComputationalProperty());
    FacetedVertex* fv = dynamic_cast<FacetedVertex*>(getComputationalProperty());
    if (rep){

    	TopoDS_Vertex v = TopoDS::Vertex(rep->getShape());
    	gp_Pnt pnt = BRep_Tool::Pnt(v);
    	return Utils::Math::Point(pnt.X(),pnt.Y(), pnt.Z());
    }
    else if (fv){
    	Utils::Math::Point pt;
    	fv->getPoint(0, pt, true);
    	return pt;
    }
    else
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, type inconnu pour getCenteredPosition", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Vertex::getCoord() const
{
    return getCenteredPosition();
}
/*----------------------------------------------------------------------------*/
double Vertex::getX() const
{
    Utils::Math::Point p = getCenteredPosition();
    return p.getX();
}
/*----------------------------------------------------------------------------*/
double Vertex::getY() const
{
    Utils::Math::Point p = getCenteredPosition();
    return p.getY();
}
/*----------------------------------------------------------------------------*/
double Vertex::getZ() const
{
    Utils::Math::Point p = getCenteredPosition();
    return p.getZ();
}
/*----------------------------------------------------------------------------*/
bool Vertex::isA(const std::string& name)
{
    return (name.compare(0,getTinyName().size(),getTinyName()) == 0);
}
/*---------------------------------------------------------------------------*/
std::ostream& operator<<(std::ostream& str, const Vertex& v)
{
  str<<"Sommet ("<<v.getX()<<", "<<v.getY()<<", "<<v.getZ()<<")";
  return str;
}
/*---------------------------------------------------------------------------*/
TkUtil::UTF8String& operator<<(TkUtil::UTF8String& str, const Vertex& v)
{
  str<<"("<<v.getX()<<", "<<v.getY()<<", "<<v.getZ()<<")";
  return str;
}

/*----------------------------------------------------------------------------*/
void Vertex::add(Group::Group0D* grp)
{
    m_groups.push_back(grp);
}
/*----------------------------------------------------------------------------*/
void Vertex::remove(Group::Group0D* grp)
{
    uint i = 0;
    for (; i<m_groups.size() && grp != m_groups[i]; ++i)
        ;

    if (i!=m_groups.size())
        m_groups.erase(m_groups.begin()+i);
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne (pas de groupe), avec Vertex::remove", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
bool Vertex::find(Group::Group0D* grp)
{
    uint i = 0;
    for (; i<m_groups.size() && grp != m_groups[i]; ++i)
        ;

    return (i!=m_groups.size());
}
/*----------------------------------------------------------------------------*/
void Vertex::getGroupsName (std::vector<std::string>& gn) const
{
    gn.clear();
    for (uint i = 0; i<m_groups.size(); ++i)
        gn.push_back(m_groups[i]->getName());
}
/*----------------------------------------------------------------------------*/
void Vertex::getGroups(std::vector<Group::GroupEntity*>& grp) const
{
    grp.insert(grp.end(), m_groups.begin(), m_groups.end());
}
/*----------------------------------------------------------------------------*/
void Vertex::setGroups(std::vector<Group::GroupEntity*>& grp)
{
	m_groups.clear();
	for (std::vector<Group::GroupEntity*>::iterator iter = grp.begin(); iter != grp.end(); iter++){
		Group::Group0D* g0 = dynamic_cast<Group::Group0D*>(*iter);
		if (g0 == 0)
			throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne avec conversion en groupe local dans setGroups", TkUtil::Charset::UTF_8));
		m_groups.push_back(g0);
	}
}
/*----------------------------------------------------------------------------*/
int Vertex::getNbGroups() const
{
    return m_groups.size();
}
/*----------------------------------------------------------------------------*/
void Vertex::setDestroyed(bool b)
{
    if (isDestroyed() == b)
        return;

    // supprime la relation du groupe vers le sommet en cas de destruction
    if (b)
        for (uint i = 0; i<m_groups.size(); ++i)
            m_groups[i]->remove(this);
    else
        // et inversement en cas de ressurection
        for (uint i = 0; i<m_groups.size(); ++i)
            m_groups[i]->add(this);

    Entity::setDestroyed(b);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

