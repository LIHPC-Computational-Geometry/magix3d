/*----------------------------------------------------------------------------*/
/** \file CommandNewTopoOnGeometry.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 07/03/2011
 */
/*----------------------------------------------------------------------------*/
#include "Geom/EntityFactory.h"
#include "Topo/CommandNewTopoOnGeometry.h"
#include "Topo/TopoManager.h"
#include "Topo/Block.h"
#include "Topo/CoFace.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Utils/Common.h"
#include "Internal/ServiceGeomToTopo.h"
#include "Internal/Context.h"
#include "Group/Group3D.h"
#include "Group/Group2D.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandNewTopoOnGeometry::
CommandNewTopoOnGeometry(Internal::Context& c, Geom::GeomEntity* entity, eTopoType topoType)
:CommandCreateTopo(c, "Nom de commande à définir")
, m_freeTopo(false)
, m_dim(0)
, m_insertionBlock(false)
, m_ni(0)
, m_nj(0)
, m_nk(0)
{
    setGeomEntity(entity);

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);

    if (entity == 0 && topoType != FREE_BLOCK)
    	throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOnGeometry sans entité géométrique ne peut se faire qu'avec le type FREE_BLOCK", TkUtil::Charset::UTF_8));

    switch (topoType){
    case UNSTRUCTURED_BLOCK:
    	setStructured(false);
    	if (entity->getDim() == 3)
    		comments << "Création d'un bloc topologique non structuré sur une géométrie";
    	else if (entity->getDim() == 2)
    		comments << "Création d'une face topologique non structurée sur une géométrie";
    	else
    		comments << "Création d'une topologie non structurée sur une géométrie";
    	break;
    case STRUCTURED_BLOCK:
    	setStructured(true);
    	if (entity->getDim() == 3)
    		comments << "Création d'un bloc topologique structuré sur une géométrie";
    	else if (entity->getDim() == 2)
    		comments << "Création d'une face topologique structurée sur une géométrie";
    	else
    		comments << "Création d'une topologie structurée sur une géométrie";
    	break;
    case FREE_BLOCK:
    	setStructured(true);
    	if (entity==0 || entity->getDim() == 3){
    		comments << "Création d'un bloc topologique structuré sans projection";
    		m_dim = 3;
    	}
    	else if (entity->getDim() == 2) {
    		comments << "Création d'une face topologique structurée sans projection";
    		m_dim = 2;
    	}
    	else {
    		comments << "Création d'une topologie structurée sans projection";
    		m_dim = 3; // un bloc par défaut
    	}
    	m_freeTopo = true;
    	break;
    case INSERTION_BLOCK:
    	setStructured(false);
    	if (entity==0 || entity->getDim() == 3)
    		comments << "Création d'un bloc topologique pour l'insertion";
    	else if (entity->getDim() == 2)
    		comments << "Création d'une face topologique pour l'insertion";
    	else
    		comments << "Création d'une topologie pour l'insertion";
    	m_insertionBlock = true;
    	break;
    case ASSOCIATED_TOPO:
    	setStructured(false);
    	if (entity!=0 && entity->getDim() == 0){
    		comments << "Création d'un sommet topologique";
    		m_dim = 0;
    	}
    	else if (entity!=0 && entity->getDim() == 1) {
    		comments << "Création d'une arête topologique";
    		m_dim = 2;
    	}
    	else {
    		throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOnGeometry pour un cas non prévu", TkUtil::Charset::UTF_8));
    	}
    	m_freeTopo = false;
    	break;
    default:
    	throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOnGeometry pour un type de topologie non prévu", TkUtil::Charset::UTF_8));
    }
    if (entity)
    	comments<<" ("<<entity->getName()<<")";

    setScriptComments(comments);
    setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandNewTopoOnGeometry::
CommandNewTopoOnGeometry(Internal::Context& c, Geom::CommandCreateGeom* command_geom, eTopoType topoType)
:CommandCreateTopo(c, "Création d'une topologique sur une géométrie depuis une commande")
, m_freeTopo(false)
, m_dim(0)
, m_insertionBlock(false)
, m_ni(0)
, m_nj(0)
, m_nk(0)
{
    setCommandCreateGeom(command_geom);

    switch (topoType){
    case UNSTRUCTURED_BLOCK:
    	setStructured(false);
    	break;
    case STRUCTURED_BLOCK:
    	setStructured(true);
    	break;
    case FREE_BLOCK:
    	setStructured(true);
    	m_freeTopo = true;
    	break;
    case INSERTION_BLOCK:
    	setStructured(false);
    	m_insertionBlock = true;
    	break;
    default:
    	throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOnGeometry pour un type de topologie non prévu", TkUtil::Charset::UTF_8));
    }
    setName("Création d'une topologique sur une géométrie depuis une commande");
}
/*----------------------------------------------------------------------------*/
CommandNewTopoOnGeometry::
CommandNewTopoOnGeometry(Internal::Context& c, const std::string& ng, const short& dim)
:CommandCreateTopo(c, "")
, m_freeTopo(true)
, m_dim(dim)
, m_insertionBlock(false)
, m_groupName(ng)
, m_ni(0)
, m_nj(0)
, m_nk(0)
{
	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Création d'un bloc unitaire mis dans le groupe " << ng;
	setScriptComments(comments);
	setName(std::string("Création d'un bloc unitaire mis dans le groupe ")+ng);
}
/*----------------------------------------------------------------------------*/
CommandNewTopoOnGeometry::
CommandNewTopoOnGeometry(Internal::Context& c, Geom::CommandCreateGeom* command_geom, const int ni, const int nj, const int nk)
:CommandCreateTopo(c, "Création d'une topologique (ni nj nk) sur une géométrie depuis une commande")
, m_freeTopo(false)
, m_dim(0)
, m_insertionBlock(false)
, m_ni(ni)
, m_nj(nj)
, m_nk(nk)
{
    setCommandCreateGeom(command_geom);
    setStructured(true);
    setName("Création d'une topologique (ni nj nk) sur une géométrie depuis une commande");
}
/*----------------------------------------------------------------------------*/
CommandNewTopoOnGeometry::
CommandNewTopoOnGeometry(Internal::Context& c, Geom::Surface* entity, std::vector<Geom::Vertex*>& vertices)
:CommandCreateTopo(c, "Création d'une face topologique structurée sur une géométrie")
, m_freeTopo(false)
, m_dim(0)
, m_insertionBlock(false)
, m_ni(0)
, m_nj(0)
, m_nk(0)
{
	setStructured(true);
	setGeomEntity(entity);
	m_extrem_vertices.insert(m_extrem_vertices.end(), vertices.begin(), vertices.end());
}
/*----------------------------------------------------------------------------*/
CommandNewTopoOnGeometry::
~CommandNewTopoOnGeometry()
{}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOnGeometry::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandNewTopoOnGeometry::execute pour la commande " << getName ( )
		    << " de nom unique " << getUniqueName ( )
		    <<(isStructured()?", en mode structuré":", en mode non-structuré");

	// récupération si nécessaire et validation du type de géométrie
	validGeomEntity();


	// cas d'un bloc ou d'une face simple
	if (m_freeTopo){

		// création d'une CoFace dans le cas d'une surface
		if (m_dim == 2 || (getGeomEntity() && getGeomEntity()->getDim() == 2)){
			// création d'une simple coface
			CoFace* cf = new CoFace(getContext());
			addCreatedCoFace(cf);
			if (!m_groupName.empty()){
		    	Group::Group2D* grp = getContext().getLocalGroupManager().getNewGroup2D(m_groupName, &getInfoCommand());
				grp->add(cf);
				cf->getGroupsContainer().add(grp);
			}

			 if (getGeomEntity()){
				 cf->setGeomAssociation(getGeomEntity());

				 // on place la face suivant la boite englobante de la surface
				 Utils::Math::Point pmin;
				 Utils::Math::Point pmax;
				 getGeomEntity()->computeBoundingBox(pmin, pmax);
//				 std::cout<<"pmax-pmin : "<<pmax-pmin<<std::endl;

				 // on cherche si la boite est platte
				 if (std::fabs(pmin.getZ()-pmax.getZ())<Mgx3D::Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon){
					 cf->getVertex(0)->setCoord(Utils::Math::Point(pmin.getX(), pmax.getY(), pmin.getZ()));
					 cf->getVertex(1)->setCoord(Utils::Math::Point(pmin.getX(), pmin.getY(), pmin.getZ()));
					 cf->getVertex(2)->setCoord(Utils::Math::Point(pmax.getX(), pmin.getY(), pmin.getZ()));
					 cf->getVertex(3)->setCoord(Utils::Math::Point(pmax.getX(), pmax.getY(), pmin.getZ()));
				 }
				 else if (std::fabs(pmin.getY()-pmax.getY())<Mgx3D::Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon){
					 cf->getVertex(0)->setCoord(Utils::Math::Point(pmin.getX(), pmin.getY(), pmax.getZ()));
					 cf->getVertex(1)->setCoord(Utils::Math::Point(pmin.getX(), pmin.getY(), pmin.getZ()));
					 cf->getVertex(2)->setCoord(Utils::Math::Point(pmax.getX(), pmin.getY(), pmin.getZ()));
					 cf->getVertex(3)->setCoord(Utils::Math::Point(pmax.getX(), pmin.getY(), pmax.getZ()));
				 }
				 else if (std::fabs(pmin.getX()-pmax.getX())<Mgx3D::Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon){
					 cf->getVertex(0)->setCoord(Utils::Math::Point(pmin.getX(), pmin.getY(), pmax.getZ()));
					 cf->getVertex(1)->setCoord(Utils::Math::Point(pmin.getX(), pmin.getY(), pmin.getZ()));
					 cf->getVertex(2)->setCoord(Utils::Math::Point(pmin.getX(), pmax.getY(), pmin.getZ()));
					 cf->getVertex(3)->setCoord(Utils::Math::Point(pmin.getX(), pmax.getY(), pmax.getZ()));
				 }
				 else {
					 cf->getVertex(0)->setCoord(Utils::Math::Point(pmin.getX(), pmin.getY(), pmax.getZ()));
					 cf->getVertex(1)->setCoord(Utils::Math::Point(pmin.getX(), pmin.getY(), pmin.getZ()));
					 cf->getVertex(2)->setCoord(Utils::Math::Point(pmax.getX(), pmax.getY(), pmin.getZ()));
					 cf->getVertex(3)->setCoord(Utils::Math::Point(pmax.getX(), pmax.getY(), pmax.getZ()));
				 }
			 }
		}
		else {
			// création d'un simple bloc
			Block* bl = new Block(getContext(), 0,0,0,BlockMeshingProperty::transfinite);
			addCreatedBlock(bl);
			if (!m_groupName.empty()){
		    	Group::Group3D* grp = getContext().getLocalGroupManager().getNewGroup3D(m_groupName, &getInfoCommand());
				grp->add(bl);
				bl->getGroupsContainer().add(grp);
			}

			if (getGeomEntity()){
				bl->setGeomAssociation(getGeomEntity());

				// on place le bloc suivant la boite englobante du volume
				Utils::Math::Point pmin;
				Utils::Math::Point pmax;
				getGeomEntity()->computeBoundingBox(pmin, pmax);

				bl->getVertex(false, false, false)->setCoord(Utils::Math::Point(pmin.getX(), pmin.getY(), pmin.getZ()));
				bl->getVertex(true,  false, false)->setCoord(Utils::Math::Point(pmax.getX(), pmin.getY(), pmin.getZ()));
				bl->getVertex(false, true,  false)->setCoord(Utils::Math::Point(pmin.getX(), pmax.getY(), pmin.getZ()));
				bl->getVertex(true,  true,  false)->setCoord(Utils::Math::Point(pmax.getX(), pmax.getY(), pmin.getZ()));
				bl->getVertex(false, false, true )->setCoord(Utils::Math::Point(pmin.getX(), pmin.getY(), pmax.getZ()));
				bl->getVertex(true,  false, true )->setCoord(Utils::Math::Point(pmax.getX(), pmin.getY(), pmax.getZ()));
				bl->getVertex(false, true,  true )->setCoord(Utils::Math::Point(pmin.getX(), pmax.getY(), pmax.getZ()));
				bl->getVertex(true,  true,  true )->setCoord(Utils::Math::Point(pmax.getX(), pmax.getY(), pmax.getZ()));
			}
		}
		// stockage dans le InfoCommand des différentes entités (bloc et niveau inférieur)
	    split();

	} // end if (m_freeTopo)
	else {
		if (getGeomEntity() == 0)
			throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, CommandNewTopoOnGeometry sans géométrie alors que le type de topologie en nécessite une", TkUtil::Charset::UTF_8));


		if (getGeomEntity()->getGeomProperty()->getType() == Geom::GeomProperty::SPHERE_PART){
			createSpherePartBlock();
		}
		else if (getGeomEntity()->getGeomProperty()->getType() == Geom::GeomProperty::HOLLOW_SPHERE_PART){
			createHollowSpherePartBlock();
		}
		else {

			if (getGeomEntity()->getDim() == 0){
				createVertex();
			}
			else if (getGeomEntity()->getDim() == 1){
				createCoEdge();
			}
			else if (getGeomEntity()->getDim() == 3){
				// Utilise un service de création d'une topologie à partir d'un volume
				Internal::ServiceGeomToTopo g2t(getContext(), getGeomEntity(), m_extrem_vertices, &getInfoCommand());

				// convertion en un bloc structuré si cela est demandé
				if (isStructured() && g2t.convertBlockStructured(m_ni, m_nj, m_nk)){
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
					message <<"Convertion en un bloc structuré impossible pour le volume "<<getGeomEntity()->getName();
					throw TkUtil::Exception(message);
				}

				if (m_insertionBlock)
					g2t.convertInsertionBlock();

				addCreatedBlock(g2t.getBlock());
			}
			else if (getGeomEntity()->getDim() == 2){
				// Utilise un service de création d'une topologie à partir d'une surface
				Internal::ServiceGeomToTopo g2t(getContext(), getGeomEntity(), m_extrem_vertices, &getInfoCommand());

				// convertion en une face commune structurés si cela est possible et demandé
				if (isStructured() && g2t.convertCoFaceStructured()){
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
					message <<"Convertion en une face structurées impossible pour la surface "<<getGeomEntity()->getName();
					throw TkUtil::Exception(message);
				}

				addCreatedCoFace(g2t.getCoFace());
			}
			else {
				throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOnGeometry imposible avec une géométrie autre qu'un Volume ou une Surface", TkUtil::Charset::UTF_8));
			}

		} // end else ... GeomProperty

		// suppression des parties internes de type save... qui n'ont pas lieu d'être
	    // puisque l'on est en cours de création
	    saveInternalsStats();
	    deleteInternalsStats();
	} // end else / if (m_freeTopo)

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOnGeometry::validGeomEntity()
{
	CommandCreateTopo::validGeomEntity();

    if (getGeomEntity()){
        // s'il y a une entité il faut que ce soit un Volume, une Surface, une Curve ou un Vertex
    	if (getGeomEntity()->getType() != Utils::Entity::GeomVertex
    			&& getGeomEntity()->getType() != Utils::Entity::GeomCurve
				&& getGeomEntity()->getType() != Utils::Entity::GeomSurface
				&& getGeomEntity()->getType() != Utils::Entity::GeomVolume)
    		throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOnGeometry imposible avec une géométrie autre qu'un volume, une surface, une courbe ou un sommet", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOnGeometry::createVertex()
{
	if (getGeomEntity() == 0 || getGeomEntity()->getType() != Utils::Entity::GeomVertex)
		throw TkUtil::Exception (TkUtil::UTF8String ("createVertex imposible sans une géométrie (sommet)", TkUtil::Charset::UTF_8));

	Geom::Vertex* gvtx = dynamic_cast<Geom::Vertex*>(getGeomEntity());
	if (gvtx == 0)
		throw TkUtil::Exception (TkUtil::UTF8String ("createVertex imposible pour autre chose qu'un sommet géométrique", TkUtil::Charset::UTF_8));

	Vertex* vtx = new Vertex(getContext(), gvtx->getCoord());
	vtx->setGeomAssociation(getGeomEntity());
	getInfoCommand().addTopoInfoEntity(vtx, Internal::InfoCommand::CREATED);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOnGeometry::createCoEdge()
{
	if (getGeomEntity() == 0 || getGeomEntity()->getType() != Utils::Entity::GeomCurve)
		throw TkUtil::Exception (TkUtil::UTF8String ("createCoEdge imposible sans une géométrie (courbe)", TkUtil::Charset::UTF_8));

	Geom::Curve* crv = dynamic_cast<Geom::Curve*>(getGeomEntity());
	if (crv == 0)
		throw TkUtil::Exception (TkUtil::UTF8String ("createCoEdge imposible pour autre chose qu'une courbe géométrique", TkUtil::Charset::UTF_8));

	// récupéaration des sommets
	std::vector<Geom::Vertex*> gvertices;
	crv->get(gvertices);
	std::vector<Topo::Vertex*> tvertices;
	for (uint i=0; i<gvertices.size(); i++){
		const std::vector<Topo::TopoEntity* > ref_topo = gvertices[i]->getRefTopo();
		if (ref_topo.size() == 1){
			Topo::Vertex* vtx = dynamic_cast<Topo::Vertex*>(ref_topo[0]);
			if (vtx == 0)
				throw TkUtil::Exception (TkUtil::UTF8String ("createCoEdge imposible, un des sommets géométriques référence autre chose qu'un sommet topologique", TkUtil::Charset::UTF_8));
			tvertices.push_back(vtx);
		}
		else if (ref_topo.empty())
			throw TkUtil::Exception (TkUtil::UTF8String ("createCoEdge imposible, un des sommets ne référence aucune entité", TkUtil::Charset::UTF_8));
		else
			throw TkUtil::Exception (TkUtil::UTF8String ("createCoEdge imposible, un des sommets références plusieurs entités", TkUtil::Charset::UTF_8));
	}

	if (tvertices.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("createCoEdge imposible, les sommets de la courbe ne référencent aucun sommet topologique", TkUtil::Charset::UTF_8));
	else if (tvertices.size()>2)
		throw TkUtil::Exception (TkUtil::UTF8String ("createCoEdge imposible, les sommets de la courbe référencent plus de 2 sommets topologiques", TkUtil::Charset::UTF_8));

	EdgeMeshingPropertyUniform emp(getContext().getLocalTopoManager().getDefaultNbMeshingEdges());
	CoEdge* coedge = new CoEdge(getContext(), &emp, tvertices[0], tvertices[tvertices.size() == 1?0:1]);
	coedge->setGeomAssociation(getGeomEntity());
	getInfoCommand().addTopoInfoEntity(coedge, Internal::InfoCommand::CREATED);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOnGeometry::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationNewCoedges(dr);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOnGeometry::createSpherePartBlock()
{
	// cas avec dégénérescence à l'origine
	Topo::Block* bloc = new Topo::Block(getContext(), m_nj, m_nk, m_ni,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

	addCreatedBlock(bloc);
	split();

	std::vector<Geom::Vertex*> vertices;
	getGeomEntity()->get(vertices);

	for (uint i=0; i<5; i++)
		bloc->getVertex(i)->setCoord(vertices[i]->getCoord());
	for (uint i=5; i<8; i++)
		bloc->getVertex(i)->setCoord(vertices[4]->getCoord());

	bloc->degenerateFaceInVertex(Block::k_max, &getInfoCommand());

	// les associations
	bloc->setGeomAssociation(getGeomEntity());

	std::vector<Geom::Curve*> curves;
	getGeomEntity()->get(curves);
	bloc->getFace(Block::k_min)->getEdge(bloc->getVertex(0), bloc->getVertex(1))->setGeomAssociation(curves[0]);
	bloc->getFace(Block::k_min)->getEdge(bloc->getVertex(0), bloc->getVertex(2))->setGeomAssociation(curves[1]);
	bloc->getFace(Block::k_min)->getEdge(bloc->getVertex(3), bloc->getVertex(2))->setGeomAssociation(curves[2]);
	bloc->getFace(Block::k_min)->getEdge(bloc->getVertex(3), bloc->getVertex(1))->setGeomAssociation(curves[3]);
	bloc->getFace(Block::i_min)->getEdge(bloc->getVertex(0), bloc->getVertex(4))->setGeomAssociation(curves[4]);
	bloc->getFace(Block::i_min)->getEdge(bloc->getVertex(2), bloc->getVertex(4))->setGeomAssociation(curves[6]);
	bloc->getFace(Block::i_max)->getEdge(bloc->getVertex(1), bloc->getVertex(4))->setGeomAssociation(curves[5]);
	bloc->getFace(Block::i_max)->getEdge(bloc->getVertex(3), bloc->getVertex(4))->setGeomAssociation(curves[7]);

	std::vector<Geom::Surface*> surfaces;
	getGeomEntity()->get(surfaces);
	bloc->getFace(Block::k_min)->setGeomAssociation(surfaces[0]);
	bloc->getFace(Block::i_min)->setGeomAssociation(surfaces[2]);
	bloc->getFace(Block::i_max)->setGeomAssociation(surfaces[4]);
	bloc->getFace(Block::j_min)->setGeomAssociation(surfaces[1]);
	bloc->getFace(Block::j_max)->setGeomAssociation(surfaces[3]);

	for (uint i=0; i<5; i++)
		bloc->getVertex(i)->setGeomAssociation(vertices[i]);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOnGeometry::createHollowSpherePartBlock()
{
	// cas d'un bloc régulier
	Topo::Block* bloc = new Topo::Block(getContext(), m_nj, m_nk, m_ni,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

	addCreatedBlock(bloc);
	split();

	std::vector<Geom::Vertex*> vertices;
	getGeomEntity()->get(vertices);

	for (uint i=0; i<8; i++)
		bloc->getVertex(i)->setCoord(vertices[i]->getCoord());

	// les associations
	bloc->setGeomAssociation(getGeomEntity());

	std::vector<Geom::Curve*> curves;
	getGeomEntity()->get(curves);
	bloc->getFace(Block::k_min)->getEdge(bloc->getVertex(0), bloc->getVertex(1))->setGeomAssociation(curves[0]);
	bloc->getFace(Block::k_min)->getEdge(bloc->getVertex(0), bloc->getVertex(2))->setGeomAssociation(curves[1]);
	bloc->getFace(Block::k_min)->getEdge(bloc->getVertex(3), bloc->getVertex(2))->setGeomAssociation(curves[2]);
	bloc->getFace(Block::k_min)->getEdge(bloc->getVertex(3), bloc->getVertex(1))->setGeomAssociation(curves[3]);
	bloc->getFace(Block::i_min)->getEdge(bloc->getVertex(0), bloc->getVertex(4))->setGeomAssociation(curves[4]);
	bloc->getFace(Block::i_max)->getEdge(bloc->getVertex(5), bloc->getVertex(1))->setGeomAssociation(curves[6]);
	bloc->getFace(Block::i_max)->getEdge(bloc->getVertex(3), bloc->getVertex(7))->setGeomAssociation(curves[10]);
	bloc->getFace(Block::i_min)->getEdge(bloc->getVertex(2), bloc->getVertex(6))->setGeomAssociation(curves[8]);
	bloc->getFace(Block::k_max)->getEdge(bloc->getVertex(4), bloc->getVertex(5))->setGeomAssociation(curves[5]);
	bloc->getFace(Block::k_max)->getEdge(bloc->getVertex(6), bloc->getVertex(7))->setGeomAssociation(curves[9]);
	bloc->getFace(Block::k_max)->getEdge(bloc->getVertex(4), bloc->getVertex(6))->setGeomAssociation(curves[7]);
	bloc->getFace(Block::k_max)->getEdge(bloc->getVertex(5), bloc->getVertex(7))->setGeomAssociation(curves[11]);

	std::vector<Geom::Surface*> surfaces;
	getGeomEntity()->get(surfaces);
	bloc->getFace(Block::k_min)->setGeomAssociation(surfaces[0]);
	bloc->getFace(Block::i_min)->setGeomAssociation(surfaces[2]);
	bloc->getFace(Block::i_max)->setGeomAssociation(surfaces[4]);
	bloc->getFace(Block::j_min)->setGeomAssociation(surfaces[1]);
	bloc->getFace(Block::j_max)->setGeomAssociation(surfaces[3]);
	bloc->getFace(Block::k_max)->setGeomAssociation(surfaces[5]);

	for (uint i=0; i<8; i++)
		bloc->getVertex(i)->setGeomAssociation(vertices[i]);

}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
