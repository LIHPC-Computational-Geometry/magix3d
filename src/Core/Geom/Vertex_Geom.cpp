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
#include "Geom/OCCHelper.h"
#include "Group/Group0D.h"
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
        GeomProperty* gprop, TopoDS_Vertex& shape)
: GeomEntity(ctx, prop, disp, gprop)
, m_occ_vertex(shape)
{
}
/*----------------------------------------------------------------------------*/
void Vertex::apply(std::function<void(const TopoDS_Shape&)> const& lambda) const
{
    lambda(m_occ_vertex);
}
/*----------------------------------------------------------------------------*/
void Vertex::applyAndReturn(std::function<TopoDS_Shape(const TopoDS_Shape&)> const& lambda)
{
    m_occ_vertex = TopoDS::Vertex(lambda(m_occ_vertex));
}
/*----------------------------------------------------------------------------*/
GeomEntity* Vertex::clone(Internal::Context& c)
{
    return new Vertex(c,
            c.newProperty(this->getType()),
            c.newDisplayProperties(this->getType()),
            new GeomProperty(),
            m_occ_vertex);
}
/*----------------------------------------------------------------------------*/
Vertex::~Vertex()
{}
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

#ifdef _DEBUG		// Issue#111
    // précision OpenCascade ou autre
	TkUtil::UTF8String	precStr (TkUtil::Charset::UTF_8);
    precStr << BRep_Tool::Tolerance(m_occ_vertex);;

    propertyGeomDescription.addProperty (
    	        Utils::SerializedRepresentation::Property ("Précision", precStr.ascii()) );
#endif	// _DEBUG

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
void Vertex::computeBoundingBox(Utils::Math::Point& pmin,Utils::Math::Point& pmax) const
{
    OCCHelper::computeBoundingBox(m_occ_vertex, pmin, pmax);
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
uint Vertex::project(Utils::Math::Point& P) const
{
    P = getCenteredPosition();
    return 0;
}
/*----------------------------------------------------------------------------*/
uint Vertex::project(const Utils::Math::Point& P1, Utils::Math::Point& P2) const
{
    P2 = getCenteredPosition();
    return 0;
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
    gp_Pnt pnt = BRep_Tool::Pnt(m_occ_vertex);
    return Utils::Math::Point(pnt.X(),pnt.Y(), pnt.Z());
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

