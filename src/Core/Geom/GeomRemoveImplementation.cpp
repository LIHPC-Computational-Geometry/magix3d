/*----------------------------------------------------------------------------*/
/*
 * GeomRemoveImplementation.cpp
 *
 *  Created on: 10 janv. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include <Geom/GeomRemoveImplementation.h>
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
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
    for(unsigned int i=0;i<m_entities_param.size();i++){
        GeomEntity* ei = m_entities_param[i];

        //suppression de l'entité elle-même
        toRemove[ei->getDim()].insert(ei);

        if(ei->getDim()<3){
            std::vector<Volume*> adj;
            ei->get(adj);
            toRemove[3].insert(adj.begin(),adj.end());
        }

        if(ei->getDim()<2){
            std::vector<Surface*> adj;
            ei->get(adj);
            toRemove[2].insert(adj.begin(),adj.end());
        }

        if(ei->getDim()<1){
            std::vector<Curve*> adj;
            ei->get(adj);
            toRemove[1].insert(adj.begin(),adj.end());
        }
    }
    //========================================================================
    // Si m_propagate = true, on doit aussi supprimer les éléments incidents
    // de dimension inférieure sauf s'ils sont partagés avec des entités qui
    // ne sont pas supprimées.

    //========================================================================.

    if(m_propagate){
        std::set<GeomEntity*> candidatesToBeRemove[4];
        for(unsigned int i=0;i<m_entities_param.size();i++){
            GeomEntity* ei = m_entities_param[i];

            if(ei->getDim()>0){
                std::vector<Vertex*> adj;
                ei->get(adj);
                candidatesToBeRemove[0].insert(adj.begin(),adj.end());
            }
            if(ei->getDim()>1){
                std::vector<Curve*> adj;
                ei->get(adj);
                candidatesToBeRemove[1].insert(adj.begin(),adj.end());
            }
            if(ei->getDim()>2){
                std::vector<Surface*> adj;
                ei->get(adj);
                candidatesToBeRemove[2].insert(adj.begin(),adj.end());
            }
        }
        // on regarde maintenant si effectivement ces entités peuvent être
        // supprimées

        // pour les surfaces
        std::set<GeomEntity*>::iterator it = candidatesToBeRemove[2].begin();
        for(;it!=candidatesToBeRemove[2].end();it++){
            //une face ne peut pas être retiré si elle est utilisée par
            //un volume qui est conservé.
            GeomEntity* e = *it;
            std::vector<Volume*> vols;
            e->get(vols);
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
            GeomEntity* e = *it;
            std::vector<Surface*> surfs;
            e->get(surfs);
            bool keep_e = false;
            for(unsigned int i=0;i<surfs.size() && !keep_e;i++){
                GeomEntity* ei = surfs[i];
                if(toRemove[2].find(ei)==toRemove[2].end())//ei est conservé
                    keep_e = true; //donc e aussi
            }
            std::vector<Volume*> vols;
            e->get(vols);
            for(unsigned int i=0;i<vols.size() && !keep_e;i++){
                GeomEntity* ei = vols[i];
                if(toRemove[3].find(ei)==toRemove[3].end())//ei est conservé
                    keep_e = true; //donc e aussi
            }
            if(!keep_e)
                toRemove[1].insert(e);
        }

        //pour les sommets
        it = candidatesToBeRemove[0].begin();
        for(;it!=candidatesToBeRemove[0].end();it++){
            //un sommet ne peut pas être retiré si il est utilisé par
            //un volume, une face ou une courbe qui est conservé.
            GeomEntity* e = *it;
            std::vector<Curve*> curvs;
            e->get(curvs);
            bool keep_e = false;
            for(unsigned int i=0;i<curvs.size() && !keep_e;i++){
                GeomEntity* ei = curvs[i];
                if(toRemove[1].find(ei)==toRemove[1].end())//ei est conservé
                    keep_e = true; //donc e aussi
            }

            std::vector<Surface*> surfs;
            e->get(surfs);
            for(unsigned int i=0;i<surfs.size() && !keep_e;i++){
                GeomEntity* ei = surfs[i];
                if(toRemove[2].find(ei)==toRemove[2].end())//ei est conservé
                    keep_e = true; //donc e aussi
            }
            std::vector<Volume*> vols;
            e->get(vols);
            for(unsigned int i=0;i<vols.size() && !keep_e;i++){
                GeomEntity* ei = vols[i];
                if(toRemove[3].find(ei)==toRemove[3].end())//ei est conservé
                    keep_e = true; //donc e aussi
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
        GeomEntity* e = *it;
        std::vector<Surface*> surfs;
        e->get(surfs);
        for(unsigned int i=0;i<surfs.size();i++){
            GeomEntity* ei = surfs[i];
            if(toRemove[2].find(ei)==toRemove[2].end())//ei est conservé
                ei->remove(e);
        }
    }
    //pour les surfaces, on doit vérifier courbes et volumes
    it = toRemove[2].begin();
    for(;it!=toRemove[2].end();it++){
        GeomEntity* e = *it;
        std::vector<Curve*> curvs;
        e->get(curvs);
        for(unsigned int i=0;i<curvs.size();i++){
            GeomEntity* ei = curvs[i];
            if(toRemove[1].find(ei)==toRemove[1].end())//ei est conservé
                ei->remove(e);
        }
        std::vector<Volume*> vols;
        e->get(vols);
        for(unsigned int i=0;i<vols.size();i++){
            GeomEntity* ei = vols[i];
            if(toRemove[3].find(ei)==toRemove[3].end())//ei est conservé
                ei->remove(e);
        }
    }
    //pour les courbes, on doit vérifier sommets et surfaces
    it = toRemove[1].begin();
    for(;it!=toRemove[1].end();it++){
        GeomEntity* e = *it;
        std::vector<Surface*> surfs;
        e->get(surfs);
        for(unsigned int i=0;i<surfs.size();i++){
            GeomEntity* ei = surfs[i];
            if(toRemove[2].find(ei)==toRemove[2].end())//ei est conservé
                ei->remove(e);
        }
        std::vector<Vertex*> verts;
        e->get(verts);
        for(unsigned int i=0;i<verts.size();i++){
            GeomEntity* ei = verts[i];
            if(toRemove[0].find(ei)==toRemove[0].end())//ei est conservé
                ei->remove(e);
        }
    }
    //pour les sommets, on doit vérifier les courbes
    it = toRemove[0].begin();
    for(;it!=toRemove[0].end();it++){
        GeomEntity* e = *it;
        std::vector<Curve*> curvs;
        e->get(curvs);
        for(unsigned int i=0;i<curvs.size();i++){
            GeomEntity* ei = curvs[i];
            if(toRemove[1].find(ei)==toRemove[1].end())//ei est conservé
                ei->remove(e);
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
