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
#include <BRepBuilderAPI_Transform.hxx>
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
    buildInitialSet(init_entities);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    checkValidity(init_entities);

    std::vector<GeomEntity*>::iterator it  = m_modifiedEntities.begin();
    std::vector<GeomEntity*>::iterator ite = m_modifiedEntities.end();
    for(;it!=ite;it++)
        translateSingle(*it);

    // on force l'ajout des dépendances de dimension inférieure
    buildInitialSet(init_entities);
    //maintenant on met a jour les entites de references
    m_modifiedEntities.clear();
    m_modifiedEntities.insert(m_modifiedEntities.end(),init_entities.begin(),init_entities.end());

    m_movedEntities.insert(m_movedEntities.end(),m_modifiedEntities.begin(),m_modifiedEntities.end());
}
/*----------------------------------------------------------------------------*/
void GeomTranslateImplementation::
translateSingle(GeomEntity* e)
{
    auto trans = [&](const TopoDS_Shape& sh) { return translate(sh, m_dv); };
    e->applyAndReturn(trans);
    e->setGeomProperty(new GeomProperty());
}
/*----------------------------------------------------------------------------*/
TopoDS_Shape GeomTranslateImplementation::
translate(const TopoDS_Shape& shape, const Utils::Math::Vector& V)
{
    gp_Trsf T;
    gp_Vec v(V.getX(),V.getY(),V.getZ());
    T.SetTranslation(v);
    BRepBuilderAPI_Transform translat(T);
    //on effectue la translation
    translat.Perform(shape);

    if(!translat.IsDone())
        throw TkUtil::Exception("Echec d'une translation!!");

    //on stocke le résultat de la translation (maj de la shape interne)
    return translat.Shape();
}
/*----------------------------------------------------------------------------*/
void GeomTranslateImplementation::
performUndo()
{
    Utils::Math::Vector dv_inv(-m_dv.getX(), -m_dv.getY(), -m_dv.getZ());
    auto undo = [&](const TopoDS_Shape& sh) { return translate(sh, dv_inv); };
    for (uint i=0; i<m_undoableEntities.size(); i++)
        m_undoableEntities[i]->applyAndReturn(undo);
}
/*----------------------------------------------------------------------------*/
void GeomTranslateImplementation::
performRedo()
{
    auto redo = [&](const TopoDS_Shape& sh) { return translate(sh, m_dv); };
    for (uint i=0; i<m_undoableEntities.size(); i++)
        m_undoableEntities[i]->applyAndReturn(redo);
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
