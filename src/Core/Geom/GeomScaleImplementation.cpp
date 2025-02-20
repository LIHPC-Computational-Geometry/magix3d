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
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
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
        const double factorZ,
        const Point& pcentre)
: GeomModificationBaseClass(c)
, m_factor(0)
, m_isHomogene(false)
, m_factorX(factorX)
, m_factorY(factorY)
, m_factorZ(factorZ)
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
        const double factorX,
        const double factorY,
        const double factorZ,
        const Point& pcentre)
: GeomModificationBaseClass(c)
, m_factor(0)
, m_isHomogene(false)
, m_factorX(factorX)
, m_factorY(factorY)
, m_factorZ(factorZ)
, m_center(pcentre)
{
    init(cmd);
}
/*----------------------------------------------------------------------------*/
GeomScaleImplementation::
GeomScaleImplementation(Internal::Context& c,
        const double factorX,
        const double factorY,
        const double factorZ,
        const Point& pcentre)
: GeomModificationBaseClass(c)
, m_factor(0)
, m_isHomogene(false)
, m_factorX(factorX)
, m_factorY(factorY)
, m_factorZ(factorZ)
, m_center(pcentre)
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
    buildInitialSet(init_entities);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    checkValidity(init_entities);

    std::vector<GeomEntity*>::iterator it  = m_modifiedEntities.begin();
    std::vector<GeomEntity*>::iterator ite = m_modifiedEntities.end();
    for(;it!=ite;it++)
        scaleSingle(*it);

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

    // on force l'ajout des dépendances de dimension inférieure
    buildInitialSet(init_entities);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.clear();
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    m_movedEntities.insert(m_movedEntities.end(),m_modifiedEntities.begin(),m_modifiedEntities.end());
}
/*----------------------------------------------------------------------------*/
void GeomScaleImplementation::scaleSingle(GeomEntity* e)
{
    //std::cout<<"GeomScaleImplementation::scaleSingle pour "<<e->getName()<<std::endl;
    if (m_isHomogene) {
        auto _scale = [&](const TopoDS_Shape& sh) { return scale(sh, m_factor, m_center); };
        e->applyAndReturn(_scale);
    } else {
        auto _scale = [&](const TopoDS_Shape& sh) { return scale(sh, m_factorX, m_factorY, m_factorZ, m_center); };
        e->applyAndReturn(_scale);
    }
    e->setGeomProperty(new GeomProperty());
}
/*----------------------------------------------------------------------------*/
TopoDS_Shape GeomScaleImplementation::
scale(const TopoDS_Shape& shape, const double F, const Point& center) const
{
    Bnd_Box box;
    box.SetGap(Utils::Math::MgxNumeric::mgxDoubleEpsilon);
    BRepBndLib::Add(shape, box);

    double xmin,ymin,zmin,xmax,ymax,zmax;
    box.Get(xmin,ymin,zmin,xmax,ymax,zmax);

    gp_Trsf T;
    T.SetScale(gp_Pnt(center.getX(), center.getY(), center.getZ()),F);
    BRepBuilderAPI_Transform scaling(T);

    //on effectue l'homotéthie
    scaling.Perform(shape);

    if(!scaling.IsDone())
    	throw TkUtil::Exception(TkUtil::UTF8String ("Echec d'une homothétie!!", TkUtil::Charset::UTF_8));

    //on stocke le résultat de la translation (maj de la shape interne)
    return scaling.Shape();
}
/*----------------------------------------------------------------------------*/
TopoDS_Shape GeomScaleImplementation::
scale(const TopoDS_Shape& shape, const double factorX, const double factorY, const double factorZ, const Point& center) const
{
    Bnd_Box box;
    box.SetGap(Utils::Math::MgxNumeric::mgxDoubleEpsilon);
    BRepBndLib::Add(shape, box);

    double xmin,ymin,zmin,xmax,ymax,zmax;
    box.Get(xmin,ymin,zmin,xmax,ymax,zmax);

    gp_GTrsf GT;
    GT.SetValue(1, 1, factorX);
    GT.SetValue(2, 2, factorY);
    GT.SetValue(3, 3, factorZ);
	GT.SetValue(1, 4, (1-factorX) * center.getX());
	GT.SetValue(2, 4, (1-factorY) * center.getY());
    GT.SetValue(3, 4, (1-factorZ) * center.getZ());

    BRepBuilderAPI_GTransform scaling(GT);

    //on effectue l'homothétie
    scaling.Perform(shape);

    if(!scaling.IsDone())
    	throw TkUtil::Exception(TkUtil::UTF8String ("Echec d'une homothétie!!", TkUtil::Charset::UTF_8));

    //on stocke le résultat de la translation (maj de la shape interne)
    return scaling.Shape();
}
/*----------------------------------------------------------------------------*/
void GeomScaleImplementation::
performUndo()
{
    for (uint i=0; i<m_undoableEntities.size(); i++) {
        if (m_isHomogene) {
            auto undo = [&](const TopoDS_Shape& sh) { return scale(sh, 1.0/m_factor, m_center); };
            m_undoableEntities[i]->applyAndReturn(undo);
        } else {
            auto undo = [&](const TopoDS_Shape& sh) { return scale(sh, 1.0/m_factorX, 1.0/m_factorY, 1.0/m_factorZ, m_center); };
            m_undoableEntities[i]->applyAndReturn(undo);
        }
    }
}
/*----------------------------------------------------------------------------*/
void GeomScaleImplementation::
performRedo()
{
    for (uint i=0; i<m_undoableEntities.size(); i++) {
        if (m_isHomogene) {
            auto redo = [&](const TopoDS_Shape& sh) { return scale(sh, m_factor, m_center); };
            m_undoableEntities[i]->applyAndReturn(redo);
        } else {
            auto redo = [&](const TopoDS_Shape& sh) { return scale(sh,  m_factorX, m_factorY, m_factor, m_center); };
            m_undoableEntities[i]->applyAndReturn(redo);
        }
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

