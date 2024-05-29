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
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/GeomRotationImplementation.h"
#include "Geom/CommandGeomCopy.h"
/*----------------------------------------------------------------------------*/
//inclusion de fichiers en-tête d'Open Cascade
#include <TopoDS_Shape.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
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
    buildInitialSet(init_entities, false);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    checkValidity(init_entities);

    std::vector<GeomEntity*>::iterator it  = m_modifiedEntities.begin();
    std::vector<GeomEntity*>::iterator ite = m_modifiedEntities.end();
    for(;it!=ite;it++)
        makeRevol(*it);

    for(it  = m_modifiedEntities.begin();it!=ite;it++)
        if (!(*it)->needLowerDimensionalEntityModification())
            m_undoableEntities.push_back(*it);

    // on force l'ajout des dépendances de dimension inférieur, même pour le cas facétisé
    // c'est nécessaire pour identifier qu'il y a eu modifications des courbes facétisées
    buildInitialSet(init_entities, true);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.clear();
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    m_movedEntities.insert(m_movedEntities.end(),m_modifiedEntities.begin(),m_modifiedEntities.end());
}
/*----------------------------------------------------------------------------*/
void GeomRotationImplementation::
makeRevol(GeomEntity* e)
{
    std::vector<GeomRepresentation*> reps = e->getComputationalProperties();
    std::vector<GeomRepresentation*> new_reps;
    for (uint i=0; i<reps.size(); i++){
        GeomRepresentation* new_rep = reps[i]->clone();
        new_rep->rotate(m_axis1,m_axis2,m_angle);

        new_reps.push_back(new_rep);
    }

    e->setComputationalProperties(new_reps);
    e->setGeomProperty(new GeomProperty());
}
/*----------------------------------------------------------------------------*/
void GeomRotationImplementation::
performUndo()
{
    for (uint i=0; i<m_undoableEntities.size(); i++){
        GeomRepresentation* rep = m_undoableEntities[i]->getComputationalProperty();
        rep->rotate(m_axis1,m_axis2,-m_angle);
    }
}
/*----------------------------------------------------------------------------*/
void GeomRotationImplementation::
performRedo()
{
    for (uint i=0; i<m_undoableEntities.size(); i++){
        GeomRepresentation* rep = m_undoableEntities[i]->getComputationalProperty();
        rep->rotate(m_axis1,m_axis2,m_angle);
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
