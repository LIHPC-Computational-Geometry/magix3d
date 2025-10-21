/*----------------------------------------------------------------------------*/
#include "Geom/CommandExtrusion.h"
#include "Geom/GeomExtrudeImplementation.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Group/GroupEntity.h"
#include "Group/GroupManager.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandExtrusion::
CommandExtrusion(Internal::Context& c, 
    std::string name,
    std::vector<GeomEntity*>& entities, 
    const bool keep)
: CommandEditGeom(c, name)
, m_entities(entities)
, m_keep(keep)
{
}
/*----------------------------------------------------------------------------*/
void CommandExtrusion::internalSpecificPreExecute()
{
    m_impl->prePerform();
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
    // Les nouveaux objets ont été mis "hors groupe" à la création
    // On se sert de la correspondance entre les entités initiales et finales
    // pour transmetre les groupes de la dimension N à la dimension N+1

    for (std::map<Geom::Surface*,Geom::Volume*>::iterator iter = m_s2v.begin();
            iter != m_s2v.end(); ++iter){
        Geom::Surface* surf = iter->first;
        Geom::Volume*  vol  = iter->second;
        if (vol){
        	std::vector<Group::GroupEntity*> grp;
            surf->getGroups(grp);

            for (uint i=0; i<grp.size(); i++){
                std::string nom = grp[i]->getName();
                if (nom != getContext().getGroupManager().getDefaultName(2)) {
                    Group::Group3D* new_grp = m_group_helper.addToGroup(nom, vol);
                    new_grp->setLevel(grp[i]->getLevel());
                }
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
            	if (nom != getContext().getGroupManager().getDefaultName(1)) {
                    Group::Group2D* new_grp = m_group_helper.addToGroup(nom, surf);
                    new_grp->setLevel(grp[i]->getLevel());
                }
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
            	if (nom != getContext().getGroupManager().getDefaultName(0)) {
                    Group::Group1D* new_grp = m_group_helper.addToGroup(nom, curve);
                    new_grp->setLevel(grp[i]->getLevel());
                }
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
    for (std::map<Geom::Surface*,Geom::Surface*>::iterator iter = s2s.begin();
            iter != s2s.end(); ++iter){
        Geom::Surface* surf1 = iter->first;
        Geom::Surface* surf2 = iter->second;

        if (surf2){
            std::vector<std::string> gn;
            surf1->getGroupsName(gn);
            for (uint i=0; i<gn.size(); i++){
                std::string& nom = gn[i];
                m_group_helper.addToGroup(pre + "_" + nom, surf2);
            }

            // ajoute la surface dans le groupe / plan de sym
            m_group_helper.addToGroup(pre, surf2);
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
                m_group_helper.addToGroup(pre + "_" + nom, crv2);
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
                m_group_helper.addToGroup(pre + "_" + nom, vtx2);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/


















