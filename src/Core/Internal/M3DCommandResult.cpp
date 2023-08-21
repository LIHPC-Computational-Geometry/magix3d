/*----------------------------------------------------------------------------*/
/** \file		M3DCommandResult.cpp
 *  \author		Charles, Eric
 *  \date		02/04/2013
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Internal/M3DCommandResult.h"
#include "Internal/CommandInternalAdapter.h"
#include "Utils/Common.h"
#include "Geom/CommandEditGeom.h"
#include "Geom/GeomEntity.h"
#include "Mesh/MeshEntity.h"

#include <TkUtil/Exception.h>

using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace TkUtil;


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Internal
{

/*----------------------------------------------------------------------------*/

M3DCommandResult::M3DCommandResult (CommandInternal& cmd)
	: M3DCommandResultIfc (cmd),
	  _commandInternal (&cmd),
	  m_geom_modif(0)
{
}	// M3DCommandResult::M3DCommandResult

M3DCommandResult::M3DCommandResult (CommandInternal& cmd, Geom::CommandEditGeom* geom_modif)
	: M3DCommandResultIfc (cmd),
	  _commandInternal (&cmd),
	  m_geom_modif(geom_modif)
{
}	// M3DCommandResult::M3DCommandResult

M3DCommandResult::M3DCommandResult ()
	: M3DCommandResultIfc (),
	  _commandInternal (0),
	  m_geom_modif(0)
{
}	// M3DCommandResult::M3DCommandResult

M3DCommandResult::M3DCommandResult (const M3DCommandResult&)
	: M3DCommandResultIfc (*new CommandInternalAdapter ("invalid")),
	  _commandInternal (0),
	  m_geom_modif(0)
{
	MGX_FORBIDDEN ("M3DCommandResult copy constructor is not allowed.")
}	// M3DCommandResult::M3DCommandResult


M3DCommandResult& M3DCommandResult::operator = (const M3DCommandResult&)
{
	MGX_FORBIDDEN ("M3DCommandResult assignment operator is not allowed.")
	return *this;
}	// M3DCommandResult::operator =


M3DCommandResult::~M3DCommandResult ( )
{
	AutoMutex	autoMutex ((Mutex*)getMutex ( ));
	_commandInternal	= 0;
	unregisterReferences ( );
}	// M3DCommandResult::~M3DCommandResult


void M3DCommandResult::observableModified (
								ReferencedObject* object, unsigned long event)
{
	CommandResult::observableModified (object, event);
}	// M3DCommandResult::observableModified


void M3DCommandResult::observableDeleted (ReferencedObject* object)
{
	AutoMutex	autoMutex ((Mutex*)getMutex ( ));

	if (dynamic_cast<CommandInternal*>(object) == _commandInternal)
	{
		_commandInternal	= 0;
	}	// if (dynamic_cast<Command*>(object) == _command)

	CommandResult::observableDeleted (object);
}	// M3DCommandResult::observableDeleted


std::vector<Geom::Volume*> M3DCommandResult::getVolumesObj ( )
{
#ifdef _DEBUG_GEOM_MODIF
	std::cout<<"M3DCommandResult::getVolumesObj() pour commande "<<_commandInternal->getName()<<std::endl;
#endif
    std::vector<Geom::Volume*> objets;

    if (m_geom_modif){
    	// dans le cas où l'on dispose de la commande de base de modification géométrique
    	// on récupère les volumes parmi les entités conservées, déplacés et celles nouvelles
    	{
#ifdef _DEBUG_GEOM_MODIF
	std::cout<<" on utilise GeomModificationBaseClass "<<std::endl;
#endif
    		std::vector<Geom::GeomEntity*> entities = m_geom_modif->getKeepedEntities();
    		for (uint i=0; i<entities.size(); i++)
    			if (entities[i]->getDim() == 3){
    				Geom::Volume* ob = dynamic_cast<Geom::Volume*>(entities[i]);
    				if (0 == ob)
    					throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getGeomVolumes", TkUtil::Charset::UTF_8));
    				objets.push_back(ob);
#ifdef _DEBUG_GEOM_MODIF
    				std::cout<<"   volume ajouté (via getKeepedEntities) : "<<ob->getName()<<std::endl;
#endif
    			}
    	}
    	{
    		std::vector<Geom::GeomEntity*> entities = m_geom_modif->getNewEntities();
    		for (uint i=0; i<entities.size(); i++)
    			if (entities[i]->getDim() == 3){
    				Geom::Volume* ob = dynamic_cast<Geom::Volume*>(entities[i]);
    				if (0 == ob)
    					throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getGeomVolumes", TkUtil::Charset::UTF_8));
    				objets.push_back(ob);
#ifdef _DEBUG_GEOM_MODIF
    				std::cout<<"   volume ajouté (via getNewEntities) : "<<ob->getName()<<std::endl;
#endif
    			}
    	}
    	{
    		std::vector<Geom::GeomEntity*> entities = m_geom_modif->getMovedEntities();
    		for (uint i=0; i<entities.size(); i++)
    			if (entities[i]->getDim() == 3){
    				Geom::Volume* ob = dynamic_cast<Geom::Volume*>(entities[i]);
    				if (0 == ob)
    					throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getGeomVolumes", TkUtil::Charset::UTF_8));
    				objets.push_back(ob);
#ifdef _DEBUG_GEOM_MODIF
    				std::cout<<"   volume ajouté (via getMovedEntities) : "<<ob->getName()<<std::endl;
#endif
    			}
    	}
    }
    else {
#ifdef _DEBUG_GEOM_MODIF
	std::cout<<" on utilise InfoCommand "<<std::endl;
#endif
        // recherche depuis l'InfoCommand, les volumes créés
    	InfoCommand& ifc = getInfoCommand();

    	std::vector<InfoCommand::GeomEntityInfo>& gei = ifc.getGeomInfoEntities();

    	for (std::vector <InfoCommand::GeomEntityInfo>::const_iterator iter = gei.begin();
    			iter != gei.end(); ++iter) {
    		if (iter->m_geom_entity->getDim() == 3 && iter->m_type == InfoCommand::CREATED){
    			Geom::Volume* ob = dynamic_cast<Geom::Volume*>(iter->m_geom_entity);
    			if (0 == ob)
    				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getGeomVolumes", TkUtil::Charset::UTF_8));
    			objets.push_back(ob);
    		}
    	}
    }
    return objets;

}	// M3DCommandResult::getVolumesObj


Geom::Volume* M3DCommandResult::getVolumeObj (size_t index)
{
    std::vector<Geom::Volume*> volumes = getVolumesObj();

    if (index >= volumes.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);

        messErr << "getGeomVolume("<<index
                << ") impossible, il n'y a que "<<volumes.size()
                << " volume[s] dans cette commande";
        throw TkUtil::Exception(messErr);
    }

    return volumes[index];

}	// M3DCommandResult::getVolumeObj


Geom::Volume* M3DCommandResult::getVolumeObj ( )
{
    std::vector<Geom::Volume*> volumes = getVolumesObj();

    if (volumes.size() == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("getGeomVolume impossible, il n'y a pas de volume créé dans cette commande", TkUtil::Charset::UTF_8));
    else if (volumes.size() > 1){
    	std::cout<<"M3DCommandResult::getVolumeObj ( ) en erreur, liste des volumes créés pour info :";
    	for (uint i=0; i<volumes.size(); i++){
    		std::cout<<volumes[i]->getName()<<" ";
    	}
    	std::cout<<std::endl;
    	throw TkUtil::Exception(TkUtil::UTF8String ("getGeomVolume impossible, il y a plus d'un volume de créé dans cette commande", TkUtil::Charset::UTF_8));
    }
    else
        return volumes[0];

}	// M3DCommandResult::getVolumeObj


std::vector<Geom::Surface*> M3DCommandResult::getSurfacesObj ( )
{
    // recherche depuis l'InfoCommand, les surfaces créées
    std::vector<Geom::Surface*> objets;

    InfoCommand& ifc = getInfoCommand();

    std::vector<InfoCommand::GeomEntityInfo>& gei = ifc.getGeomInfoEntities();

    for (std::vector <InfoCommand::GeomEntityInfo>::const_iterator iter = gei.begin();
            iter != gei.end(); ++iter) {
        if (iter->m_geom_entity->getDim() == 2 && iter->m_type == InfoCommand::CREATED){
            Geom::Surface* ob = dynamic_cast<Geom::Surface*>(iter->m_geom_entity);
            if (0 == ob)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getGeomSurfaces", TkUtil::Charset::UTF_8));
            objets.push_back(ob);
        }
    }

    return objets;

}	// M3DCommandResult::getSurfacesObj


Geom::Surface* M3DCommandResult::getSurfaceObj (size_t index)
{
    std::vector<Geom::Surface*> surfaces = getSurfacesObj();

    if (index >= surfaces.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);

        messErr << "getGeomSurface("<<index
                << ") impossible, il n'y a que "<<surfaces.size()
                << " surface[s] dans cette commande";
        throw TkUtil::Exception(messErr);
    }

    return surfaces[index];

}	// M3DCommandResult::getSurfaceObj


Geom::Surface* M3DCommandResult::getSurfaceObj ( )
{
    std::vector<Geom::Surface*> surfaces = getSurfacesObj();

    if (surfaces.size() == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("getGeomSurface impossible, il n'y a pas de surface dans cette commande", TkUtil::Charset::UTF_8));
    else if (surfaces.size() > 1)
        throw TkUtil::Exception(TkUtil::UTF8String ("getGeomSurface impossible, il y a plus d'un surface dans cette commande", TkUtil::Charset::UTF_8));
    else
        return surfaces[0];

}	// M3DCommandResult::getSurfaceObj


std::vector<Geom::Curve*> M3DCommandResult::getCurvesObj ( )
{
    // recherche depuis l'InfoCommand, les courbes créées
    std::vector<Geom::Curve*> objets;

    InfoCommand& ifc = getInfoCommand();

    std::vector<InfoCommand::GeomEntityInfo>& gei = ifc.getGeomInfoEntities();

    for (std::vector <InfoCommand::GeomEntityInfo>::const_iterator iter = gei.begin();
            iter != gei.end(); ++iter) {
        if (iter->m_geom_entity->getDim() == 1 && iter->m_type == InfoCommand::CREATED){
            Geom::Curve* ob = dynamic_cast<Geom::Curve*>(iter->m_geom_entity);
            if (0 == ob)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getGeomCurves", TkUtil::Charset::UTF_8));
            objets.push_back(ob);
        }
    }

    return objets;

}	// M3DCommandResult::getCurvesObj


Geom::Curve* M3DCommandResult::getCurveObj (size_t index)
{
    std::vector<Geom::Curve*> curves = getCurvesObj();

    if (index >= curves.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);

        messErr << "getGeomCurve("<<index
                << ") impossible, il n'y a que "<<curves.size()
                << " courbe[s] dans cette commande";
        throw TkUtil::Exception(messErr);
    }

    return curves[index];

}	// M3DCommandResult::getCurveObj


Geom::Curve* M3DCommandResult::getCurveObj ( )
{
    std::vector<Geom::Curve*> curves = getCurvesObj();

    if (curves.size() == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("getGeomCurve impossible, il n'y a pas de courbe dans cette commande", TkUtil::Charset::UTF_8));
    else if (curves.size() > 1)
        throw TkUtil::Exception(TkUtil::UTF8String ("getGeomCurve impossible, il y a plus d'une courbe dans cette commande", TkUtil::Charset::UTF_8));
    else
        return curves[0];

}	// M3DCommandResult::getCurveObj


std::vector<Geom::Vertex*> M3DCommandResult::getVerticesObj ( )
{
    // recherche depuis l'InfoCommand, les courbes créées
    std::vector<Geom::Vertex*> objets;

    InfoCommand& ifc = getInfoCommand();

    std::vector<InfoCommand::GeomEntityInfo>& gei = ifc.getGeomInfoEntities();

    for (std::vector <InfoCommand::GeomEntityInfo>::const_iterator iter = gei.begin();
            iter != gei.end(); ++iter) {
        if (iter->m_geom_entity->getDim() == 0 && iter->m_type == InfoCommand::CREATED){
            Geom::Vertex* ob = dynamic_cast<Geom::Vertex*>(iter->m_geom_entity);
            if (0 == ob)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getGeomVertices", TkUtil::Charset::UTF_8));
            objets.push_back(ob);
        }
    }

    return objets;

}	// M3DCommandResult::getVerticesObj


Geom::Vertex* M3DCommandResult::getVertexObj (size_t index)
{
    std::vector<Geom::Vertex*> vertices = getVerticesObj();

    if (index >= vertices.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);

        messErr << "getGeomVertex("<<index
                << ") impossible, il n'y a que "<<vertices.size()
                << " sommet[s] dans cette commande";
        throw TkUtil::Exception(messErr);
    }

    return vertices[index];

}	// M3DCommandResult::getVertexObj


Geom::Vertex* M3DCommandResult::getVertexObj ( )
{
    std::vector<Geom::Vertex*> vertices = getVerticesObj();

    if (vertices.size() == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("getGeomVertex impossible, il n'y a pas de sommet dans cette commande", TkUtil::Charset::UTF_8));
    else if (vertices.size() > 1)
        throw TkUtil::Exception(TkUtil::UTF8String ("getGeomVertex impossible, il y a plus d'un sommet dans cette commande", TkUtil::Charset::UTF_8));
    else
        return vertices[0];

}	// M3DCommandResult::getVertexObj


std::vector<Topo::Block*> M3DCommandResult::getBlocksObj ( )
{
	// recherche depuis l'InfoCommand, les blocs créés
	std::list<Topo::Block*> l_te;

	InfoCommand& ifc = getInfoCommand();

	std::map<Topo::TopoEntity*, InfoCommand::type> tei = ifc.getTopoInfoEntity();

	for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::const_iterator iter = tei.begin();
			iter != tei.end(); ++iter) {
		if (iter->first->getDim() == 3 && iter->second == InfoCommand::CREATED){
			Topo::Block* ob = dynamic_cast<Topo::Block*>(iter->first);
			if (0 == ob)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getTopoBlocks", TkUtil::Charset::UTF_8));
			l_te.push_back(ob);
		}
	}

	std::vector<Topo::Block*> objets;

    l_te.sort(Utils::Entity::compareEntity);
    l_te.unique();
    objets.insert(objets.end(), l_te.begin(), l_te.end());

	return objets;

}	// M3DCommandResult::getBlocksObj


Topo::Block* M3DCommandResult::getBlockObj (size_t index)
{
    std::vector<Topo::Block*> objets = getBlocksObj();

    if (index >= objets.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);

        messErr << "getBlock("<<index
                << ") impossible, il n'y a que "<<objets.size()
                << " bloc[s] dans cette commande";
        throw TkUtil::Exception(messErr);
    }

    return objets[index];

}	// M3DCommandResult::getBlockObj


Topo::Block* M3DCommandResult::getBlockObj ( )
{
    std::vector<Topo::Block*> objets = getBlocksObj();

    if (objets.size() == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("getTopoBlock impossible, il n'y a pas de bloc dans cette commande", TkUtil::Charset::UTF_8));
    else if (objets.size() > 1)
        throw TkUtil::Exception(TkUtil::UTF8String ("getTopoBlock impossible, il y a plus d'un bloc dans cette commande", TkUtil::Charset::UTF_8));
    else
        return objets[0];

}	// M3DCommandResult::getBlockObj


std::vector<Topo::CoFace*> M3DCommandResult::getFacesObj ( )
{
	// recherche depuis l'InfoCommand, les faces créées
	std::list<Topo::CoFace*> l_te;

	InfoCommand& ifc = getInfoCommand();

	//std::cout<<"M3DCommandResult::getFaces () avec getInfoCommand() :"<<std::endl;
	//std::cout<<ifc<<std::endl;

	std::map<Topo::TopoEntity*, InfoCommand::type> tei = ifc.getTopoInfoEntity();

	for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::const_iterator iter = tei.begin();
			iter != tei.end(); ++iter) {
		if (iter->first->getDim() == 2 && iter->second == InfoCommand::CREATED){
			Topo::CoFace* ob = dynamic_cast<Topo::CoFace*>(iter->first);
			// on passe les Faces et on ne retourne que les CoFaces
			if (0 != ob)
				l_te.push_back(ob);
		}
	}

	std::vector<Topo::CoFace*> objets;

	l_te.sort(Utils::Entity::compareEntity);
	l_te.unique();
	objets.insert(objets.end(), l_te.begin(), l_te.end());

	return objets;

}	// M3DCommandResult::getFacesObj


Topo::CoFace* M3DCommandResult::getFaceObj (size_t index)
{
    std::vector<Topo::CoFace*> objets = getFacesObj();

    if (index >= objets.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);

        messErr << "getTopoFace("<<index
                << ") impossible, il n'y a que "<<objets.size()
                << " face[s] dans cette commande";
        throw TkUtil::Exception(messErr);
    }

    return objets[index];

}	// M3DCommandResult::getFaceObj


Topo::CoFace* M3DCommandResult::getFaceObj ( )
{
    std::vector<Topo::CoFace*> objets = getFacesObj();

    if (objets.size() == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("getTopoFace impossible, il n'y a pas de face dans cette commande", TkUtil::Charset::UTF_8));
    else if (objets.size() > 1)
        throw TkUtil::Exception(TkUtil::UTF8String ("getTopoFace impossible, il y a plus d'une face dans cette commande", TkUtil::Charset::UTF_8));
    else
        return objets[0];

}	// M3DCommandResult::getFaceObj


std::vector<Topo::CoEdge*> M3DCommandResult::getEdgesObj ( )
{
    // recherche depuis l'InfoCommand, les arêtes créées
	std::list<Topo::CoEdge*> l_te;

	InfoCommand& ifc = getInfoCommand();

	std::map<Topo::TopoEntity*, InfoCommand::type> tei = ifc.getTopoInfoEntity();

	for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::const_iterator iter = tei.begin();
			iter != tei.end(); ++iter) {
		if (iter->first->getDim() == 1 && iter->second == InfoCommand::CREATED){
			Topo::CoEdge* ob = dynamic_cast<Topo::CoEdge*>(iter->first);
			// on passe les Edges et on ne retourne que les CoEdges
			if (0 != ob)
				l_te.push_back(ob);
		}
	}

	std::vector<Topo::CoEdge*> objets;

	l_te.sort(Utils::Entity::compareEntity);
	l_te.unique();
	objets.insert(objets.end(), l_te.begin(), l_te.end());

	return objets;

}	// M3DCommandResult::getEdgesObj


Topo::CoEdge* M3DCommandResult::getEdgeObj (size_t index)
{
    std::vector<Topo::CoEdge*> objets = getEdgesObj();

    if (index >= objets.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);

        messErr << "getTopoEdge("<<index
                << ") impossible, il n'y a que "<<objets.size()
                << " arête[s] dans cette commande";
        throw TkUtil::Exception(messErr);
    }

    return objets[index];

}	// M3DCommandResult::getEdgeObj


Topo::CoEdge* M3DCommandResult::getEdgeObj ( )
{
    std::vector<Topo::CoEdge*> objets = getEdgesObj();

    if (objets.size() == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("getTopoEdge impossible, il n'y a pas d'arête dans cette commande", TkUtil::Charset::UTF_8));
    else if (objets.size() > 1)
        throw TkUtil::Exception(TkUtil::UTF8String ("getTopoEdge impossible, il y a plus d'une arête dans cette commande", TkUtil::Charset::UTF_8));
    else
        return objets[0];

}	// M3DCommandResult::getEdgeObj


std::vector<Topo::Vertex*> M3DCommandResult::getTopoVerticesObj ( )
{
    // recherche depuis l'InfoCommand, les sommets créés
	std::list<Topo::Vertex*> l_te;

	InfoCommand& ifc = getInfoCommand();

	std::map<Topo::TopoEntity*, InfoCommand::type> tei = ifc.getTopoInfoEntity();

	for (std::map<Topo::TopoEntity*, Internal::InfoCommand::type>::const_iterator iter = tei.begin();
			iter != tei.end(); ++iter) {
		if (iter->first->getDim() == 0 && iter->second == InfoCommand::CREATED){
			Topo::Vertex* ob = dynamic_cast<Topo::Vertex*>(iter->first);
			if (0 == ob)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getTopoVerticesObj", TkUtil::Charset::UTF_8));
			l_te.push_back(ob);
		}
	}

	std::vector<Topo::Vertex*> objets;

	l_te.sort(Utils::Entity::compareEntity);
	l_te.unique();
	objets.insert(objets.end(), l_te.begin(), l_te.end());

	return objets;

}	// M3DCommandResult::getTopoVerticesObj


Topo::Vertex* M3DCommandResult::getTopoVertexObj (size_t index)
{
    std::vector<Topo::Vertex*> objets = getTopoVerticesObj();

    if (index >= objets.size()){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);

        messErr << "getTopoVertexObj("<<index
                << ") impossible, il n'y a que "<<objets.size()
                << " arête[s] dans cette commande";
        throw TkUtil::Exception(messErr);
    }

    return objets[index];

}	// M3DCommandResult::getTopoVertexObj


Topo::Vertex* M3DCommandResult::getTopoVertexObj ( )
{
    std::vector<Topo::Vertex*> objets = getTopoVerticesObj();

    if (objets.size() == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("getTopoVertexObj impossible, il n'y a pas de sommet dans cette commande", TkUtil::Charset::UTF_8));
    else if (objets.size() > 1)
        throw TkUtil::Exception(TkUtil::UTF8String ("getTopoVertexObj impossible, il y a plus d'un sommet dans cette commande", TkUtil::Charset::UTF_8));
    else
        return objets[0];

}	// M3DCommandResult::getTopoVertexObj


std::vector<std::string> M3DCommandResult::getVolumes ( )
{
    std::vector<Geom::Volume*> volumes = getVolumesObj();
    std::vector<std::string> names;
    for (std::vector<Geom::Volume*>::iterator iter = volumes.begin(); iter != volumes.end(); ++iter)
        names.push_back((*iter)->getName());
    return names;
}

std::string M3DCommandResult::getVolume (size_t index)
{
	return getVolumeObj(index)->getName();
}

std::string M3DCommandResult::getVolume ( )
{
	return getVolumeObj()->getName();
}


std::vector<std::string> M3DCommandResult::getSurfaces ( )
{
    std::vector<Geom::Surface*> surfaces = getSurfacesObj();
    std::vector<std::string> names;
    for (std::vector<Geom::Surface*>::iterator iter = surfaces.begin(); iter != surfaces.end(); ++iter)
        names.push_back((*iter)->getName());
    return names;
}

std::string M3DCommandResult::getSurface (size_t index)
{
	return getSurfaceObj(index)->getName();
}

std::string M3DCommandResult::getSurface ( )
{
	return getSurfaceObj()->getName();
}


std::vector<std::string> M3DCommandResult::getCurves ( )
{
    std::vector<Geom::Curve*> curves = getCurvesObj();
    std::vector<std::string> names;
    for (std::vector<Geom::Curve*>::iterator iter = curves.begin(); iter != curves.end(); ++iter)
        names.push_back((*iter)->getName());
    return names;
}

std::string M3DCommandResult::getCurve (size_t index)
{
	return getCurveObj(index)->getName();
}

std::string M3DCommandResult::getCurve ( )
{
	return getCurveObj()->getName();
}


std::vector<std::string> M3DCommandResult::getVertices ( )
{
    std::vector<Geom::Vertex*> vertices = getVerticesObj();
    std::vector<std::string> names;
    for (std::vector<Geom::Vertex*>::iterator iter = vertices.begin(); iter != vertices.end(); ++iter)
        names.push_back((*iter)->getName());
    return names;
}

std::string M3DCommandResult::getVertex (size_t index)
{
	return getVertexObj(index)->getName();
}

std::string M3DCommandResult::getVertex ( )
{
	return getVertexObj()->getName();
}



std::vector<std::string> M3DCommandResult::getBlocks ( )
{
    std::vector<Topo::Block*> blocs = getBlocksObj();
    std::vector<std::string> names;
    for (std::vector<Topo::Block*>::iterator iter = blocs.begin(); iter != blocs.end(); ++iter)
        names.push_back((*iter)->getName());
    return names;
}

std::string M3DCommandResult::getBlock (size_t index)
{
	return getBlockObj(index)->getName();
}

std::string M3DCommandResult::getBlock ( )
{
	return getBlockObj()->getName();
}


std::vector<std::string> M3DCommandResult::getFaces ( )
{
    std::vector<Topo::CoFace*> surfaces = getFacesObj();
    std::vector<std::string> names;
    for (std::vector<Topo::CoFace*>::iterator iter = surfaces.begin(); iter != surfaces.end(); ++iter)
        names.push_back((*iter)->getName());
    return names;
}

std::string M3DCommandResult::getFace (size_t index)
{
	return getFaceObj(index)->getName();
}

std::string M3DCommandResult::getFace ( )
{
	return getFaceObj()->getName();
}


std::vector<std::string> M3DCommandResult::getEdges ( )
{
    std::vector<Topo::CoEdge*> edges = getEdgesObj();
    std::vector<std::string> names;
    for (std::vector<Topo::CoEdge*>::iterator iter = edges.begin(); iter != edges.end(); ++iter)
        names.push_back((*iter)->getName());
    return names;
}

std::string M3DCommandResult::getEdge (size_t index)
{
	return getEdgeObj(index)->getName();
}

std::string M3DCommandResult::getEdge ( )
{
	return getEdgeObj()->getName();
}


std::vector<std::string> M3DCommandResult::getTopoVertices ( )
{
    std::vector<Topo::Vertex*> vertices = getTopoVerticesObj();
    std::vector<std::string> names;
    for (std::vector<Topo::Vertex*>::iterator iter = vertices.begin(); iter != vertices.end(); ++iter)
        names.push_back((*iter)->getName());
    return names;
}

std::string M3DCommandResult::getTopoVertex (size_t index)
{
	return getTopoVertexObj(index)->getName();
}

std::string M3DCommandResult::getTopoVertex ( )
{
	return getTopoVertexObj()->getName();
}


std::vector<Mgx3D::Mesh::Volume*> M3DCommandResult::getMeshVolumesObj ( )
{
	std::vector<Mesh::Volume*> objets;

	// recherche depuis l'InfoCommand, les volumes créés
	InfoCommand& ifc = getInfoCommand();

	std::vector<InfoCommand::MeshEntityInfo>& mei = ifc.getMeshInfoEntities();

	for (std::vector <InfoCommand::MeshEntityInfo>::const_iterator iter = mei.begin();
			iter != mei.end(); ++iter) {
		if (iter->m_mesh_entity->getDim() == 3 && iter->m_type == InfoCommand::CREATED){
			Mesh::Volume* ob = dynamic_cast<Mesh::Volume*>(iter->m_mesh_entity);
			if (0 == ob)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getMeshVolumesObj", TkUtil::Charset::UTF_8));
			objets.push_back(ob);
		}
	}

	return objets;
}

std::vector<Mgx3D::Mesh::Surface*> M3DCommandResult::getMeshSurfacesObj ( )
{
	std::vector<Mesh::Surface*> objets;

	// recherche depuis l'InfoCommand, les surfaces créées
	InfoCommand& ifc = getInfoCommand();

	std::vector<InfoCommand::MeshEntityInfo>& mei = ifc.getMeshInfoEntities();

	for (std::vector <InfoCommand::MeshEntityInfo>::const_iterator iter = mei.begin();
			iter != mei.end(); ++iter) {
		if (iter->m_mesh_entity->getDim() == 2 && iter->m_type == InfoCommand::CREATED){
			Mesh::Surface* ob = dynamic_cast<Mesh::Surface*>(iter->m_mesh_entity);
			if (0 == ob)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getMeshSurfacesObj", TkUtil::Charset::UTF_8));
			objets.push_back(ob);
		}
	}

	return objets;
}

std::vector<Mgx3D::Mesh::Line*> M3DCommandResult::getMeshLinesObj ( )
{
	std::vector<Mesh::Line*> objets;

	// recherche depuis l'InfoCommand, les lignes créées
	InfoCommand& ifc = getInfoCommand();

	std::vector<InfoCommand::MeshEntityInfo>& mei = ifc.getMeshInfoEntities();

	for (std::vector <InfoCommand::MeshEntityInfo>::const_iterator iter = mei.begin();
			iter != mei.end(); ++iter) {
		if (iter->m_mesh_entity->getDim() == 1 && iter->m_type == InfoCommand::CREATED){
			Mesh::Line* ob = dynamic_cast<Mesh::Line*>(iter->m_mesh_entity);
			if (0 == ob)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getMeshLinesObj", TkUtil::Charset::UTF_8));
			objets.push_back(ob);
		}
	}

	return objets;
}


std::vector<Mgx3D::Mesh::Cloud*> M3DCommandResult::getMeshCloudsObj ( )
{
	std::vector<Mesh::Cloud*> objets;

	// recherche depuis l'InfoCommand, les nuages créés
	InfoCommand& ifc = getInfoCommand();

	std::vector<InfoCommand::MeshEntityInfo>& mei = ifc.getMeshInfoEntities();

	for (std::vector <InfoCommand::MeshEntityInfo>::const_iterator iter = mei.begin();
			iter != mei.end(); ++iter) {
		if (iter->m_mesh_entity->getDim() == 0 && iter->m_type == InfoCommand::CREATED){
			Mesh::Cloud* ob = dynamic_cast<Mesh::Cloud*>(iter->m_mesh_entity);
			if (0 == ob)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getMeshCloudsObj", TkUtil::Charset::UTF_8));
			objets.push_back(ob);
		}
	}

	return objets;
}


std::vector<Mgx3D::Group::Group0D*> M3DCommandResult::getGroups0DObj ( )
{
	// recherche depuis l'InfoCommand, les blocs créés
	std::list<Group::Group0D*> l_gr;

	InfoCommand& ifc = getInfoCommand();

	std::map<Group::GroupEntity*, Internal::InfoCommand::type>& gei = ifc.getGroupInfoEntity();

	for (std::map<Group::GroupEntity*, Internal::InfoCommand::type>::const_iterator iter = gei.begin();
			iter != gei.end(); ++iter) {
		if (iter->first->getDim() == 0 && iter->second == InfoCommand::CREATED){
			Group::Group0D* ob = dynamic_cast<Group::Group0D*>(iter->first);
			if (0 == ob)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getGroups0DObj", TkUtil::Charset::UTF_8));
			l_gr.push_back(ob);
		}
	}

	std::vector<Group::Group0D*> objets;

	l_gr.sort(Utils::Entity::compareEntity);
	l_gr.unique();
    objets.insert(objets.end(), l_gr.begin(), l_gr.end());

	return objets;
}

std::vector<std::string> M3DCommandResult::getGroups0D ( )
{
    std::vector<Group::Group0D*> groups = getGroups0DObj();
    std::vector<std::string> names;
    for (std::vector<Group::Group0D*>::iterator iter = groups.begin(); iter != groups.end(); ++iter)
        names.push_back((*iter)->getName());
    return names;
}

std::vector<Mgx3D::Group::Group1D*> M3DCommandResult::getGroups1DObj ( )
{
	// recherche depuis l'InfoCommand, les blocs créés
	std::list<Group::Group1D*> l_gr;

	InfoCommand& ifc = getInfoCommand();

	std::map<Group::GroupEntity*, Internal::InfoCommand::type>& gei = ifc.getGroupInfoEntity();

	for (std::map<Group::GroupEntity*, Internal::InfoCommand::type>::const_iterator iter = gei.begin();
			iter != gei.end(); ++iter) {
		if (iter->first->getDim() == 1 && iter->second == InfoCommand::CREATED){
			Group::Group1D* ob = dynamic_cast<Group::Group1D*>(iter->first);
			if (0 == ob)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getGroups1DObj", TkUtil::Charset::UTF_8));
			l_gr.push_back(ob);
		}
	}

	std::vector<Group::Group1D*> objets;

	l_gr.sort(Utils::Entity::compareEntity);
	l_gr.unique();
    objets.insert(objets.end(), l_gr.begin(), l_gr.end());

	return objets;
}

std::vector<std::string> M3DCommandResult::getGroups1D ( )
{
    std::vector<Group::Group1D*> groups = getGroups1DObj();
    std::vector<std::string> names;
    for (std::vector<Group::Group1D*>::iterator iter = groups.begin(); iter != groups.end(); ++iter)
        names.push_back((*iter)->getName());
    return names;
}


std::vector<Mgx3D::Group::Group2D*> M3DCommandResult::getGroups2DObj ( )
{
	// recherche depuis l'InfoCommand, les blocs créés
	std::list<Group::Group2D*> l_gr;

	InfoCommand& ifc = getInfoCommand();

	std::map<Group::GroupEntity*, Internal::InfoCommand::type>& gei = ifc.getGroupInfoEntity();

	for (std::map<Group::GroupEntity*, Internal::InfoCommand::type>::const_iterator iter = gei.begin();
			iter != gei.end(); ++iter) {
		if (iter->first->getDim() == 2 && iter->second == InfoCommand::CREATED){
			Group::Group2D* ob = dynamic_cast<Group::Group2D*>(iter->first);
			if (0 == ob)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getGroups2DObj", TkUtil::Charset::UTF_8));
			l_gr.push_back(ob);
		}
	}

	std::vector<Group::Group2D*> objets;

	l_gr.sort(Utils::Entity::compareEntity);
	l_gr.unique();
    objets.insert(objets.end(), l_gr.begin(), l_gr.end());

	return objets;
}

std::vector<std::string> M3DCommandResult::getGroups2D ( )
{
    std::vector<Group::Group2D*> groups = getGroups2DObj();
    std::vector<std::string> names;
    for (std::vector<Group::Group2D*>::iterator iter = groups.begin(); iter != groups.end(); ++iter)
        names.push_back((*iter)->getName());
    return names;
}


std::vector<Mgx3D::Group::Group3D*> M3DCommandResult::getGroups3DObj ( )
{
	// recherche depuis l'InfoCommand, les blocs créés
	std::list<Group::Group3D*> l_gr;

	InfoCommand& ifc = getInfoCommand();

	std::map<Group::GroupEntity*, Internal::InfoCommand::type>& gei = ifc.getGroupInfoEntity();

	for (std::map<Group::GroupEntity*, Internal::InfoCommand::type>::const_iterator iter = gei.begin();
			iter != gei.end(); ++iter) {
		if (iter->first->getDim() == 3 && iter->second == InfoCommand::CREATED){
			Group::Group3D* ob = dynamic_cast<Group::Group3D*>(iter->first);
			if (0 == ob)
				throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, echec pour dynamic_cast dans getGroups3DObj", TkUtil::Charset::UTF_8));
			l_gr.push_back(ob);
		}
	}

	std::vector<Group::Group3D*> objets;

	l_gr.sort(Utils::Entity::compareEntity);
	l_gr.unique();
    objets.insert(objets.end(), l_gr.begin(), l_gr.end());

	return objets;
}

std::vector<std::string> M3DCommandResult::getGroups3D ( )
{
    std::vector<Group::Group3D*> groups = getGroups3DObj();
    std::vector<std::string> names;
    for (std::vector<Group::Group3D*>::iterator iter = groups.begin(); iter != groups.end(); ++iter)
        names.push_back((*iter)->getName());
    return names;
}

std::string M3DCommandResult::getSysCoord ( )
{
	return getSysCoordObj()->getName();
}

CoordinateSystem::SysCoord* M3DCommandResult::getSysCoordObj ( )
{
    // recherche depuis l'InfoCommand, le repère créé
	InfoCommand& ifc = getInfoCommand();

	int nb = ifc.getNbSysCoordInfoEntity();

	for (uint i=0; i<nb; i++) {
		CoordinateSystem::SysCoord* rep = 0;
		Internal::InfoCommand::type t;
		ifc.getSysCoordInfoEntity(i, rep, t);

		if (t == InfoCommand::CREATED)
			return rep;
	}

	throw TkUtil::Exception(TkUtil::UTF8String ("getSysCoord impossible, il n'y a pas de repère créé dans cette commande", TkUtil::Charset::UTF_8));

}	// M3DCommandResult::getSysCoordObj


CommandInternal& M3DCommandResult::getCommandInternal( )
{
	if (0 == _commandInternal)
		throw Exception (TkUtil::UTF8String ("M3DCommandResult::getCommandInternal : absence de commande associée.", TkUtil::Charset::UTF_8));

	return *_commandInternal;

}	// M3DCommandResult::getCommand


InfoCommand& M3DCommandResult::getInfoCommand()
{
    if (Mgx3D::Utils::CommandIfc::DONE == getStatus())
            return getCommandInternal().getInfoCommand();
        else {
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
            messErr <<"M3DCommandResult::getInfoCommand : status de la commande autre que terminée ("
                    << getStrStatus() <<") pour "<<_commandInternal->getName();
            throw TkUtil::Exception(messErr);
        }
}


/*----------------------------------------------------------------------------*/

}	// namespace Internal
/*----------------------------------------------------------------------------*/

}	// namespace Mgx3D
/*----------------------------------------------------------------------------*/

