/*----------------------------------------------------------------------------*/
#include "Geom/GeomRotationImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/CommandGeomCopy.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Ax1.hxx>
/*----------------------------------------------------------------------------*/
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomRotationImplementation::
GeomRotationImplementation(Internal::Context& c,
		std::vector<GeomEntity*> entities,
        const Utils::Math::Point& axis1, const Utils::Math::Point& axis2,
        const double& angle)
: GeomModificationBaseClass(c)
, m_axis1(axis1), m_axis2(axis2)
, m_angle((double)(angle*M_PI/180.0))
{
	init(entities);
}
/*----------------------------------------------------------------------------*/
GeomRotationImplementation::
GeomRotationImplementation(Internal::Context& c,
		Geom::CommandGeomCopy* cmd,
        const Utils::Math::Point& axis1, const Utils::Math::Point& axis2,
        const double& angle)
: GeomModificationBaseClass(c)
, m_axis1(axis1), m_axis2(axis2)
, m_angle((double)(angle*M_PI/180.0))
{
	init(cmd);
}
/*----------------------------------------------------------------------------*/
GeomRotationImplementation::
GeomRotationImplementation(Internal::Context& c,
        const Utils::Math::Point& axis1, const Utils::Math::Point& axis2,
        const double& angle)
: GeomModificationBaseClass(c)
, m_axis1(axis1), m_axis2(axis2)
, m_angle((double)(angle*M_PI/180.0))
{
	initWithAll();
}
/*----------------------------------------------------------------------------*/
GeomRotationImplementation::~GeomRotationImplementation()
{}
/*----------------------------------------------------------------------------*/
void GeomRotationImplementation::prePerform()
{
	if (m_buildEntitiesCmd)
		init(m_buildEntitiesCmd->getNewEntities());
}
/*----------------------------------------------------------------------------*/
void GeomRotationImplementation::perform(std::vector<GeomEntity*>& res)
{
    m_modifiedEntities.clear();

    // on sépare les entités à révolutionner ;-) ainsi que les entités incidentes
    // de dimension inférieure en trois ensembles selon leur dimension.
    Utils::EntitySet<GeomEntity*> init_entities(Utils::Entity::compareEntity);
    buildInitialSet(init_entities);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    checkValidity(init_entities);

    std::vector<GeomEntity*>::iterator it  = m_modifiedEntities.begin();
    std::vector<GeomEntity*>::iterator ite = m_modifiedEntities.end();
    for(;it!=ite;it++)
        makeRevol(*it);

    // on force l'ajout des dépendances de dimension inférieur
    buildInitialSet(init_entities);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.clear();
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    m_movedEntities.insert(m_movedEntities.end(),m_modifiedEntities.begin(),m_modifiedEntities.end());
}
/*----------------------------------------------------------------------------*/
void GeomRotationImplementation::
makeRevol(GeomEntity* e)
{
    auto revol = [&](const TopoDS_Shape& sh) { return rotate(sh, m_axis1, m_axis2, m_angle); };
    e->applyAndReturn(revol);
    e->setGeomProperty(new GeomProperty());
}
/*----------------------------------------------------------------------------*/
TopoDS_Shape GeomRotationImplementation::
rotate(const TopoDS_Shape& shape, const Utils::Math::Point& P1, const Utils::Math::Point& P2, double Angle) const
{
    gp_Trsf T;
    gp_Pnt p1(P1.getX(),P1.getY(),P1.getZ());

    gp_Dir dir( P2.getX()-P1.getX(),
                P2.getY()-P1.getY(),
                P2.getZ()-P1.getZ());

    gp_Ax1 axis(p1,dir);
    T.SetRotation(axis,Angle);
    BRepBuilderAPI_Transform rotat(T);
    //on effectue la rotation
    rotat.Perform(shape);

    if(!rotat.IsDone())
        throw TkUtil::Exception("Echec d'une rotation!!");

    //on stocke le résultat de la translation (maj de la shape interne)
    return rotat.Shape();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
