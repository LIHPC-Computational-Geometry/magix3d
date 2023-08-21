/*----------------------------------------------------------------------------*/
/** \file CommandExtrusion.cpp
 *
 *  Created on: 8/11/2019
 *      Author: Eric B
 *     à partir de CommandRevolution, renommé depuis en CommandExtrudeRevolution
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include "Geom/CommandExtrusion.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomExtrudeImplementation.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
#include "Group/Group1D.h"
#include "Group/Group0D.h"
#include "Group/GroupManager.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandExtrusion::
CommandExtrusion(Internal::Context& c, std::string name)
:   CommandCreateGeom(c, name)
{
}
/*----------------------------------------------------------------------------*/
CommandExtrusion::~CommandExtrusion()
{
}
/*----------------------------------------------------------------------------*/
std::map<Geom::Vertex*,Geom::Vertex*>& CommandExtrusion::
getAssociationV2V()
{
    return m_v2v;
}
/*----------------------------------------------------------------------------*/
std::map<Geom::Vertex*,Geom::Vertex*>& CommandExtrusion::
getAssociationV2VOpp()
{
    return m_v2v_opp;
}
/*----------------------------------------------------------------------------*/
std::map<Geom::Vertex*,Geom::Curve*>& CommandExtrusion::
getAssociationV2C()
{
    return m_v2c;
}
/*----------------------------------------------------------------------------*/
std::map<Geom::Curve*,Geom::Curve*>& CommandExtrusion::
getAssociationC2C()
{
    return m_c2c;
}
/*----------------------------------------------------------------------------*/
std::map<Geom::Curve*,Geom::Curve*>& CommandExtrusion::
getAssociationC2COpp()
{
    return m_c2c_opp;
}
/*----------------------------------------------------------------------------*/
std::map<Geom::Curve*,Geom::Surface*>& CommandExtrusion::
getAssociationC2S()
{
    return m_c2s;
}
/*----------------------------------------------------------------------------*/
std::map<Geom::Surface*,Geom::Surface*>& CommandExtrusion::
getAssociationS2S()
{
    return m_s2s;
}
/*----------------------------------------------------------------------------*/
std::map<Geom::Surface*,Geom::Surface*>& CommandExtrusion::
getAssociationS2SOpp()
{
    return m_s2s_opp;
}
/*----------------------------------------------------------------------------*/
std::map<Geom::Surface*,Geom::Volume*>& CommandExtrusion::
getAssociationS2V()
{
    return m_s2v;
}
/*----------------------------------------------------------------------------*/
void CommandExtrusion::printInfoAssociations() const
{
	std::cout<<"------------------------------------------------------"<<std::endl;
	std::cout<<"printInfoAssociations() => "<<std::endl;
	std::cout<<"## V2V"<<std::endl;
	for (std::map<Geom::Vertex* ,Geom::Vertex*>::const_iterator iter = m_v2v.begin(); iter != m_v2v.end(); ++iter){
		Geom::Vertex* geom1 = iter->first;
		Geom::Vertex* geom2 = iter->second;
		std::cout<<" "<<geom1->getName()<<" -> "<<(geom2?geom2->getName():"rien")<<std::endl;
	}
	std::cout<<"## V2V OPP"<<std::endl;
	for (std::map<Geom::Vertex* ,Geom::Vertex*>::const_iterator iter = m_v2v_opp.begin(); iter != m_v2v_opp.end(); ++iter){
		Geom::Vertex* geom1 = iter->first;
		Geom::Vertex* geom2 = iter->second;
		std::cout<<" "<<geom1->getName()<<" -> "<<(geom2?geom2->getName():"rien")<<std::endl;
	}
	std::cout<<"## V2C"<<std::endl;
	for (std::map<Geom::Vertex* ,Geom::Curve*>::const_iterator iter = m_v2c.begin(); iter != m_v2c.end(); ++iter){
		Geom::Vertex* geom1 = iter->first;
		Geom::Curve* geom2 = iter->second;
		std::cout<<" "<<geom1->getName()<<" -> "<<(geom2?geom2->getName():"rien")<<std::endl;
	}
	std::cout<<"## C2C"<<std::endl;
	for (std::map<Geom::Curve* ,Geom::Curve*>::const_iterator iter = m_c2c.begin(); iter != m_c2c.end(); ++iter){
		Geom::Curve* geom1 = iter->first;
		Geom::Curve* geom2 = iter->second;
		std::cout<<" "<<geom1->getName()<<" -> "<<(geom2?geom2->getName():"rien")<<std::endl;
	}
	std::cout<<"## C2C OPP"<<std::endl;
	for (std::map<Geom::Curve* ,Geom::Curve*>::const_iterator iter = m_c2c_opp.begin(); iter != m_c2c_opp.end(); ++iter){
		Geom::Curve* geom1 = iter->first;
		Geom::Curve* geom2 = iter->second;
		std::cout<<" "<<geom1->getName()<<" -> "<<(geom2?geom2->getName():"rien")<<std::endl;
	}
	std::cout<<"## C2S"<<std::endl;
	for (std::map<Geom::Curve* ,Geom::Surface*>::const_iterator iter = m_c2s.begin(); iter != m_c2s.end(); ++iter){
		Geom::Curve* geom1 = iter->first;
		Geom::Surface* geom2 = iter->second;
		std::cout<<" "<<geom1->getName()<<" -> "<<(geom2?geom2->getName():"rien")<<std::endl;
	}
	std::cout<<"## S2S"<<std::endl;
	for (std::map<Geom::Surface* ,Geom::Surface*>::const_iterator iter = m_s2s.begin(); iter != m_s2s.end(); ++iter){
		Geom::Surface* geom1 = iter->first;
		Geom::Surface* geom2 = iter->second;
		std::cout<<" "<<geom1->getName()<<" -> "<<(geom2?geom2->getName():"rien")<<std::endl;
	}
	std::cout<<"## S2S OPP"<<std::endl;
	for (std::map<Geom::Surface* ,Geom::Surface*>::const_iterator iter = m_s2s_opp.begin(); iter != m_s2s_opp.end(); ++iter){
		Geom::Surface* geom1 = iter->first;
		Geom::Surface* geom2 = iter->second;
		std::cout<<" "<<geom1->getName()<<" -> "<<(geom2?geom2->getName():"rien")<<std::endl;
	}
	std::cout<<"## S2V"<<std::endl;
	for (std::map<Geom::Surface* ,Geom::Volume*>::const_iterator iter = m_s2v.begin(); iter != m_s2v.end(); ++iter){
		Geom::Surface* geom1 = iter->first;
		Geom::Volume* geom2 = iter->second;
		std::cout<<" "<<geom1->getName()<<" -> "<<(geom2?geom2->getName():"rien")<<std::endl;
	}
	std::cout<<"------------------------------------------------------"<<std::endl;
}
/*----------------------------------------------------------------------------*/
void CommandExtrusion::
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
void CommandExtrusion::prefixGroupsName(const std::string& pre,
        std::map<Geom::Vertex* ,Geom::Vertex*>&  v2v,
        std::map<Geom::Curve*  ,Geom::Curve*>&   c2c,
        std::map<Geom::Surface*,Geom::Surface*>& s2s)
{
    // le groupe commun pour toutes les surfaces
    Group::Group2D* grp_comm = getContext().getLocalGroupManager().getNewGroup2D(pre, &getInfoCommand());

    for (std::map<Geom::Surface*,Geom::Surface*>::iterator iter = s2s.begin();
            iter != s2s.end(); ++iter){
        Geom::Surface* surf1 = iter->first;
        Geom::Surface* surf2 = iter->second;

        if (surf2){
            std::vector<std::string> gn;
            surf1->getGroupsName(gn);
            for (uint i=0; i<gn.size(); i++){
                std::string& nom = gn[i];
                Group::Group2D* grp = getContext().getLocalGroupManager().getNewGroup2D(pre + "_" + nom, &getInfoCommand());
                grp->add(surf2);
                surf2->add(grp);
            }

            // ajoute la surface dans le groupe / plan de sym
            grp_comm->add(surf2);
            surf2->add(grp_comm);
        }
    }

    for (std::map<Geom::Curve*  ,Geom::Curve*>::iterator iter = c2c.begin();
            iter != c2c.end(); ++iter){
        Geom::Curve* crv1 = iter->first;
        Geom::Curve* crv2 = iter->second;

        if (crv2){
            std::vector<std::string> gn;
            crv1->getGroupsName(gn);
            for (uint i=0; i<gn.size(); i++){
                std::string& nom = gn[i];
                Group::Group1D* grp = getContext().getLocalGroupManager().getNewGroup1D(pre + "_" + nom, &getInfoCommand());
                grp->add(crv2);
                crv2->add(grp);
            }
        }
    }

    for (std::map<Geom::Vertex* ,Geom::Vertex*>::iterator iter = v2v.begin();
            iter != v2v.end(); ++iter){
        Geom::Vertex* vtx1 = iter->first;
        Geom::Vertex* vtx2 = iter->second;

        if (vtx2){
            std::vector<std::string> gn;
            vtx1->getGroupsName(gn);
            for (uint i=0; i<gn.size(); i++){
                std::string& nom = gn[i];
                Group::Group0D* grp = getContext().getLocalGroupManager().getNewGroup0D(pre + "_" + nom, &getInfoCommand());
                grp->add(vtx2);
                vtx2->add(grp);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/


















