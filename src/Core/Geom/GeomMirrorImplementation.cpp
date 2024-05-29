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
    buildInitialSet(init_entities, false);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    checkValidity(init_entities);

    std::vector<GeomEntity*>::iterator it  = m_modifiedEntities.begin();
    std::vector<GeomEntity*>::iterator ite = m_modifiedEntities.end();
    for(;it!=ite;it++)
        mirrorSingle(*it);

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
void GeomMirrorImplementation::
    mirrorSingle(GeomEntity* e)
{
    //std::cout<<"GeomMirrorImplementation::mirrorSingle pour "<<e->getName()<<std::endl;
    std::vector<GeomRepresentation*> reps = e->getComputationalProperties();
    std::vector<GeomRepresentation*> new_reps;
    for (uint i=0; i<reps.size(); i++){
        GeomRepresentation* new_rep = reps[i]->clone();
        new_rep->mirror(m_plane);

        new_reps.push_back(new_rep);
    }

    e->setComputationalProperties(new_reps);
    e->setGeomProperty(new GeomProperty());
}
/*----------------------------------------------------------------------------*/
void GeomMirrorImplementation::
performUndo()
{
    for (uint i=0; i<m_undoableEntities.size(); i++){
        GeomRepresentation* rep = m_undoableEntities[i]->getComputationalProperty();
        rep->mirror(m_plane);
    }
}
/*----------------------------------------------------------------------------*/
void GeomMirrorImplementation::
performRedo()
{
    for (uint i=0; i<m_undoableEntities.size(); i++){
        GeomRepresentation* rep = m_undoableEntities[i]->getComputationalProperty();
        rep->mirror(m_plane);
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
