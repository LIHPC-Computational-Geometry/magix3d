/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Geom/CommandJoinCurves.h"
#include "Group/GroupEntity.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandJoinCurves::
CommandJoinCurves(Internal::Context& c, std::vector<GeomEntity*>& entities)
: CommandJoinEntities(c, "Fusion de courbes", entities)
{
} 
/*----------------------------------------------------------------------------*/
CommandJoinCurves::~CommandJoinCurves()
{
}
/*----------------------------------------------------------------------------*/
void CommandJoinCurves::validate()
{
    if(m_entities.empty())
        throw TkUtil::Exception(TkUtil::UTF8String ("Liste vide de courbes", TkUtil::Charset::UTF_8));

    IN_UTIL UTF8String  warningText (TkUtil::Charset::UTF_8);
    bool invalid = false;
    for(unsigned int i=0;i<m_entities.size();i++){
        GeomEntity *e=m_entities[i];
        if (e == 0){
        	invalid=true;
        	warningText<<"On ne peut faire une fusion de courbes, l'une d'entre elles est nulle\n";
        }
        else if(e->getDim()!=1){
            invalid=true;
            warningText<<"L'entité "<<e->getName()<<" (dimension "
                       <<(short)e->getDim()<<") n'est pas une courbe\n";
        }
    }

    if(invalid) 
        throw TkUtil::Exception(warningText);

    // il faut que les courbes fusionnées appartiennent aux mêmes surfaces
	Curve* c0 = dynamic_cast<Curve*>(m_entities[0]);
	m_toKeepSurfaces = c0->getSurfaces();
    std::map<Surface*,uint> filtre;
    for (uint i=0; i<m_toKeepSurfaces.size(); i++)
    	filtre[m_toKeepSurfaces[i]] = 1;

    for (uint i=1; i<m_entities.size();i++){
		Curve* ci = dynamic_cast<Curve*>(m_entities[i]);
    	auto surfs = ci->getSurfaces();
    	if (surfs.size() != m_toKeepSurfaces.size())
    		throw TkUtil::Exception(TkUtil::UTF8String ("Les courbes doivent être dans les mêmes surfaces", TkUtil::Charset::UTF_8));
    	for (uint j=0; j<surfs.size(); j++)
    		if (filtre[surfs[j]] != 1)
    			throw TkUtil::Exception(TkUtil::UTF8String ("Les courbes doivent être dans les mêmes surfaces", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void CommandJoinCurves::
internalSpecificExecute()
{
	// recherche des sommets au bord de la nouvelle courbe
	std::vector<Vertex*> extremites;

	// le map doit être trié de manière reproductible (pas par les pointeurs)
	std::map<Vertex*, uint, decltype(&Utils::Entity::compareEntity)> filtre(&Utils::Entity::compareEntity);
    for (uint i=0;i<m_entities.size();i++){
		Curve* ci = dynamic_cast<Curve*>(m_entities[i]);
    	auto vertices = ci->getVertices();
    	for (uint j=0; j<vertices.size(); j++)
    		filtre[vertices[j]] += 1;
    }

    for (std::map<Vertex*,uint>::iterator iter=filtre.begin(); iter!=filtre.end(); ++iter)
    	if (iter->second == 1){
    		extremites.push_back(iter->first);
    		m_toKeepVertices.push_back(iter->first);
    	}

    for (std::map<Vertex*,uint>::iterator iter=filtre.begin(); iter!=filtre.end(); ++iter)
        // cas cyclique
    	if (extremites.empty() && iter->second >= 2){
    		// on prend un des sommets
    		extremites.push_back(iter->first);
    		m_toKeepVertices.push_back(iter->first);
    	}
    	else if (iter->second >= 2)
    		m_removedEntities.push_back(iter->first);

    if (extremites.size() != 2 && extremites.size() != 1)
    	throw TkUtil::Exception(TkUtil::UTF8String ("Les courbes ne doivent pas être discontinues", TkUtil::Charset::UTF_8));

    // pour avoir les entités toujours dans le même ordre
    if (extremites.size() == 2 && extremites[0]->getUniqueId()>extremites[1]->getUniqueId()){
    	Vertex* tmp = extremites[0];
    	extremites[0] = extremites[1];
    	extremites[1] = tmp;
    }

    // construction d'un vecteur ordonné de courbes (en partant du premier sommet extrémité)
    std::vector<Curve*> courbesOrdonnees;
    // recherche d'une première courbe qui contienne le premier sommet
    Vertex* vtxDep = extremites[0];
    Curve* crvDep = getCurveContains(m_entities, vtxDep);
    if (crvDep==0)
    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, recherche crvDep==0", TkUtil::Charset::UTF_8));
    courbesOrdonnees.push_back(crvDep);

    do {
    	// recherche du sommet opposé sur la courbe
    	vtxDep = getOppositeVtx(crvDep, vtxDep);

    	// recherche d'une courbe à partir d'un sommet en excluant une première courbe
    	crvDep = getCurve(vtxDep, crvDep);

    	courbesOrdonnees.push_back(crvDep);

    } while (courbesOrdonnees.size() != m_entities.size());

	// utilisation d'un vecteur de shapes pour la courbe composite
	std::vector<TopoDS_Edge> reps;
	for (auto courbesOrdonnee : courbesOrdonnees) {
		auto loc_reps = courbesOrdonnee->getOCCEdges();
		std::copy(loc_reps.begin(), loc_reps.end(), std::back_inserter(reps));
	}

	// création de la courbe union
	Curve* newCurve = new Curve(getContext(),
			getContext().newProperty(Utils::Entity::GeomCurve),
            getContext().newDisplayProperties(Utils::Entity::GeomCurve),
			new GeomProperty(),
			reps);
	getContext().newGraphicalRepresentation (*newCurve);
	m_newEntities.push_back(newCurve);

	// reprise des groupes de la première courbe
	std::vector<Group::GroupEntity*> grp;
	m_entities[0]->getGroups(grp);
	for (uint i=0; i<grp.size(); i++){
		m_group_helper.addToGroup(grp[i]->getName(), newCurve);
	}

	// destruction des anciennes courbes
	m_removedEntities.insert(m_removedEntities.end(), m_entities.begin(), m_entities.end());

	// relation courbe détruite -> nouvelle courbe
	for(uint i=0; i<m_entities.size(); i++)
		m_replacedEntities[m_entities[i]].push_back(newCurve);

	// mise à jour des relations surface-courbe, les surfaces restent les même
	for (uint i=0; i<m_toKeepSurfaces.size(); i++){
		newCurve->add(m_toKeepSurfaces[i]);
		m_toKeepSurfaces[i]->add(newCurve);
	}

	// mise à jour des relations courbe-sommet
	for (uint i=0; i<extremites.size(); i++){
		newCurve->add(extremites[i]);
		extremites[i]->add(newCurve);
	}

	 m_createdEntities = m_newEntities;
}
/*----------------------------------------------------------------------------*/
Curve* CommandJoinCurves::getCurveContains(std::vector<GeomEntity*>& entities,
		Vertex* vtx)
{
	Curve* ge = 0;
	for (uint i=0;i<entities.size();i++){
		Curve* c = dynamic_cast<Curve*>(entities[i]);
		for (auto v : c->getVertices())
			if (v == vtx)
				ge = c;
	}
	return ge;
}
/*----------------------------------------------------------------------------*/
Vertex* CommandJoinCurves::getOppositeVtx(Curve* crv, Vertex* vtx)
{
	auto vertices = crv->getVertices();
	if (vertices.size() != 2)
		throw TkUtil::Exception(TkUtil::UTF8String ("Les courbes doivent avoir 2 sommets", TkUtil::Charset::UTF_8));

	if (vertices[0] == vtx)
		return vertices[1];
	else if (vertices[1] == vtx)
		return vertices[0];
	else
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, on ne trouve pas un sommet dans une courbe", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Curve* CommandJoinCurves::getCurve(Vertex* vtx, Curve* crvExclude)
{
	auto curves = vtx->getCurves();
	if (curves.size() == 1)
		throw TkUtil::Exception(TkUtil::UTF8String ("Les sommets doivent être reliés à 2 courbes distinctes", TkUtil::Charset::UTF_8));
	if (curves.size() != 2)
		throw TkUtil::Exception(TkUtil::UTF8String ("Les sommets ne doivent être reliés qu'à 2 courbes", TkUtil::Charset::UTF_8));

	if (curves[0] == crvExclude)
		return curves[1];
	else if (curves[1] == crvExclude)
		return curves[0];
	else
		throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, on ne trouve pas une courbe reliée à un sommet", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
