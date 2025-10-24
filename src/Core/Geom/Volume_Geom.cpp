/*----------------------------------------------------------------------------*/
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/OCCHelper.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
/*----------------------------------------------------------------------------*/
#include <list>
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
const char* Volume::typeNameGeomVolume = "GeomVolume";
/*----------------------------------------------------------------------------*/
Volume::Volume(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp,
        GeomProperty* gprop, TopoDS_Shape& shape)
: GeomEntity(ctx, prop, disp, gprop)
, m_occ_shape(shape)
{
}
/*----------------------------------------------------------------------------*/
void Volume::apply(std::function<void(const TopoDS_Shape&)> const& lambda) const
{
    lambda(m_occ_shape);
}
/*----------------------------------------------------------------------------*/
void Volume::applyAndReturn(std::function<TopoDS_Shape(const TopoDS_Shape&)> const& lambda)
{
    m_occ_shape = lambda(m_occ_shape);
}
/*----------------------------------------------------------------------------*/
GeomEntity* Volume::clone(Internal::Context& c)
{
    return new Volume(c,
            c.newProperty(this->getType()),
            c.newDisplayProperties(this->getType()),
            new GeomProperty(),
            m_occ_shape);
}

/*----------------------------------------------------------------------------*/
Volume::~Volume()
{}
/*----------------------------------------------------------------------------*/
void Volume::add(Surface* s)
{
    m_surfaces.push_back(s);
}
/*----------------------------------------------------------------------------*/
void Volume::remove(Surface* s)
{
#ifdef _DEBUG2
	std::cout<<"Volume::remove ("<<s->getName()<<") dans "<<getName()<<"\n qui contient les surfaces :"<<std::endl;
	for (uint i=0; i<m_surfaces.size(); i++)
		std::cout<<" "<<m_surfaces[i]->getName();
	std::cout<<std::endl;
#endif

    std::vector<Surface*>::iterator it= m_surfaces.begin();
    while((it!=m_surfaces.end()) && (*it!=s))
        it++;

    if(it!=m_surfaces.end())
        m_surfaces.erase(it);
}
/*----------------------------------------------------------------------------*/
double Volume::computeArea() const
{
    // 1 seule représentation pour le volume
    return OCCHelper::computeArea(m_occ_shape);
}
/*----------------------------------------------------------------------------*/
void Volume::computeBoundingBox(Utils::Math::Point& pmin, Utils::Math::Point& pmax) const
{
    OCCHelper::computeBoundingBox(m_occ_shape, pmin, pmax);
}
/*----------------------------------------------------------------------------*/
void Volume::addAllDownLevelEntity(std::list<GeomEntity*>& l_entity) const
{
    for(unsigned int i=0; i <m_surfaces.size();i++){
        l_entity.push_back(m_surfaces[i]);
        m_surfaces[i]->addAllDownLevelEntity(l_entity);
    }
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Volume::
getCenteredPosition() const
{
    // on prend le barycentre des points aux centres des surfaces
    // ce qui est plus juste pour le cas d'une demi sphère creuse par exemple
    Utils::Math::Point pt;

    for (uint i=0; i<m_surfaces.size(); i++)
        pt += m_surfaces[i]->getCenteredPosition();
    pt /= (double)m_surfaces.size();
    return pt;
}
/*----------------------------------------------------------------------------*/
bool Volume::isA(const std::string& name)
{
    return (name.compare(0,getTinyName().size(),getTinyName()) == 0);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
