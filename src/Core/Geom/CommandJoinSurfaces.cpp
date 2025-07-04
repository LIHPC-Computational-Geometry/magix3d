/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/Volume.h"
#include "Geom/CommandJoinSurfaces.h"
#include "Group/Group2D.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_JOIN
CommandJoinSurfaces::
CommandJoinSurfaces(Internal::Context& c, std::vector<GeomEntity*>& entities)
: CommandJoinEntities(c, "Fusion de surfaces", entities)
{
}
/*----------------------------------------------------------------------------*/
CommandJoinSurfaces::~CommandJoinSurfaces()
{
}
/*----------------------------------------------------------------------------*/
void CommandJoinSurfaces::validate()
{
    if(m_entities.empty())
        throw TkUtil::Exception("Liste vide de surfaces");

    IN_UTIL UTF8String  warningText (TkUtil::Charset::UTF_8);
    warningText<<" Impossible d'unir des entités qui ne sont pas des surfaces\n";
    bool invalid = false;
    for(unsigned int i=0;i<m_entities.size();i++){
        GeomEntity *e=m_entities[i];
        if(e->getDim()!=2){
            invalid=true;
            warningText<<"L'entité "<<e->getName()<<" (dimension "
                       <<(short)e->getDim()<<") n'est pas une surface\n";
        }
    }

    if(invalid)
        throw TkUtil::Exception(warningText);

    // il faut que les surfaces fusionnées appartiennent aux mêmes volumes
	Surface* s0 = dynamic_cast<Surface*>(m_entities[0]);
    m_toKeepVolumes = s0->getVolumes();
    std::map<Volume*,uint> filtre;
    for (uint i=0; i<m_toKeepVolumes.size(); i++)
    	filtre[m_toKeepVolumes[i]] = 1;

    for (uint i=1; i<m_entities.size();i++){
   		Surface* si = dynamic_cast<Surface*>(m_entities[i]);
		auto vols = si->getVolumes();
    	if (vols.size() != m_toKeepVolumes.size())
    		throw TkUtil::Exception(TkUtil::UTF8String ("Les surfaces doivent être dans les mêmes volumes", TkUtil::Charset::UTF_8));
    	for (uint j=0; j<vols.size(); j++)
    		if (filtre[vols[j]] != 1)
    			throw TkUtil::Exception(TkUtil::UTF8String ("Les surfaces doivent être dans les mêmes volumes", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void CommandJoinSurfaces::init(std::vector<GeomEntity*>& es)
{
	CommandJoinEntities::init(es);

    // on marque les courbes adjacentes aux surfaces
    // si une courbe est vue 2 fois, elle sera détruite
    std::map<Curve*,uint> filtre;
    for (uint i=0;i<es.size();i++){
		Surface* si = dynamic_cast<Surface*>(es[i]);
    	auto curves = si->getCurves();
    	for (uint j=0; j<curves.size(); j++)
    		filtre[curves[j]] += 1;
    }
    for (std::map<Curve*,uint>::iterator iter=filtre.begin(); iter!=filtre.end(); ++iter){
#ifdef _DEBUG_JOIN
    	std::cout<<"init : "<<iter->first->getName()<<" filtre : "<<iter->second<<std::endl;
#endif
    	if (iter->second >= 2)
    		m_removedEntities.push_back(iter->first);
    	else
    		m_toKeepCurves.push_back(iter->first);
    }

}
/*----------------------------------------------------------------------------*/
void CommandJoinSurfaces::internalSpecificExecute()
{
	// utilisation d'un vecteur de shape pour la surface composite
	std::vector<TopoDS_Face> reps;
	for (auto entity : m_entities) {
		Surface* s = dynamic_cast<Surface*>(entity);
		auto loc_reps = s->getOCCFaces();
		std::copy(loc_reps.begin(), loc_reps.end(), std::back_inserter(reps));
	}

	// création de la surface union
	Surface* newSurface = new Surface(getContext(),
			getContext().newProperty(Utils::Entity::GeomSurface),
            getContext().newDisplayProperties(Utils::Entity::GeomSurface),
			new GeomProperty(),
			reps);
	getContext().newGraphicalRepresentation (*newSurface);
	m_newEntities.push_back(newSurface);

	// reprise des groupes de la première surface
	std::vector<Group::GroupEntity*> grp;
	m_entities[0]->getGroups(grp);
	newSurface->setGroups(grp);
	for (uint i=0; i<grp.size(); i++){
		Group::Group2D* group = getContext().getGroupManager().getNewGroup2D(grp[i]->getName(), &getInfoCommand());
		group->add(newSurface);
	}

	// destruction des anciennes surfaces
	m_removedEntities.insert(m_removedEntities.end(), m_entities.begin(), m_entities.end());

	// relation surface détruite -> nouvelle surface
	for(uint i=0; i<m_entities.size(); i++)
		m_replacedEntities[m_entities[i]].push_back(newSurface);

	// mise à jour des relations courbe-surface et surface-volume

	// les volumes restent les même
	 for (uint i=0; i<m_toKeepVolumes.size(); i++){
		 newSurface->add(m_toKeepVolumes[i]);
		 m_toKeepVolumes[i]->add(newSurface);
	 }

	 // recherche des courbes au bord de la nouvelle surface
    std::map<Curve*,uint> filtre;
    for (uint i=0;i<m_entities.size();i++){
		Surface* si = dynamic_cast<Surface*>(m_entities[i]);
    	auto curves = si->getCurves();
    	for (uint j=0; j<curves.size(); j++)
    		filtre[curves[j]] += 1;
    }

    for (std::map<Curve*,uint>::iterator iter=filtre.begin(); iter!=filtre.end(); ++iter)
    	if (iter->second == 1){
    		newSurface->add(iter->first);
    		iter->first->add(newSurface);
    	}

    m_createdEntities = m_newEntities;
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
