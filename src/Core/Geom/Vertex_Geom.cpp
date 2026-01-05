/*----------------------------------------------------------------------------*/
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/OCCHelper.h"
#include "Internal/Context.h"
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
#include <memory>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
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
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

