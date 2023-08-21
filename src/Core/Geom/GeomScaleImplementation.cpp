/*
 * GeomScaleImplementation.cpp
 *
 *  Created on: 19 juin 2013
 *      Author: ledouxf
 */

/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <set>
/*----------------------------------------------------------------------------*/
#include "Geom/GeomScaleImplementation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/CommandGeomCopy.h"
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomScaleImplementation::
GeomScaleImplementation(Internal::Context& c,
        std::vector<GeomEntity*>& es,
        const double factor,
        const Point& pcentre)
: GeomModificationBaseClass(c)
, m_factor(factor)
, m_isHomogene(true)
, m_factorX(0)
, m_factorY(0)
, m_factorZ(0)
, m_center(pcentre)
{
    m_modifiedEntities.insert(m_modifiedEntities.end(),es.begin(),es.end());

    // Mise à jour des connectivés de références
    init(m_modifiedEntities);
}
/*----------------------------------------------------------------------------*/
GeomScaleImplementation::
GeomScaleImplementation(Internal::Context& c,
		Geom::CommandGeomCopy* cmd,
        const double factor,
        const Point& pcentre)
: GeomModificationBaseClass(c)
, m_factor(factor)
, m_isHomogene(true)
, m_factorX(0)
, m_factorY(0)
, m_factorZ(0)
, m_center(pcentre)
{
    // Mise à jour des connectivés de références
    init(cmd);
}
/*----------------------------------------------------------------------------*/
GeomScaleImplementation::
GeomScaleImplementation(Internal::Context& c,
        const double factor,
        const Point& pcentre)
: GeomModificationBaseClass(c)
, m_factor(factor)
, m_isHomogene(true)
, m_factorX(0)
, m_factorY(0)
, m_factorZ(0)
, m_center(pcentre)
{
	initWithAll();
    m_modifiedEntities.insert(m_modifiedEntities.end(),m_init_entities.begin(),m_init_entities.end());
}
/*----------------------------------------------------------------------------*/
GeomScaleImplementation::
GeomScaleImplementation(Internal::Context& c,
        std::vector<GeomEntity*>& es,
        const double factorX,
        const double factorY,
        const double factorZ)
: GeomModificationBaseClass(c)
, m_factor(0)
, m_isHomogene(false)
, m_factorX(factorX)
, m_factorY(factorY)
, m_factorZ(factorZ)
, m_center()
{
    m_modifiedEntities.insert(m_modifiedEntities.end(),es.begin(),es.end());

    // Mise à jour des connectivés de références
    init(m_modifiedEntities);
}
/*----------------------------------------------------------------------------*/
GeomScaleImplementation::
GeomScaleImplementation(Internal::Context& c,
		Geom::CommandGeomCopy* cmd,
        const double factorX,
        const double factorY,
        const double factorZ)
: GeomModificationBaseClass(c)
, m_factor(0)
, m_isHomogene(false)
, m_factorX(factorX)
, m_factorY(factorY)
, m_factorZ(factorZ)
, m_center()
{
    init(cmd);
}
/*----------------------------------------------------------------------------*/
GeomScaleImplementation::
GeomScaleImplementation(Internal::Context& c,
        const double factorX,
        const double factorY,
        const double factorZ)
: GeomModificationBaseClass(c)
, m_factor(0)
, m_isHomogene(false)
, m_factorX(factorX)
, m_factorY(factorY)
, m_factorZ(factorZ)
, m_center()
{
	initWithAll();
    m_modifiedEntities.insert(m_modifiedEntities.end(),m_init_entities.begin(),m_init_entities.end());
}
/*----------------------------------------------------------------------------*/
GeomScaleImplementation::~GeomScaleImplementation()
{
}
/*----------------------------------------------------------------------------*/
void GeomScaleImplementation::prePerform()
{
	if (m_buildEntitiesCmd){
		std::vector<GeomEntity*>& es = m_buildEntitiesCmd->getNewEntities();
		m_modifiedEntities.insert(m_modifiedEntities.end(),es.begin(),es.end());
		init(m_modifiedEntities);
	}
}
/*----------------------------------------------------------------------------*/
void GeomScaleImplementation::perform(std::vector<GeomEntity*>& res)
{
    //std::cout<<"GeomScaleImplementation::perform"<<std::endl;
    m_modifiedEntities.clear();

    // on sépare les entités à scaler, ainsi que les entités incidentes
    // de dimension inférieure en différents ensembles selon leur dimension.
    std::set<GeomEntity*> init_entities;
    buildInitialSet(init_entities, false);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    checkValidity(init_entities);

    std::vector<GeomEntity*>::iterator it  = m_modifiedEntities.begin();
    std::vector<GeomEntity*>::iterator ite = m_modifiedEntities.end();
    for(;it!=ite;it++)
        scaleSingle(*it);

    for(it  = m_modifiedEntities.begin();it!=ite;it++)
        if (!(*it)->needLowerDimensionalEntityModification())
            m_undoableEntities.push_back(*it);

    // traitement spécifique pour les courbes composites
	for(std::list<GeomEntity*>::iterator it = m_ref_entities[1].begin();
			it!=m_ref_entities[1].end();it++){
		GeomEntity* entity = *it;
		std::vector<Vertex*> vertices;
		entity->get(vertices);
		Curve* crv = dynamic_cast<Curve*>(entity);
		if (crv && vertices.size())
			crv->computeParams(vertices[0]->getPoint());
	}

    // on force l'ajout des dépendances de dimension inférieur, même pour le cas facétisé
    // c'est nécessaire pour identifier qu'il y a eu modifications des courbes facétisées
    buildInitialSet(init_entities, true);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.clear();
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    m_movedEntities.insert(m_movedEntities.end(),m_modifiedEntities.begin(),m_modifiedEntities.end());
}
/*----------------------------------------------------------------------------*/
void GeomScaleImplementation::scaleSingle(GeomEntity* e)
{
    //std::cout<<"GeomScaleImplementation::scaleSingle pour "<<e->getName()<<std::endl;
    std::vector<GeomRepresentation*> reps = e->getComputationalProperties();
    std::vector<GeomRepresentation*> new_reps;
    for (uint i=0; i<reps.size(); i++){
        GeomRepresentation* new_rep = reps[i]->clone();
        if (m_isHomogene)
            new_rep->scale(m_factor, m_center);
        else
            new_rep->scale(m_factorX, m_factorY, m_factorZ);
        new_reps.push_back(new_rep);
    }

    e->setComputationalProperties(new_reps);
    e->setGeomProperty(new GeomProperty());
}
/*----------------------------------------------------------------------------*/
void GeomScaleImplementation::
performUndo()
{
    for (uint i=0; i<m_undoableEntities.size(); i++){
        GeomRepresentation* rep = m_undoableEntities[i]->getComputationalProperty();
        if (m_isHomogene)
            rep->scale(1.0/m_factor, m_center);
        else
            rep->scale(1.0/m_factorX, 1.0/m_factorY, 1.0/m_factorZ);
    }
}
/*----------------------------------------------------------------------------*/
void GeomScaleImplementation::
performRedo()
{
    for (uint i=0; i<m_undoableEntities.size(); i++){
        GeomRepresentation* rep = m_undoableEntities[i]->getComputationalProperty();
        if (m_isHomogene)
            rep->scale(m_factor, m_center);
        else
            rep->scale(m_factorX, m_factorY, m_factorZ);
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
