/*----------------------------------------------------------------------------*/
/** \file GeomTranslateImplementation.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 10/12/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <list>
#include <set>
/*----------------------------------------------------------------------------*/
#include "Geom/GeomTranslateImplementation.h"
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
GeomTranslateImplementation::
GeomTranslateImplementation(Internal::Context& c,
        std::vector<GeomEntity*>& es,
		const Utils::Math::Vector& dv)
: GeomModificationBaseClass(c), m_dv(dv)
{
    init(es);
}
/*----------------------------------------------------------------------------*/
GeomTranslateImplementation::
GeomTranslateImplementation(Internal::Context& c,
		Geom::CommandGeomCopy* cmd,
		const Utils::Math::Vector& dv)
: GeomModificationBaseClass(c), m_dv(dv)
{
    init(cmd);
}
/*----------------------------------------------------------------------------*/
GeomTranslateImplementation::
GeomTranslateImplementation(Internal::Context& c,
		const Utils::Math::Vector& dv)
: GeomModificationBaseClass(c), m_dv(dv)
{
    initWithAll();
}
/*----------------------------------------------------------------------------*/
GeomTranslateImplementation::~GeomTranslateImplementation()
{
}
/*----------------------------------------------------------------------------*/
void GeomTranslateImplementation::prePerform()
{
	if (m_buildEntitiesCmd)
		init(m_buildEntitiesCmd->getNewEntities());
}
/*----------------------------------------------------------------------------*/
void GeomTranslateImplementation::perform(std::vector<GeomEntity*>& res)
{
    //std::cout<<"GeomTranslateImplementation::perform"<<std::endl;
    m_modifiedEntities.clear();

    // on sépare les entités à translater, ainsi que les entités incidentes
    // de dimension inférieure en différents ensembles selon leur dimension.
    std::set<GeomEntity*> init_entities;
    buildInitialSet(init_entities, false);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    checkValidity(init_entities);

    std::vector<GeomEntity*>::iterator it  = m_modifiedEntities.begin();
    std::vector<GeomEntity*>::iterator ite = m_modifiedEntities.end();
    for(;it!=ite;it++)
        translateSingle(*it);

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
void GeomTranslateImplementation::
translateSingle(GeomEntity* e)
{
    //std::cout<<"GeomTranslateImplementation::translateSingle pour "<<e->getName()<<std::endl;
    std::vector<GeomRepresentation*> reps = e->getComputationalProperties();
    std::vector<GeomRepresentation*> new_reps;
    for (uint i=0; i<reps.size(); i++){
        GeomRepresentation* new_rep = reps[i]->clone();
        new_rep->translate(m_dv);

        new_reps.push_back(new_rep);
    }

    e->setComputationalProperties(new_reps);
    e->setGeomProperty(new GeomProperty());
}
/*----------------------------------------------------------------------------*/
void GeomTranslateImplementation::
performUndo()
{
    Utils::Math::Vector dv_inv(-m_dv.getX(), -m_dv.getY(), -m_dv.getZ());
    for (uint i=0; i<m_undoableEntities.size(); i++){
        GeomRepresentation* rep = m_undoableEntities[i]->getComputationalProperty();
        rep->translate(dv_inv);
    }
}
/*----------------------------------------------------------------------------*/
void GeomTranslateImplementation::
performRedo()
{
    for (uint i=0; i<m_undoableEntities.size(); i++){
        GeomRepresentation* rep = m_undoableEntities[i]->getComputationalProperty();
        rep->translate(m_dv);
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
