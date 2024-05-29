/*----------------------------------------------------------------------------*/
/** \file CommandSetGeomAssociation.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 07/03/2011
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandSetGeomAssociation.h"
#include "Geom/EntityFactory.h"
#include "Topo/TopoManager.h"
#include "Topo/Block.h"
#include "Topo/TopoHelper.h"
#include "Topo/FaceMeshingPropertyTransfinite.h"

#include "Utils/Common.h"
#include "Internal/Context.h"
#include "Geom/CommandCreateGeom.h"
#include "Geom/CommandEditGeom.h"
#include "Geom/GeomModificationBaseClass.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandSetGeomAssociation::
CommandSetGeomAssociation(Internal::Context& c, std::vector<TopoEntity*> &topoentities, Geom::CommandCreateGeom* command_geom)
:CommandEditTopo(c, "Affectation d'une projection pour des entités topologiques")
, m_topo_entities(topoentities.begin(), topoentities.end())
, m_geom_entity(0)
, m_command_create_geom(command_geom)
, m_command_edit_geom(0)
, m_move_vertices(false)
{
	if (topoentities.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("L'association ne peut se faire sans rien", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Affectation d'une projection pour les entités";
	for (uint i=0; i<topoentities.size() && i<5; i++)
		comments << " " << topoentities[i]->getName();
	if (topoentities.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSetGeomAssociation::
CommandSetGeomAssociation(Internal::Context& c, Geom::CommandEditGeom* command_geom)
:CommandEditTopo(c, "Suppressions de projections pour des entités topologiques")
, m_topo_entities()
, m_geom_entity(0)
, m_command_create_geom(0)
, m_command_edit_geom(command_geom)
, m_move_vertices(false)
{

}
/*----------------------------------------------------------------------------*/
CommandSetGeomAssociation::
CommandSetGeomAssociation(Internal::Context& c,
		std::vector<TopoEntity*> &topo_entities,
		Geom::GeomEntity* geom_entity,
		bool move_vertices)
:CommandEditTopo(c, "Affectation d'une projection pour des entités topologiques")
, m_topo_entities(topo_entities.begin(), topo_entities.end())
, m_geom_entity(geom_entity)
, m_command_create_geom(0)
, m_command_edit_geom(0)
, m_move_vertices(move_vertices)
{
	if (topo_entities.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("L'association ne peut se faire sans rien", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);

	if (geom_entity == 0)
		comments << "Suppression des projections pour les entités topologiques";
	else
		comments << "Affectation d'une projection vers "<<geom_entity->getName()<<" pour les entités topologiques";

	for (uint i=0; i<topo_entities.size() && i<5; i++)
		comments << " " << topo_entities[i]->getName();
	if (topo_entities.size()>5)
		comments << " ... ";

	setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSetGeomAssociation::
~CommandSetGeomAssociation()
{}
/*----------------------------------------------------------------------------*/
void CommandSetGeomAssociation::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandSetGeomAssociation::execute pour la commande " << getName ( )
		    << " de nom unique " << getUniqueName ( );

	// récupération si nécessaire et validation du type de géométrie
	validGeomEntity();

	// modification de la projection, avec gestion du undo/redo
	for (std::vector<TopoEntity*>::iterator iter = m_topo_entities.begin();
	        iter != m_topo_entities.end(); ++iter){
#ifdef _DEBUG2
		std::cout<<"CommandSetGeomAssociation => project("<<(*iter)->getName()<<")"<<std::endl;
#endif

		// vérifie qu'un même groupe n'est pas référencé depuis la géométrie et depuis la topologie
		validGroupsName(*iter);

		if ((*iter)->getDim() == 3)
			project(dynamic_cast<Block*>(*iter));
		else if ((*iter)->getDim() == 2)
			project(dynamic_cast<CoFace*>(*iter));
	    else if ((*iter)->getDim() == 1)
	    	project(dynamic_cast<CoEdge*>(*iter));
	    else if ((*iter)->getDim() == 0)
	    	project(dynamic_cast<Vertex*>(*iter));
	}

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandSetGeomAssociation::validGeomEntity()
{
    // cas où on utilise une commande pour y récupérer l'entité géométrique créée
    if (m_command_create_geom){
        std::vector<Geom::GeomEntity*> res;
        m_command_create_geom->getResult(res);
        if (res.empty())
        	throw TkUtil::Exception (TkUtil::UTF8String ("Commande de création d'entité ne retourne rien pour CommandSetGeomAssociation", TkUtil::Charset::UTF_8));
        m_geom_entity = res[0];
    }
    if (m_command_edit_geom){

    	std::vector<Geom::GeomEntity*> entities = m_command_edit_geom->getRemovedEntities();

    	m_topo_entities = Topo::TopoHelper::getTopoEntities(entities);
    }
}
/*----------------------------------------------------------------------------*/
void CommandSetGeomAssociation::validGroupsName(TopoEntity* te)
{

	if (te == 0 || m_geom_entity == 0)
		return;

	std::vector<std::string> te_gn;
	te->getGroupsName(te_gn, false, true);

	std::vector<std::string> ge_gn;
	m_geom_entity->getGroupsName(ge_gn);

#ifdef _DEBUG2
	std::cout<<"validGroupsName("<<te->getName()<<")"<<std::endl;
	std::cout<<"te_gn :";
	for (uint i=0; i<te_gn.size(); i++)
		std::cout<<" "<<te_gn[i];
	std::cout<<std::endl;
	std::cout<<"ge_gn :";
	for (uint i=0; i<ge_gn.size(); i++)
		std::cout<<" "<<ge_gn[i];
	std::cout<<std::endl;
#endif

	for (std::vector<std::string>::iterator iter_t = te_gn.begin();
			iter_t != te_gn.end(); ++iter_t){
		std::vector<std::string>::iterator iter_g = std::find(ge_gn.begin(), ge_gn.end(), *iter_t);
		if (iter_g!=ge_gn.end()){
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	        message << "L'association ne peut se faire car le groupe "<<*iter_g
	                <<" possède "<<m_geom_entity->getName()<<" ainsi que "<<te->getName();
	        message << "\nIl est recommandé de supprimer la topologie du groupe";
	        throw TkUtil::Exception (message);
		}
	}
}
/*----------------------------------------------------------------------------*/
void CommandSetGeomAssociation::project(Block* bloc)
{
	getInfoCommand().addTopoInfoEntity(bloc, Internal::InfoCommand::DISPMODIFIED);
	bloc->saveTopoProperty();
	bloc->setGeomAssociation(m_geom_entity);
}
/*----------------------------------------------------------------------------*/
void CommandSetGeomAssociation::project(CoFace* coface)
{
	getInfoCommand().addTopoInfoEntity(coface, Internal::InfoCommand::DISPMODIFIED);
	coface->saveTopoProperty();
	coface->setGeomAssociation(m_geom_entity);

	std::vector<CoEdge*> coedges;
	coface->getCoEdges(coedges);

	// on passe en mode transfini dès lors que l'on change l'association
	if (coface->getMeshLaw() < Topo::CoFaceMeshingProperty::transfinite){
		coface->saveCoFaceMeshingProperty(&getInfoCommand());
		CoFaceMeshingProperty* prop = new FaceMeshingPropertyTransfinite();
		coface->switchCoFaceMeshingProperty(&getInfoCommand(), prop);
		delete prop;
	}

	for (std::vector<CoEdge*>::iterator iter = coedges.begin();
			iter != coedges.end(); ++iter){
		CoEdge* coedge = *iter;

		if (m_geom_entity){
			if ((coedge->getGeomAssociation() == 0 || coedge->getGeomAssociation()->isDestroyed()))
				project(coedge);

			// on cherche à projeter les arêtes sur les courbes de la surface autant que possible
			if (coedge->getNbVertices() == 2){
				if (coedge->getVertex(0)->getGeomAssociation() && coedge->getVertex(0)->getGeomAssociation()->getDim()==0
						&& coedge->getVertex(1)->getGeomAssociation() && coedge->getVertex(1)->getGeomAssociation()->getDim()==0){

					// recherche s'il y a une courbe entre ces 2 sommets
					Geom::Vertex* vtx1 = dynamic_cast<Geom::Vertex*>(coedge->getVertex(0)->getGeomAssociation());
					Geom::Vertex* vtx2 = dynamic_cast<Geom::Vertex*>(coedge->getVertex(1)->getGeomAssociation());
					CHECK_NULL_PTR_ERROR(vtx1);
					CHECK_NULL_PTR_ERROR(vtx2);

					Geom::Curve* common_curve = 0;
					std::vector<Geom::Curve*> curves;
					vtx1->get(curves);
					for (uint i=0; i<curves.size(); i++){
						std::vector<Geom::Vertex*> vertices;
						curves[i]->get(vertices);
						for (uint j=0; j<vertices.size(); j++)
							if (vertices[j] == vtx2)
								common_curve = curves[i];
					}

					if (common_curve){
						getInfoCommand().addTopoInfoEntity(coedge, Internal::InfoCommand::DISPMODIFIED);
						coedge->saveTopoProperty();
						coedge->setGeomAssociation(common_curve);
					}
				}
			}
		} // if (m_geom_entity)
	} // end for iter
}
/*----------------------------------------------------------------------------*/
void CommandSetGeomAssociation::project(CoEdge* coedge)
{
	getInfoCommand().addTopoInfoEntity(coedge, Internal::InfoCommand::DISPMODIFIED);
	coedge->saveTopoProperty();
	coedge->setGeomAssociation(m_geom_entity);

	std::vector<Vertex*> vertices;
	coedge->getVertices(vertices);

	for (std::vector<Vertex*>::iterator iter = vertices.begin();
			iter != vertices.end(); ++iter){
		Vertex* vtx = *iter;

		// on propage si on projète sur une entité de dimension inférieure à la projection précédente
		if (m_geom_entity
				&& (vtx->getGeomAssociation() == 0 || vtx->getGeomAssociation()->isDestroyed()
						|| (vtx->getGeomAssociation()->getDim() == 2 && m_geom_entity->getDim() == 1)))
			project(vtx);
	}

}
/*----------------------------------------------------------------------------*/
void CommandSetGeomAssociation::project(Vertex* vtx)
{
	vtx->saveTopoProperty();
	vtx->setGeomAssociation(m_geom_entity);

	// déplace le sommet topologique si la projection implique un déplacement
	if (m_geom_entity && m_move_vertices){
		Utils::Math::Point posTopo = vtx->getCoord();
		Utils::Math::Point posGeom;
		m_geom_entity->project(posTopo, posGeom);
		if (!(posTopo == posGeom)){
			vtx->saveVertexGeomProperty(&getInfoCommand(), true);
			vtx->setCoord(posGeom);
		}

		// si on se retrouve à proximité d'un sommet géométrique
		// on peut alors associer ce sommet topo à ce dernier
		std::vector<Geom::Vertex*> vertices;
		m_geom_entity->get(vertices);
		for (uint i=0; i<vertices.size(); i++){
			Utils::Math::Point posVtx = vertices[i]->getCenteredPosition();
			if (posGeom.isEpsilonEqual(posVtx, Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon))
				vtx->setGeomAssociation(vertices[i]);
		}
	}
	getInfoCommand().addTopoInfoEntity(vtx, Internal::InfoCommand::DISPMODIFIED);
}
/*----------------------------------------------------------------------------*/
void CommandSetGeomAssociation::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
