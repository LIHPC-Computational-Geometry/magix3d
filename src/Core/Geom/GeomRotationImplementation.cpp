/*
 * GeomRotationImplementation.cpp
 *
 *  Created on: 27 sept. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <set>
/*----------------------------------------------------------------------------*/
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/EntityFactory.h"
#include "Geom/GeomRotationImplementation.h"
#include "Geom/CommandGeomCopy.h"
/*----------------------------------------------------------------------------*/
//inclusion de fichiers en-tête d'Open Cascade
#include <TopoDS_Shape.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Ax1.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRepTools.hxx>
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
    std::set<GeomEntity*> init_entities;
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
    std::vector<TopoDS_Shape> reps = e->getOCCShapes();
    std::vector<TopoDS_Shape> new_reps;
    for (uint i=0; i<reps.size(); i++){
        new_reps.push_back(rotate(reps[i], m_axis1, m_axis2, m_angle));
    }

    e->setOCCShapes(new_reps);
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
void GeomRotationImplementation::
performUndo()
{
    for (uint i=0; i<m_undoableEntities.size(); i++)
        for (auto rep : m_undoableEntities[i]->getOCCShapes())
            rep = rotate(rep, m_axis1, m_axis2, -m_angle);
}
/*----------------------------------------------------------------------------*/
void GeomRotationImplementation::
performRedo()
{
    for (uint i=0; i<m_undoableEntities.size(); i++)
        for (auto rep : m_undoableEntities[i]->getOCCShapes())
            rep = rotate(rep, m_axis1, m_axis2, m_angle);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
