/*----------------------------------------------------------------------------*/
#include "Geom/GeomEntity.h"
#include "Geom/Volume.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/OCCHelper.h"
#include "Geom/GeomProperty.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Geom/OCCDisplayRepresentationBuilder.h"
#include "Geom/OCCFacetedRepresentationBuilder.h"
#include "Geom/IncidentGeomEntitiesVisitor.h"
/*----------------------------------------------------------------------------*/
#include "Services/DescriptionService.h"
/*----------------------------------------------------------------------------*/
#include "Group/Group0D.h"
#include "Group/Group1D.h"
#include "Group/Group2D.h"
#include "Group/Group3D.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
#include <memory>			// unique_ptr
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomEntity::GeomEntity(Internal::Context& ctx, Utils::Property* prop, Utils::DisplayProperties* disp, GeomProperty* gprop)
: Internal::InternalEntity (ctx, prop, disp),
  m_geomProp(gprop), m_computedAreaIsUpToDate(false), m_computedArea(0)
{
}
/*----------------------------------------------------------------------------*/
GeomEntity::~GeomEntity()
{
    if(m_geomProp!=0)
        delete m_geomProp;
}
/*----------------------------------------------------------------------------*/
void GeomEntity::getBounds (double bounds[6]) const
{
	Entity::getBounds (bounds);	// RAZ
	Utils::Math::Point	min, max;
	computeBoundingBox (min, max);
	bounds [0]	= min.getX ( );
	bounds [1]	= max.getX ( );
	bounds [2]	= min.getY ( );
	bounds [3]	= max.getY ( );
	bounds [4]	= min.getZ ( );
	bounds [5]	= max.getZ ( );
}
/*----------------------------------------------------------------------------*/
GeomProperty* GeomEntity::setGeomProperty(GeomProperty* prop)
{
    GeomProperty* old_rep=0;
    if(prop==0){
        throw   TkUtil::Exception(TkUtil::UTF8String ("Null property", TkUtil::Charset::UTF_8));
    }

    old_rep = m_geomProp;
    m_geomProp= prop;
    return old_rep;
}
/*----------------------------------------------------------------------------*/
void GeomEntity::
getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
#ifdef _DEBUG2
	std::cout<<"getRepresentation appelé pour "<<getName()<<std::endl;
#endif

    if (dr.getDisplayType()!= Utils::DisplayRepresentation::DISPLAY_GEOM)
        throw TkUtil::Exception("Invalid display type entity");

    GeomDisplayRepresentation* gdr = dynamic_cast<GeomDisplayRepresentation*>(&dr);

    auto add_representation = [&](const TopoDS_Shape& sh) { 
        OCCDisplayRepresentationBuilder builder(this, sh, gdr);
        builder.execute();
    };
    this->apply(add_representation);

	// applique le shrink sur la représentation
	Utils::Math::Point barycentre = getCenteredPosition();
	double shrink = dr.getShrink();
	std::vector<Utils::Math::Point>& pts = dr.getPoints();
	if (shrink != 1.0)
		for(unsigned int i=0;i<pts.size();i++)
			pts[i] = (barycentre + (pts[i] - barycentre) * shrink);
}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* GeomEntity::getDescription (bool alsoComputed) const
{
    return Services::DescriptionService::getDescription(this, alsoComputed);
}
/*----------------------------------------------------------------------------*/
GeomProperty::type GeomEntity::getGeomType ( ) const
{
	CHECK_NULL_PTR_ERROR (getGeomProperty ( ))
	return getGeomProperty ( )->getType ( );
}	// GeomEntity::getGeomType
/*----------------------------------------------------------------------------*/
void GeomEntity::getGroupsName (std::vector<std::string>& gn) const
{
    MGX_FORBIDDEN("getGroupsName est à redéfinir dans les classes dérivées");
}
/*----------------------------------------------------------------------------*/
void GeomEntity::getGroups(std::vector<Group::GroupEntity*>& grp) const
{
    MGX_FORBIDDEN("getGroups est à redéfinir dans les classes dérivées");
}
/*----------------------------------------------------------------------------*/
int GeomEntity::getNbGroups() const
{
    MGX_FORBIDDEN("getNbGroups est à redéfinir dans les classes dérivées");
    return 0;
}
/*----------------------------------------------------------------------------*/
double GeomEntity::getArea() const
{
	//std::cout<<"GeomEntity::getArea() pour "<<getName()<<std::endl;
	if (!m_computedAreaIsUpToDate){
		m_computedAreaIsUpToDate = true;
		//std::cout<<" => computeArea()...\n";
		m_computedArea = computeArea();
	}
	//std::cout<<"return m_computedArea = "<< m_computedArea<<std::endl;
	return m_computedArea;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
