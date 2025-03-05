/*----------------------------------------------------------------------------*/
/*
 * GeomRemoveImplementation.cpp
 *
 *  Created on: 10 janv. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/GeomRemoveImplementation.h"
#include "Geom/IncidentGeomEntitiesVisitor.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
#include <set>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomRemoveImplementation::
GeomRemoveImplementation(Internal::Context& c, std::vector<GeomEntity*>& es,
            bool propagateDown)
:GeomModificationBaseClass(c), m_propagate(propagateDown)
{
    m_entities_param.insert(m_entities_param.end(),es.begin(),es.end());
}
/*----------------------------------------------------------------------------*/
GeomRemoveImplementation::~GeomRemoveImplementation()
{}
/*----------------------------------------------------------------------------*/
void GeomRemoveImplementation::prePerform()
{
    init(m_entities_param);
}
/*----------------------------------------------------------------------------*/
void GeomRemoveImplementation::perform(std::vector<GeomEntity*>& res)
{
    //collection des éléments à supprimer.
    std::set<GeomEntity*> toRemove[4];

    //========================================================================
    // Pour chaque élément à supprimer, on doit supprimer les éléments
    // incidents de dimension supérieure
    //========================================================================
    GetUpIncidentGeomEntitiesVisitor v;
    for(unsigned int i=0;i<m_entities_param.size();i++){
        GeomEntity* ei = m_entities_param[i];

        //suppression de l'entité elle-même
        toRemove[ei->getDim()].insert(ei);

        ei->accept(v);
    }
    for (auto eii : v.get())
        toRemove[eii->getDim()].insert(eii);

    //========================================================================
    // Si m_propagate = true, on doit aussi supprimer les éléments incidents
    // de dimension inférieure sauf s'ils sont partagés avec des entités qui
    // ne sont pas supprimées.

    //========================================================================.

    if(m_propagate){
        std::set<GeomEntity*> candidatesToBeRemove[4];
        GetDownIncidentGeomEntitiesVisitor v;
        for(unsigned int i=0;i<m_entities_param.size();i++){
            GeomEntity* ei = m_entities_param[i];

            ei->accept(v);
        }
        for (auto eii : v.get())
            candidatesToBeRemove[eii->getDim()].insert(eii);

        // on regarde maintenant si effectivement ces entités peuvent être
        // supprimées

        // pour les surfaces
        std::set<GeomEntity*>::iterator it = candidatesToBeRemove[2].begin();
        for(;it!=candidatesToBeRemove[2].end();it++){
            //une face ne peut pas être retiré si elle est utilisée par
            //un volume qui est conservé.
            Surface* e = dynamic_cast<Surface*>(*it);
            auto vols = e->getVolumes();
            bool keep_e = false;
            for(unsigned int i=0;i<vols.size() && !keep_e;i++){
                GeomEntity* ei = vols[i];
                if(toRemove[3].find(ei)==toRemove[3].end())//ei est conservé
                    keep_e = true; //donc e aussi
            }
            if(!keep_e)
                toRemove[2].insert(e);
        }
        //pour les courbes
        it = candidatesToBeRemove[1].begin();
        for(;it!=candidatesToBeRemove[1].end();it++){
            //une courbe ne peut pas être retirée si elle est utilisée par
            //un volume ou une face qui est conservée.
            Curve* e = dynamic_cast<Curve*>(*it);
            auto surfs = e->getSurfaces();
            bool keep_e = false;
            for(unsigned int i=0;i<surfs.size() && !keep_e;i++){
                Surface* si = surfs[i];
                if(toRemove[2].find(si)==toRemove[2].end())//si est conservé
                    keep_e = true; //donc e aussi
                else {    
                    auto vols = si->getVolumes();    
                    for(unsigned int i=0;i<vols.size() && !keep_e;i++){
                        Volume* vi = vols[i];
                        if(toRemove[3].find(vi)==toRemove[3].end())//vi est conservé
                            keep_e = true; //donc e aussi
                    }
                }
            }
            if(!keep_e)
                toRemove[1].insert(e);
        }

        //pour les sommets
        it = candidatesToBeRemove[0].begin();
        for(;it!=candidatesToBeRemove[0].end();it++){
            //un sommet ne peut pas être retiré si il est utilisé par
            //un volume, une face ou une courbe qui est conservé.
            Vertex* e = dynamic_cast<Vertex*>(*it);
            auto curvs = e->getCurves();
            bool keep_e = false;
            for(unsigned int i=0;i<curvs.size() && !keep_e;i++){
                Curve* ci = curvs[i];
                if(toRemove[1].find(ci)==toRemove[1].end())//ci est conservé
                    keep_e = true; //donc e aussi
                else {    
                    auto surfs = ci->getSurfaces();
                    for(unsigned int i=0;i<surfs.size() && !keep_e;i++){
                        Surface* si = surfs[i];
                        if(toRemove[2].find(si)==toRemove[2].end())//si est conservé
                            keep_e = true; //donc e aussi
                        else {
                            auto vols = si->getVolumes();
                            for(unsigned int i=0;i<vols.size() && !keep_e;i++){
                                Volume* vi = vols[i];
                                if(toRemove[3].find(vi)==toRemove[3].end())//vi est conservé
                                    keep_e = true; //donc e aussi
                            }
                        }    
                    }
                }
            }
            if(!keep_e)
                toRemove[0].insert(e);
        }
    }
    //========================================================================
    // L'ensemble toRemove contient toutes les entités à supprimer. Pour
    // chacune de celles-ci, on doit maintenant prévenir les entités
    // incidentes ou adjacentes conservées de se mettre a jour
    //========================================================================

    //pour les volumes, seuls les surfaces doivent être maj (du fait du modèle
    // de connectivités pour les entités géométriques dans M3D)
    std::set<GeomEntity*>::iterator it = toRemove[3].begin();
    for(;it!=toRemove[3].end();it++){
        Volume* v = dynamic_cast<Volume*>(*it);
        auto surfs = v->getSurfaces();
        for(unsigned int i=0;i<surfs.size();i++){
            Surface* si = dynamic_cast<Surface*>(surfs[i]);
            if(toRemove[2].find(si)==toRemove[2].end())//si est conservé
                si->remove(v);
        }
    }
    //pour les surfaces, on doit vérifier courbes et volumes
    it = toRemove[2].begin();
    for(;it!=toRemove[2].end();it++){
        Surface* s = dynamic_cast<Surface*>(*it);
        auto curvs = s->getCurves();
        for(unsigned int i=0;i<curvs.size();i++){
            Curve* ci = dynamic_cast<Curve*>(curvs[i]);
            if(toRemove[1].find(ci)==toRemove[1].end())//ci est conservé
                ci->remove(s);
        }
        auto vols = s->getVolumes();
        for(unsigned int i=0;i<vols.size();i++){
            Volume* vi = dynamic_cast<Volume*>(vols[i]);
            if(toRemove[3].find(vi)==toRemove[3].end())//vi est conservé
                vi->remove(s);
        }
    }
    //pour les courbes, on doit vérifier sommets et surfaces
    it = toRemove[1].begin();
    for(;it!=toRemove[1].end();it++){
        Curve* c = dynamic_cast<Curve*>(*it);
        auto surfs = c->getSurfaces();
        for(unsigned int i=0;i<surfs.size();i++){
            Surface* si = dynamic_cast<Surface*>(surfs[i]);
            if(toRemove[2].find(si)==toRemove[2].end())//ei est conservé
                si->remove(c);
        }
        auto verts = c->getVertices();
        for(unsigned int i=0;i<verts.size();i++){
            Vertex* vi = dynamic_cast<Vertex*>(verts[i]);
            if(toRemove[0].find(vi)==toRemove[0].end())//ei est conservé
                vi->remove(c);
        }
    }
    //pour les sommets, on doit vérifier les courbes
    it = toRemove[0].begin();
    for(;it!=toRemove[0].end();it++){
        Vertex* v = dynamic_cast<Vertex*>(*it);
        auto curvs = v->getCurves();
        for(unsigned int i=0;i<curvs.size();i++){
            Curve* ci = dynamic_cast<Curve*>(curvs[i]);
            if(toRemove[1].find(ci)==toRemove[1].end())//ei est conservé
                ci->remove(v);
        }
    }

    m_removedEntities.insert(m_removedEntities.end(),toRemove[3].begin(),toRemove[3].end());
    m_removedEntities.insert(m_removedEntities.end(),toRemove[2].begin(),toRemove[2].end());
    m_removedEntities.insert(m_removedEntities.end(),toRemove[1].begin(),toRemove[1].end());
    m_removedEntities.insert(m_removedEntities.end(),toRemove[0].begin(),toRemove[0].end());
 }
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
