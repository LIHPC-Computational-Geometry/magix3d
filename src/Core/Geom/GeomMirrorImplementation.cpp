/*
 * GeomMirrorImplementation.cpp
 *
 *  Created on: 12/4/2016
 *      Author: Eric B
 */

/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
/*----------------------------------------------------------------------------*/
#include "Geom/GeomMirrorImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/CommandGeomCopy.h"
/*----------------------------------------------------------------------------*/
#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Ax2.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomMirrorImplementation::
GeomMirrorImplementation(Internal::Context& c,
        std::vector<GeomEntity*>& es,
		const Utils::Math::Plane* plane)
: GeomModificationBaseClass(c), m_plane(*plane)
{
    init(es);
}
/*----------------------------------------------------------------------------*/
GeomMirrorImplementation::
GeomMirrorImplementation(Internal::Context& c,
		Geom::CommandGeomCopy* cmd,
		const Utils::Math::Plane* plane)
: GeomModificationBaseClass(c), m_plane(*plane)
{
    init(cmd);
}
/*----------------------------------------------------------------------------*/
GeomMirrorImplementation::
GeomMirrorImplementation(Internal::Context& c,
		const Utils::Math::Plane* plane)
: GeomModificationBaseClass(c), m_plane(*plane)
{
	initWithAll();
}
/*----------------------------------------------------------------------------*/
GeomMirrorImplementation::~GeomMirrorImplementation()
{
}
/*----------------------------------------------------------------------------*/
void GeomMirrorImplementation::prePerform()
{
	if (m_buildEntitiesCmd)
		init(m_buildEntitiesCmd->getNewEntities());
}
/*----------------------------------------------------------------------------*/
void GeomMirrorImplementation::perform(std::vector<GeomEntity*>& res)
{
    //std::cout<<"GeomMirrorImplementation::perform"<<std::endl;
    m_modifiedEntities.clear();

    // on sépare les entités à symétriser, ainsi que les entités incidentes
    // de dimension inférieure en différents ensembles selon leur dimension.
    std::set<GeomEntity*> init_entities;
    buildInitialSet(init_entities);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    checkValidity(init_entities);

    std::vector<GeomEntity*>::iterator it  = m_modifiedEntities.begin();
    std::vector<GeomEntity*>::iterator ite = m_modifiedEntities.end();
    for(;it!=ite;it++)
        mirrorSingle(*it);

    // on force l'ajout des dépendances de dimension inférieure
    buildInitialSet(init_entities);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.clear();
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    m_movedEntities.insert(m_movedEntities.end(),m_modifiedEntities.begin(),m_modifiedEntities.end());
}
/*----------------------------------------------------------------------------*/
void GeomMirrorImplementation::
    mirrorSingle(GeomEntity* e)
{
    //std::cout<<"GeomMirrorImplementation::mirrorSingle pour "<<e->getName()<<std::endl;
    std::vector<TopoDS_Shape> reps = e->getOCCShapes();
    std::vector<TopoDS_Shape> new_reps;
    for (uint i=0; i<reps.size(); i++){
        new_reps.push_back(mirror(reps[i], m_plane));
    }

    e->setOCCShapes(new_reps);
    e->setGeomProperty(new GeomProperty());
}
/*----------------------------------------------------------------------------*/
TopoDS_Shape GeomMirrorImplementation::
mirror(const TopoDS_Shape& shape, const Utils::Math::Plane& plane) const
{
	gp_Trsf T;
    Utils::Math::Point plane_pnt = plane.getPoint();
    Utils::Math::Vector plane_vec = plane.getNormal();

    gp_Ax2 A2(gp_Pnt(plane_pnt.getX(), plane_pnt.getY(), plane_pnt.getZ()),
    		gp_Dir(plane_vec.getX(), plane_vec.getY(), plane_vec.getZ()));
    T.SetMirror (A2);
    BRepBuilderAPI_Transform trans(T);

    trans.Perform(shape);

    if(!trans.IsDone())
    	throw TkUtil::Exception(TkUtil::UTF8String ("Echec d'une symétrie!!", TkUtil::Charset::UTF_8));

    //on stocke le résultat de la transformation (maj de la shape interne)
    return trans.Shape();
}
/*----------------------------------------------------------------------------*/
void GeomMirrorImplementation::
performUndo()
{
    for (uint i=0; i<m_undoableEntities.size(); i++)
        for (auto rep : m_undoableEntities[i]->getOCCShapes())
            rep = mirror(rep, m_plane);
}
/*----------------------------------------------------------------------------*/
void GeomMirrorImplementation::
performRedo()
{
    performUndo();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
