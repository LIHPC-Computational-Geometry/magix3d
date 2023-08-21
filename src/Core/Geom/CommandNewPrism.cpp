/*----------------------------------------------------------------------------*/
/** \file CommandNewPrism.cpp
 *
 *  \author Franck Ledoux
 *
 *  \date 21/03/2012
 */
/*----------------------------------------------------------------------------*/
#include "Geom/CommandNewPrism.h"
#include "Geom/PropertyPrism.h"
#include "Geom/GeomNewPrismImplementation.h"
#include "Geom/GeomManager.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Utils/MgxException.h"
#include "Utils/MgxNumeric.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"
#include "Group/GroupManager.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/MemoryError.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandNewPrism::CommandNewPrism(Internal::Context& c,
        GeomEntity* e, const Utils::Math::Vector& dv,
        const std::string& groupName)
:CommandCreateWithOtherGeomEntities(c, "Création d'un prisme", groupName), m_base(e), m_dv(dv)
{
    if (Utils::Math::MgxNumeric::isNearlyZero(m_dv.abs2()))
        throw TkUtil::Exception(TkUtil::UTF8String ("La hauteur du prisme doit être strictement positive", TkUtil::Charset::UTF_8));

    m_impl = new GeomNewPrismImplementation(c,new PropertyPrism(e,dv));

    TkUtil::UTF8String comments (TkUtil::Charset::UTF_8);
	comments << "Création d'un prisme à partir de "<<m_base->getName()
			<<" extrudé suivant "<<m_dv;
	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandNewPrism::~CommandNewPrism()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandNewPrism::
internalExecute()
{

	for (uint i=0; i<m_createdEntities.size(); i++)
		saveMemento(m_createdEntities[i]);

    m_impl->perform(m_createdEntities);
    std::vector<GeomEntity*>& new_entities = m_impl->getNewEntities();
    std::vector<GeomEntity*>& rem_entities = m_impl->getRemovedEntities();

    // MAJ des connectivités V2V,...
    std::vector<Volume*> new_volumes;
    std::vector<Surface*> new_surfaces;
    std::vector<Curve*> new_curves;
    std::vector<Vertex*> new_vertices;

    for (int i=0;i<new_entities.size();i++)
    	if (new_entities[i]->getDim() == 3){
    		Volume* vol = dynamic_cast<Volume*>(new_entities[i]);
    		CHECK_NULL_PTR_ERROR(vol);
    		new_volumes.push_back(vol);
    	}
    	else if (new_entities[i]->getDim() == 2){
    		Surface* surf = dynamic_cast<Surface*>(new_entities[i]);
    		CHECK_NULL_PTR_ERROR(surf);
    		new_surfaces.push_back(surf);
    	}
    	else if (new_entities[i]->getDim() == 1){
    		Curve* crv = dynamic_cast<Curve*>(new_entities[i]);
    		CHECK_NULL_PTR_ERROR(crv);
    		new_curves.push_back(crv);
    	}
    	else if (new_entities[i]->getDim() == 0){
    		Vertex* vtx = dynamic_cast<Vertex*>(new_entities[i]);
    		CHECK_NULL_PTR_ERROR(vtx);
    		new_vertices.push_back(vtx);
    	}

//    std::cout<<"new_vertices.size() = "<<new_vertices.size()<<std::endl;
//    std::cout<<"new_curves.size() = "<<new_curves.size()<<std::endl;
//    std::cout<<"new_surfaces.size() = "<<new_surfaces.size()<<std::endl;
//    std::cout<<"new_volumes.size() = "<<new_volumes.size()<<std::endl;

    Surface* base = dynamic_cast<Surface*>(m_base);
    CHECK_NULL_PTR_ERROR(base);
    if (new_volumes.size() == 1)
    	m_s2v[base] = new_volumes[0];
    else
    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur Interne, cas avec autre chose qu'un unique volume en sortie de l'extrusion", TkUtil::Charset::UTF_8));

    // identification des entités reliées à la base

    // filtre pour marquer les entités appartenant à la base
    std::map<unsigned long, uint> filtre_uid;
    marque(base, filtre_uid);

    for (uint i=0; i<new_surfaces.size(); i++){
    	Surface* surf = new_surfaces[i];
    	Curve* curv = getMarquedCurve(surf, filtre_uid);
    	if (curv){
    		m_c2s[curv] = surf;
    		filtre_uid[surf->getUniqueId()] = 2;
    	}
    }

    for (uint i=0; i<new_curves.size(); i++){
    	Curve* curv = new_curves[i];
    	Vertex* vtx = getMarquedVertex(curv, filtre_uid);
    	if (vtx){
    		m_v2c[vtx] = curv;
    		filtre_uid[curv->getUniqueId()] = 2;

    		// recherche du sommet opposé
    		std::vector<Vertex*> vertices;
    		curv->get(vertices);
    		Vertex* opp_vtx = 0;
    		for (uint i=0; i<vertices.size(); i++)
    			if (filtre_uid[vertices[i]->getUniqueId()] == 0)
    				opp_vtx = vertices[i];
    		if (opp_vtx == 0)
    			throw TkUtil::Exception(TkUtil::UTF8String ("Erreur Interne, on ne trouve pas de point non marqué", TkUtil::Charset::UTF_8));

    		m_v2v_opp[vtx] = opp_vtx;
    	}
    }

    // les courbes en face
    for (uint i=0; i<new_surfaces.size(); i++){
    	Surface* surf = new_surfaces[i];

    	if (filtre_uid[surf->getUniqueId()] == 2){
    		std::vector<Curve*> curves;
    		surf->get(curves);
    		Curve* curv = 0;
    		Curve* opp_crv = 0;
    		for (uint i=0; i<curves.size(); i++)
    			if (filtre_uid[curves[i]->getUniqueId()] == 0)
    				opp_crv = curves[i];
    			else if (filtre_uid[curves[i]->getUniqueId()] == 1)
    				curv = curves[i];

    		if (curv == 0)
    			throw TkUtil::Exception(TkUtil::UTF8String ("Erreur Interne, on ne trouve pas de courbe marquée", TkUtil::Charset::UTF_8));
    		if (opp_crv == 0)
    			throw TkUtil::Exception(TkUtil::UTF8String ("Erreur Interne, on ne trouve pas de courbe opposée non marquée", TkUtil::Charset::UTF_8));

    		m_c2c_opp[curv] = opp_crv;
    	}
    }


    // la surface en face, celle qui n'est pas marquée
    {
    	Surface* opp_surf = 0;
    	for (uint i=0; i<new_surfaces.size(); i++)
    		if (filtre_uid[new_surfaces[i]->getUniqueId()] == 0)
    			opp_surf = new_surfaces[i];
    	if (opp_surf == 0)
    		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur Interne, on ne trouve pas de surface non marquée", TkUtil::Charset::UTF_8));

    	m_s2s_opp[base] = opp_surf;
    }

    // transmet les groupes du 2D vers le 3D
    groups2DTo3D();

    for(int i=0;i<new_entities.size();i++){
        GeomEntity* ge = new_entities[i];
        getInfoCommand ( ).addGeomInfoEntity (ge, Internal::InfoCommand::CREATED);
        getContext().getLocalGeomManager().addEntity(ge);

        // on fait l'association avec le groupe pour les volumes
        if (ge->getDim() == 3){
            Volume* vol = dynamic_cast<Volume*>(ge);
            if (vol && !m_group_name.empty() || vol->getNbGroups() == 0)
            	addToGroup(vol);
        }
    }
    for(int i=0;i<rem_entities.size();i++)
        getInfoCommand ( ).addGeomInfoEntity (rem_entities[i], Internal::InfoCommand::DELETED);

    getInfoCommand().setDestroyAndUpdateConnectivity(rem_entities);

}
/*----------------------------------------------------------------------------*/
void CommandNewPrism::marque(Surface* surf, std::map<unsigned long, uint>& filtre_uid)
{
	filtre_uid[surf->getUniqueId()] = 1;
	std::vector<Curve*> curves;
	std::vector<Vertex*> vertices;

	surf->get(curves);
	for (uint i=0; i<curves.size(); i++)
		filtre_uid[curves[i]->getUniqueId()] = 1;

	surf->get(vertices);
	for (uint i=0; i<vertices.size(); i++)
		filtre_uid[vertices[i]->getUniqueId()] = 1;
}
/*----------------------------------------------------------------------------*/
Vertex* CommandNewPrism::getMarquedVertex(Curve* curv, std::map<unsigned long, uint>& filtre_uid)
{
	std::vector<Vertex*> vertices;
	curv->get(vertices);
	for (uint i=0; i<vertices.size(); i++)
		if (filtre_uid[vertices[i]->getUniqueId()] == 1)
			return vertices[i];
	return 0;
}
/*----------------------------------------------------------------------------*/
Curve* CommandNewPrism::getMarquedCurve(Surface* surf, std::map<unsigned long, uint>& filtre_uid)
{
	std::vector<Curve*> curves;
	surf->get(curves);
	for (uint i=0; i<curves.size(); i++)
		if (filtre_uid[curves[i]->getUniqueId()] == 1)
			return curves[i];
	return 0;
}
/*----------------------------------------------------------------------------*/
//std::map<Geom::Vertex*,Geom::Vertex*>& CommandNewPrism::
//getAssociationV2V()
//{
//    return m_v2v;
//}
/*----------------------------------------------------------------------------*/
std::map<Geom::Vertex*,Geom::Vertex*>& CommandNewPrism::
getAssociationV2VOpp()
{
    return m_v2v_opp;
}
/*----------------------------------------------------------------------------*/
std::map<Geom::Vertex*,Geom::Curve*>& CommandNewPrism::
getAssociationV2C()
{
    return m_v2c;
}
/*----------------------------------------------------------------------------*/
//std::map<Geom::Curve*,Geom::Curve*>& CommandNewPrism::
//getAssociationC2C()
//{
//    return m_c2c;
//}
/*----------------------------------------------------------------------------*/
std::map<Geom::Curve*,Geom::Curve*>& CommandNewPrism::
getAssociationC2COpp()
{
    return m_c2c_opp;
}
/*----------------------------------------------------------------------------*/
std::map<Geom::Curve*,Geom::Surface*>& CommandNewPrism::
getAssociationC2S()
{
    return m_c2s;
}
/*----------------------------------------------------------------------------*/
//std::map<Geom::Surface*,Geom::Surface*>& CommandNewPrism::
//getAssociationS2S()
//{
//    return m_s2s;
//}
/*----------------------------------------------------------------------------*/
std::map<Geom::Surface*,Geom::Surface*>& CommandNewPrism::
getAssociationS2SOpp()
{
    return m_s2s_opp;
}
/*----------------------------------------------------------------------------*/
std::map<Geom::Surface*,Geom::Volume*>& CommandNewPrism::
getAssociationS2V()
{
    return m_s2v;
}
/*----------------------------------------------------------------------------*/
void CommandNewPrism::
groups2DTo3D()
{
    // [EB] on se sert de la correspondance entre les entités initiales et finales
    // pour transmetre les groupes du 2D au 3D (et du 1D au 2D ...)

    for (std::map<Geom::Surface*,Geom::Volume*>::iterator iter = m_s2v.begin();
            iter != m_s2v.end(); ++iter){
        Geom::Surface* surf = iter->first;
        Geom::Volume*  vol  = iter->second;
        if (vol){
        	std::vector<Group::GroupEntity*> grp;
            surf->getGroups(grp);

            for (uint i=0; i<grp.size(); i++){
                std::string nom = grp[i]->getName();
                // Hors Groupe 2D  ->  Hors Groupe 3D
                if (i == 0 && nom == getContext().getLocalGroupManager().getDefaultName(2))
                	nom = getContext().getLocalGroupManager().getDefaultName(3);

                Group::Group3D* new_grp = getContext().getLocalGroupManager().getNewGroup3D(nom, &getInfoCommand());
                new_grp->add(vol);
                vol->add(new_grp);
                new_grp->setLevel(grp[i]->getLevel());
            }
        }
    }

    for (std::map<Geom::Curve*  ,Geom::Surface*>::iterator iter = m_c2s.begin();
            iter != m_c2s.end(); ++iter){
        Geom::Curve*    curve = iter->first;
        Geom::Surface*  surf  = iter->second;
        if (surf){
            std::vector<Group::GroupEntity*> grp;
            curve->getGroups(grp);
            for (uint i=0; i<grp.size(); i++){
            	std::string nom = grp[i]->getName();
            	// Hors Groupe 1D  ->  Hors Groupe 2D
            	if (i == 0 && nom == getContext().getLocalGroupManager().getDefaultName(1))
            		nom = getContext().getLocalGroupManager().getDefaultName(2);

                Group::Group2D* new_grp = getContext().getLocalGroupManager().getNewGroup2D(nom, &getInfoCommand());
                new_grp->add(surf);
                surf->add(new_grp);
                new_grp->setLevel(grp[i]->getLevel());
            }
        }
    }

    for (std::map<Geom::Vertex* ,Geom::Curve*>::iterator iter = m_v2c.begin();
            iter != m_v2c.end(); ++iter){
        Geom::Vertex*  vtx  = iter->first;
        Geom::Curve*  curve = iter->second;
        if (curve){
        	std::vector<Group::GroupEntity*> grp;
            vtx->getGroups(grp);
            for (uint i=0; i<grp.size(); i++){
            	std::string nom = grp[i]->getName();
            	// Hors Groupe 0D  ->  Hors Groupe 1D
            	if (i == 0 && nom == getContext().getLocalGroupManager().getDefaultName(0))
            		nom = getContext().getLocalGroupManager().getDefaultName(1);

                Group::Group1D* new_grp = getContext().getLocalGroupManager().getNewGroup1D(nom, &getInfoCommand());
                new_grp->add(curve);
                curve->add(new_grp);
                new_grp->setLevel(grp[i]->getLevel());
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
