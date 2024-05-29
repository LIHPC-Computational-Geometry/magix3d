/*----------------------------------------------------------------------------*/
/*
 * \file EntitiesHelper.cpp
 *
 * \author Charles PIGNEROL
 *
 * \date 14/01/2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/EntitiesHelper.h"
#include "Internal/M3DCommandResult.h"
#include "Internal/CommandInternal.h"
#include "Utils/Common.h"
#include "Utils/Command.h"
#include "Utils/Vector.h"

#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Topo/Block.h"
#include "Topo/CoFace.h"
#include "Topo/Face.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"

#include <TkUtil/UTF8String.h>

#include <sys/types.h>          // CP : uint sur Bull
#include <TkUtil/MemoryError.h>
#include <TkUtil/NumericServices.h>


/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
EntitiesHelper::EntitiesHelper( )
{
    MGX_FORBIDDEN("EntitiesHelper::EntitiesHelper is not allowed.");
}	// EntitiesHelper::EntitiesHelper

/*----------------------------------------------------------------------------*/
EntitiesHelper::EntitiesHelper(const EntitiesHelper&)
{
    MGX_FORBIDDEN("EntitiesHelper::EntitiesHelper is not allowed.");
}	// EntitiesHelper::EntitiesHelper(const EntitiesHelper&)

/*----------------------------------------------------------------------------*/
EntitiesHelper& EntitiesHelper::operator = (const EntitiesHelper&)
{
    MGX_FORBIDDEN("EntitiesHelper::operator = is not allowed.");
    return *this;
}	// EntitiesHelper::operator =

/*----------------------------------------------------------------------------*/
EntitiesHelper::~EntitiesHelper ( )
{
    MGX_FORBIDDEN("EntitiesHelper::~EntitiesHelper is not allowed.");
}	// EntitiesHelper::~EntitiesHelper

/*----------------------------------------------------------------------------*/
std::vector<Mgx3D::Geom::GeomEntity*> EntitiesHelper::geomEntities (
							const std::vector<Mgx3D::Utils::Entity*>& entities)
{
	std::vector<Geom::GeomEntity*>	geomEntities;

	for (std::vector<Utils::Entity*>::const_iterator ite = entities.begin ( );
	     entities.end ( ) != ite; ite++)
	{
		Geom::GeomEntity*	ge	= dynamic_cast<Geom::GeomEntity*>(*ite);
		if (0 != ge)
			geomEntities.push_back (ge);
	}	// for (std::vector<Utils::Entity*>::iterator ite = ...

	return geomEntities;
}	// EntitiesHelper::geomEntities

/*----------------------------------------------------------------------------*/
std::vector<Mgx3D::Topo::TopoEntity*> EntitiesHelper::topoEntities (
							const std::vector<Mgx3D::Utils::Entity*>& entities)
{
	std::vector<Topo::TopoEntity*>	topoEntities;

	for (std::vector<Utils::Entity*>::const_iterator ite = entities.begin ( );
	     entities.end ( ) != ite; ite++)
	{
		Topo::TopoEntity*	te	= dynamic_cast<Topo::TopoEntity*>(*ite);
		if (0 != te)
			topoEntities.push_back (te);
	}	// for (std::vector<Utils::Entity*>::iterator ite = ...

	return topoEntities;
}	// EntitiesHelper::topoEntities

/*----------------------------------------------------------------------------*/
std::vector<Mgx3D::Topo::Block*> EntitiesHelper::topoBlocks (const std::vector<Mgx3D::Utils::Entity*>& entities)
{
	std::vector<Topo::Block*>	topoBlocks;

	for (std::vector<Utils::Entity*>::const_iterator ite = entities.begin ( ); entities.end ( ) != ite; ite++)
	{
		Topo::Block*	tb	= dynamic_cast<Topo::Block*>(*ite);
		if (0 != tb)
			topoBlocks.push_back (tb);
	}	// for (std::vector<Utils::Entity*>::iterator ite = ...

	return topoBlocks;
}	// EntitiesHelper::topoBlocks


/*----------------------------------------------------------------------------*/
std::vector<Mgx3D::Topo::CoFace*> EntitiesHelper::topoCoFaces (const std::vector<Mgx3D::Utils::Entity*>& entities)
{
	std::vector<Topo::CoFace*>	topoFaces;

	for (std::vector<Utils::Entity*>::const_iterator ite = entities.begin ( ); entities.end ( ) != ite; ite++)
	{
		Topo::CoFace*	tcf	= dynamic_cast<Topo::CoFace*>(*ite);
		if (0 != tcf)
			topoFaces.push_back (tcf);
	}	// for (std::vector<Utils::Entity*>::iterator ite = ...

	return topoFaces;
}	// EntitiesHelper::topoCoFaces
/*----------------------------------------------------------------------------*/
// [ EB] a réécrire si nécessaire en utilisant le contexte
//std::vector<Mgx3D::Utils::Entity*> EntitiesHelper::entities (
//				                        const std::vector<unsigned long>& ids)
//{
//	std::vector<Mgx3D::Utils::Entity*>	entities;
//	for (std::vector<unsigned long>::const_iterator it = ids.begin ( );
//	     ids.end ( ) != it; it++)
//		entities.push_back (&Mgx3D::Utils::Entity::uniqueIdToEntity (*it));
//
//	return entities;
//}	// EntitiesHelper::entities

/*----------------------------------------------------------------------------*/
std::vector<unsigned long> EntitiesHelper::uids (
	                        const std::vector<Mgx3D::Utils::Entity*>& entities)
{
	std::vector<unsigned long>	uids;
	for (std::vector<Mgx3D::Utils::Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
		uids.push_back ((*it)->getUniqueId ( ));

	return uids;
}	// EntitiesHelper::entities

/*----------------------------------------------------------------------------*/
bool EntitiesHelper::compare (const Mgx3D::Utils::Entity* e1, const Mgx3D::Utils::Entity* e2)
{
	CHECK_NULL_PTR_ERROR (e1)
	CHECK_NULL_PTR_ERROR (e2)
	return e1->getName ( ) < e2->getName ( );
}	// EntitiesHelper::compare

/*----------------------------------------------------------------------------*/
std::string EntitiesHelper::entitiesNames (
				const std::vector<Mgx3D::Utils::Entity*>& entities, size_t max)
{
	TkUtil::UTF8String	names (TkUtil::Charset::UTF_8);
	names << entities.size ( ) << " entités";

	if (entities.size ( ) <= max)
	{
		bool			first	= true;
		names << " ";

		for (std::vector<Utils::Entity*>::const_iterator ite =
							entities.begin ( ); entities.end ( ) != ite; ite++)
		{
			if (true != first)
				names << ", ";
			names << (*ite)->getName ( );

			first	= false;
		}	// for (std::vector<Utils::Entity*>::const_iterator ite = ...
	}	// if (entities.size ( ) <= max)

	return names.iso ( );
}	// EntitiesHelper::entitiesNames

/*----------------------------------------------------------------------------*/
std::string EntitiesHelper::entitiesNames (
			const std::vector<Mgx3D::Geom::GeomEntity*>& entities, size_t max)
{
	TkUtil::UTF8String	names (TkUtil::Charset::UTF_8);
	names << entities.size ( ) << " entités géométriques";

	if (entities.size ( ) <= max)
	{
		bool			first	= true;
		names << " ";
		for (std::vector<Geom::GeomEntity*>::const_iterator ite =
							entities.begin ( ); entities.end ( ) != ite; ite++)
		{
			if (true != first)
				names << ", ";
			names << (*ite)->getName ( );

			first	= false;
		}	// for (std::vector<Utils::Entity*>::const_iterator ite = ...
	}	// if (entities.size ( ) <= max)

	return names.iso ( );
}	// EntitiesHelper::entitiesNames

/*----------------------------------------------------------------------------*/
std::string EntitiesHelper::entitiesNames (
			const std::vector<Mgx3D::Topo::TopoEntity*>& entities, size_t max)
{
	TkUtil::UTF8String	names (TkUtil::Charset::UTF_8);
	names << entities.size ( ) << " entités topologiques";

	if (entities.size ( ) <= max)
	{
		bool			first	= true;
		names << " ";
		for (std::vector<Topo::TopoEntity*>::const_iterator ite =
							entities.begin ( ); entities.end ( ) != ite; ite++)
		{
			if (true != first)
				names << ", ";
			names << (*ite)->getName ( );

			first	= false;
		}	// for (std::vector<Utils::Entity*>::const_iterator ite = ...
	}	// if (entities.size ( ) <= max)

	return names.iso ( );
}	// EntitiesHelper::entitiesNames
/*----------------------------------------------------------------------------*/
bool EntitiesHelper::hasTopoRef(std::vector<Geom::GeomEntity*>& entities)
{
    for (uint i=0; i<entities.size(); i++){
        if (!entities[i]->getRefTopo().empty())
            return true;

        // recherche sur les entités de niveau inférieurs
        if (entities[i]->getDim()>0){
        	std::vector<Geom::Vertex*> sub_entities;
        	entities[i]->get(sub_entities);
        	for (uint j=0; j<sub_entities.size(); j++)
        		if (!sub_entities[j]->getRefTopo().empty())
        			return true;
        }
        if (entities[i]->getDim()>1){
        	std::vector<Geom::Curve*> sub_entities;
        	entities[i]->get(sub_entities);
        	for (uint j=0; j<sub_entities.size(); j++)
        		if (!sub_entities[j]->getRefTopo().empty())
        			return true;
        }
        if (entities[i]->getDim()>2){
        	std::vector<Geom::Surface*> sub_entities;
        	entities[i]->get(sub_entities);
        	for (uint j=0; j<sub_entities.size(); j++)
        		if (!sub_entities[j]->getRefTopo().empty())
        			return true;
        }

    } // end for i<entities.size()

    return false;
}
/*----------------------------------------------------------------------------*/
std::vector<TkUtil::UTF8String> EntitiesHelper::replaceNameByCommandRef(std::vector<Utils::Command*>& commands)
{
	std::vector<TkUtil::UTF8String> scripts;
	std::vector<Internal::M3DCommandResult*> results;

	for (uint i=0; i<commands.size(); i++){
		Utils::Command* command = commands[i];
		if (command->isCompleted() && command->getStatus() != Utils::CommandIfc::FAIL){
			Internal::CommandInternal* cmdi = dynamic_cast<Internal::CommandInternal*>(command);
			results.push_back(new Internal::M3DCommandResult(*cmdi));
			TkUtil::UTF8String newCmd;

			TkUtil::UTF8String oldCmd = command->getInitialScriptCommand();
			newCmd << "cmd"<<(short)i<<" = ";
			bool isObjName = false;
			TkUtil::UTF8String nomObj (TkUtil::Charset::UTF_8);

#ifdef _DEBUG2
			std::cout<<" oldCmd = "<<oldCmd<<std::endl;
#endif

			for (uint j=0; j<oldCmd.length(); j++){
//				TkUtil::WChar_t ch = oldCmd[j];
				char ch = *oldCmd[j];

				if (ch == '\"') {
					isObjName = !isObjName;

					// on vient de former un nom
					if (!isObjName){
#ifdef _DEBUG2
						std::cout<<"nomObj = "<<nomObj<<std::endl;
#endif

						// recherche d'une commande qui a créé cet objet
						TkUtil::UTF8String cmdObj (TkUtil::Charset::UTF_8);
						// recherche du type de cette objet
						Utils::Entity::objectType ot = getObjectType(nomObj);
#ifdef _DEBUG2
						std::cout<<" ot = "<<Utils::Entity::objectTypeToObjectTypeName(ot)<<std::endl;
#endif
						bool obTrouve = false;
						if (ot != Utils::Entity::undefined)
							for (uint k=0; k<i && !obTrouve; k++){
								std::vector<std::string> listeObjCmd;
								TkUtil::UTF8String nomTypeCmd (TkUtil::Charset::UTF_8);
								if (ot == Utils::Entity::GeomVolume){
									listeObjCmd = results[k]->getVolumes();
									nomTypeCmd = "getVolumes()";
								}
								else if (ot == Utils::Entity::GeomSurface){
									listeObjCmd = results[k]->getSurfaces();
									nomTypeCmd = "getSurfaces()";
								}
								else if (ot == Utils::Entity::GeomCurve){
									listeObjCmd = results[k]->getCurves();
									nomTypeCmd = "getCurves()";
								}
								else if (ot == Utils::Entity::GeomVertex){
									listeObjCmd = results[k]->getVertices();
									nomTypeCmd = "getVertices()";
								}
								else if (ot == Utils::Entity::TopoBlock){
									listeObjCmd = results[k]->getBlocks();
									nomTypeCmd = "getBlocks()";
								}
								else if (ot == Utils::Entity::TopoCoFace){
									listeObjCmd = results[k]->getFaces();
									nomTypeCmd = "getFaces()";
								}
								else if (ot == Utils::Entity::TopoCoEdge){
									listeObjCmd = results[k]->getEdges();
									nomTypeCmd = "getEdges()";
								}
								else if (ot == Utils::Entity::TopoVertex){
									listeObjCmd = results[k]->getTopoVertices();
									nomTypeCmd = "getTopoVertices()";
								}
								else
									throw TkUtil::Exception(TkUtil::UTF8String ("Erreur Interne dans replaceNameByCommandRef", TkUtil::Charset::UTF_8));

								// recherche si l'objet fait parti des objets créés
								uint indObj = 0;
								for (; indObj<listeObjCmd.size() && nomObj!=listeObjCmd[indObj]; indObj++)
									;
								if (indObj!=listeObjCmd.size()){
									obTrouve = true;
									newCmd << "cmd"<<(short)k<<"."<<nomTypeCmd<<"["<<(short)indObj<<"]";
								}

							} // end for k
						if (!obTrouve)
							newCmd <<'\"' << nomObj <<'\"';

						nomObj.clear();
					}
				}
				else {
					if (isObjName)
						nomObj << ch;
					else
						newCmd << ch;
				}
			} // end for j

			scripts.push_back(newCmd);

		} // end if (command->isCompleted())
		else
			scripts.push_back("");
	} // end for i<commands.size()

	for (uint i=0; i<results.size(); i++)
		delete results[i];

	return scripts;
}
/*----------------------------------------------------------------------------*/
Utils::Entity::objectType EntitiesHelper::getObjectType(TkUtil::UTF8String& nomObj)
{
	std::string nom = nomObj.ascii();
	if (Geom::Volume::isA(nom))
		return Utils::Entity::GeomVolume;
	else if (Geom::Surface::isA(nom))
		return Utils::Entity::GeomSurface;
	else if (Geom::Curve::isA(nom))
		return Utils::Entity::GeomCurve;
	else if (Geom::Vertex::isA(nom))
		return Utils::Entity::GeomVertex;
	else if (Topo::Block::isA(nom))
		return Utils::Entity::TopoBlock;
	else if (Topo::CoFace::isA(nom))
		return Utils::Entity::TopoCoFace;
	else if (Topo::CoEdge::isA(nom))
		return Utils::Entity::TopoCoEdge;
	else if (Topo::Vertex::isA(nom))
		return Utils::Entity::TopoVertex;
	else
		return Utils::Entity::undefined;
}
/*----------------------------------------------------------------------------*/
void EntitiesHelper::getAssociatedCurves(Topo::Vertex* vtx, std::vector<Geom::Curve*>& curves)
{
	Geom::GeomEntity* ge = vtx->getGeomAssociation();
	if (ge){
		if (ge->getDim()==0){
			ge->get(curves);
		}
		else if (ge->getDim()==1){
			Geom::Curve* curve = dynamic_cast<Geom::Curve*>(ge);
			curves.push_back(curve);
		}
	} // end if (ge)
}
/*----------------------------------------------------------------------------*/
void EntitiesHelper::getAssociatedSurfaces(Topo::Vertex* vtx, std::vector<Geom::Surface*>& surfaces)
{
	Geom::GeomEntity* ge = vtx->getGeomAssociation();
	if (ge){
		if (ge->getDim()==0 || ge->getDim()==1){
			ge->get(surfaces);
		}
		else if (ge->getDim()==2){
			Geom::Surface* surf = dynamic_cast<Geom::Surface*>(ge);
			surfaces.push_back(surf);
		}
	} // end if (ge)
}
/*----------------------------------------------------------------------------*/
void EntitiesHelper::getAssociatedSurfaces(Topo::CoEdge* coedge, std::vector<Geom::Surface*>& surfaces)
{
	Geom::GeomEntity* ge = coedge->getGeomAssociation();
	if (ge){
		if (ge->getDim()==1){
			ge->get(surfaces);
		}
		else if (ge->getDim()==2){
			Geom::Surface* surface = dynamic_cast<Geom::Surface*>(ge);
			surfaces.push_back(surface);
		}
	} // end if (ge)
}
/*----------------------------------------------------------------------------*/
Geom::Surface* EntitiesHelper::getCommonSurface(Topo::CoFace* coface)
{
#ifdef _DEBUG2
	std::cout<<"getCommonSurface pour "<<coface->getName()<<std::endl;
#endif
	Geom::Surface* surface = 0;

	std::vector<Topo::CoEdge* > coedges;
	coface->getCoEdges(coedges, true);

	if (!coedges.empty()){
		// recherche d'une surface commune à tous les groupes de surfaces
		// associés aux différentes arêtes
		std::map<Geom::Surface*, uint> filtre_surfaces;

		for (uint i=0; i<coedges.size(); i++){
			std::vector<Geom::Surface*> surfaces;
			getAssociatedSurfaces(coedges[i], surfaces);
#ifdef _DEBUG2
	std::cout<<"  surfaces pour "<<coedges[i]->getName()<<" :";
#endif

			for (uint j=0; j<surfaces.size(); j++){
				filtre_surfaces[surfaces[j]] += 1;
#ifdef _DEBUG2
				std::cout<<" "<<surfaces[j]->getName();
#endif

			}
#ifdef _DEBUG2
			std::cout<<std::endl;
#endif
		}

		uint nb_ok=0;
		for (std::map<Geom::Surface*, uint>::iterator iter2 = filtre_surfaces.begin();
				iter2!=filtre_surfaces.end(); ++iter2){
			if ((*iter2).second == coedges.size()){
				nb_ok+=1;
				surface = (*iter2).first;
			}
		}

#ifdef _DEBUG2
		std::cout<<"nb_ok = "<<nb_ok<<std::endl;
#endif

		// s'il y a ambiguité, on vérifie si les arêtes serait sur les contours d'une surface
		if (nb_ok>1){
			std::map<Geom::Curve*, uint> filtre_curves;
			for (uint i=0; i<coedges.size(); i++){
				Geom::GeomEntity* ge = coedges[i]->getGeomAssociation();
				if (ge && ge->getDim()==1){
					Geom::Curve* curve = dynamic_cast<Geom::Curve*>(ge);
					filtre_curves[curve] += 1;
				}
			}
			uint nb_crv = 0;
			for (std::map<Geom::Curve*, uint>::iterator iter2 = filtre_curves.begin();
					iter2!=filtre_curves.end(); ++iter2){
				nb_crv += 1;
			}
			// on recommence la recherche avec en plus un critère sur le nombre de courbes
			nb_ok=0;
			for (std::map<Geom::Surface*, uint>::iterator iter2 = filtre_surfaces.begin();
					iter2!=filtre_surfaces.end(); ++iter2){
				std::vector<Geom::Curve*> curves;
				(*iter2).first->get(curves);
				if ((*iter2).second == coedges.size() && curves.size() == nb_crv){
					nb_ok+=1;
					surface = (*iter2).first;
				}
			}
			// on pourrait aussi vérifier les courbes trouvées

#ifdef _DEBUG2
			std::cout<<"nb_ok (bis) = "<<nb_ok<<std::endl;
#endif
			if (nb_ok>1){
				surface = 0;
			}

		} // end if (nb_ok>1)
	} // end if (!coedges.empty())

	return surface;
}
/*----------------------------------------------------------------------------*/
double EntitiesHelper::length(const Utils::Entity* ce1, const Utils::Entity* ce2)
{
	double	distance	= TkUtil::NumericServices::doubleMachInfinity();

	if (ce1 == 0 || ce2 == 0)
		return distance;

	Utils::Entity* e1 = (Utils::Entity*)ce1;
	Utils::Entity* e2 = (Utils::Entity*)ce2;

	// on met en premier celle de dimension la plus petite
	if (e1->getDim() > e2->getDim()){
		Utils::Entity* tmp = e2;
		e2 = e1;
		e1 = tmp;
	}

	if (e1->getDim() == 0){
		Utils::Math::Point p1;
		if (e1->isGeomEntity()){
			Geom::Vertex* vertex = dynamic_cast<Geom::Vertex*>(e1);
			if (vertex)
				p1 = vertex->getCoord();
			else
				throw TkUtil::Exception ("Erreur interne Entity::length, pb dynamic_cast Geom::Vertex pour e1");
		} else if (e1->isTopoEntity()){
			Topo::Vertex* vertex = dynamic_cast<Topo::Vertex*>(e1);
			if (vertex)
				p1 = vertex->getCoord();
			else
				throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne Entity::length, pb dynamic_cast Topo::Vertex pour e1", TkUtil::Charset::UTF_8));
		} else
			throw TkUtil::Exception (TkUtil::UTF8String ("La première entité doit être un sommet géométrique ou topologique", TkUtil::Charset::UTF_8));

		Utils::Math::Point p2;
		if (e2->isGeomEntity()){
			Geom::Vertex* vertex = dynamic_cast<Geom::Vertex*>(e2);
			if (vertex)
				p2 = vertex->getCoord();
			else {
				Geom::GeomEntity* ge = dynamic_cast<Geom::GeomEntity*>(e2);
				if (ge)
					ge->project (p1, p2);
				else
					throw TkUtil::Exception (TkUtil::UTF8String ("La deuxième entité doit être une entité géométrique du type sommet, courbe ou surface", TkUtil::Charset::UTF_8));
			}
		} else if (e2->isTopoEntity()){
			Topo::Vertex* vertex = dynamic_cast<Topo::Vertex*>(e2);
			if (vertex)
				p2 = vertex->getCoord();
			else
				throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne Entity::length, pb dynamic_cast Topo::Vertex pour e2", TkUtil::Charset::UTF_8));
		} else
			throw TkUtil::Exception (TkUtil::UTF8String ("La deuxième entité doit être un sommet topologique ou une entité géométrique (sommet, courbe ou surface)", TkUtil::Charset::UTF_8));

		distance = p1.length (p2);
	}
	else
		throw TkUtil::Exception (TkUtil::UTF8String ("L'une des 2 entités doit être un point ou un sommet", TkUtil::Charset::UTF_8));


	return distance;
}
/*----------------------------------------------------------------------------*/
double EntitiesHelper::angle(const Utils::Entity* cec, const Utils::Entity* ce1, const Utils::Entity* ce2)
{

	double	angle	= TkUtil::NumericServices::doubleMachInfinity();

	if (cec == 0 || ce1 == 0 || ce2 == 0)
		return angle;

	if (ce1->getDim() != 0 || ce2->getDim() != 0 || cec->getDim() != 0)
		throw TkUtil::Exception (TkUtil::UTF8String ("Toutes les entités doivent être des points ou des sommets", TkUtil::Charset::UTF_8));

	Utils::Math::Point p1, p2, pc;

	if (ce1->isGeomEntity()){
		const Geom::Vertex* vertex = dynamic_cast<const Geom::Vertex*>(ce1);
		p1 = vertex->getCoord();
	}
	else if (ce1->isTopoEntity()){
		const Topo::Vertex* vertex = dynamic_cast<const Topo::Vertex*>(ce1);
		p1 = vertex->getCoord();
	}
	else
		throw TkUtil::Exception (TkUtil::UTF8String ("Le point 1 n'est pas un sommet géométrique ou topologique", TkUtil::Charset::UTF_8));

	if (ce2->isGeomEntity()){
		const Geom::Vertex* vertex = dynamic_cast<const Geom::Vertex*>(ce2);
		p2 = vertex->getCoord();
	}
	else if (ce2->isTopoEntity()){
		const Topo::Vertex* vertex = dynamic_cast<const Topo::Vertex*>(ce2);
		p2 = vertex->getCoord();
	}
	else
		throw TkUtil::Exception (TkUtil::UTF8String ("Le point 2 n'est pas un sommet géométrique ou topologique", TkUtil::Charset::UTF_8));

	if (cec->isGeomEntity()){
		const Geom::Vertex* vertex = dynamic_cast<const Geom::Vertex*>(cec);
		pc = vertex->getCoord();
	}
	else if (cec->isTopoEntity()){
		const Topo::Vertex* vertex = dynamic_cast<const Topo::Vertex*>(cec);
		pc = vertex->getCoord();
	}
	else
		throw TkUtil::Exception (TkUtil::UTF8String ("Le point central n'est pas un sommet géométrique ou topologique", TkUtil::Charset::UTF_8));

	Utils::Math::Vector v1(pc, p1);
	Utils::Math::Vector v2(pc, p2);

	v1 /= v1.norme();
	v2 /= v2.norme();

	// produit scalaire
	double ps = v1.dot(v2);

	angle =  acos(ps)*180/M_PI;

	return angle;
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
