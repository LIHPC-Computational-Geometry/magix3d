/*----------------------------------------------------------------------------*/
/** \file CommandNewTopoOGridOnGeometry.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/1/2012
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandNewTopoOGridOnGeometry.h"
#include "Topo/TopoManager.h"
#include "Topo/Block.h"
#include "Topo/TopoHelper.h"
#include "Utils/Common.h"
#include "Utils/Point.h"
#include "Utils/Vector.h"
#include "Utils/Spherical.h"
#include "Utils/Matrix33.h"
#include "Internal/Context.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/PropertyCylinder.h"
#include "Geom/PropertyCone.h"
#include "Geom/PropertyHollowCylinder.h"
#include "Geom/PropertySphere.h"
#include "Geom/PropertyHollowSphere.h"
#include "Geom/GeomEntity.h"
#include "Geom/GeomHelper.h"
#include "Internal/NameManager.h"
#include "Internal/Context.h"
#include "Internal/EntitiesHelper.h"
#include "Utils/TypeDedicatedNameManager.h"

/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>

//#define _DEBUG2
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandNewTopoOGridOnGeometry::
CommandNewTopoOGridOnGeometry(Internal::Context& c, Geom::GeomEntity* entity, const double& rat)
:CommandCreateTopo(c,
		std::string("Création d'une topologique en o-grid sur la géométrie ")
         + (entity?entity->getName():"vide"))
, m_ratio(rat)
, m_naxe(0)
, m_ni(0)
, m_nr(0)
{
    setGeomEntity(entity);
    setStructured(true);

    if (m_ratio<0.0 || m_ratio>1.0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le ratio pour positionner l'o-grid doit être entre 0 et 1 (compris)", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandNewTopoOGridOnGeometry::
CommandNewTopoOGridOnGeometry(Internal::Context& c, Geom::CommandCreateGeom* command_geom, const double& rat)
:CommandCreateTopo(c, "Création d'une topologique en o-grid sur une géométrie depuis une commande")
, m_ratio(rat)
, m_naxe(0)
, m_ni(0)
, m_nr(0)
{
    setCommandCreateGeom(command_geom);
    setStructured(true);

    if (m_ratio<0.0 || m_ratio>1.0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le ratio pour positionner l'o-grid doit être entre 0 et 1 (compris)", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandNewTopoOGridOnGeometry::
~CommandNewTopoOGridOnGeometry()
{}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	message << "CommandNewTopoOGridOnGeometry::execute pour la commande " << getName ( )
		    << " de nom unique " << getUniqueName ( );

	// récupération si nécessaire et validation du type de géométrie
	validGeomEntity();

    // rien à attendre de la PropertyBox pour positionner le bloc topologique,
	// on se base sur les sommets du volume si ce dernier est donné
	if (getGeomEntity()) {

		// recherche si c'est bien un cylindre
	    if (getGeomEntity()){
	          Geom::GeomProperty* gp = getGeomEntity()->getGeomProperty();
	          switch (gp->getType()) {
	          case Geom::GeomProperty::CYLINDER:
	              if (m_ratio==1.0)
	                  createCylinderTopo1Block();
	              else
	                  createCylinderTopoOGrid();
	              break;
	          case Geom::GeomProperty::CONE:
	              if (m_ratio==1.0)
	                  createConeTopo1Block();
	              else
	                  createConeTopoOGrid();
	              break;
              case Geom::GeomProperty::SPHERE:
                  if (m_ratio==1.0)
                      createSphereTopo1Block();
                  else
                      createSphereTopoOGrid();
                  break;
              case Geom::GeomProperty::HOLLOW_CYLINDER:
            	  createHollowCylinderTopo();
            	  break;
              case Geom::GeomProperty::HOLLOW_SPHERE:
            	  createHollowSphereTopo();
            	  break;
	          default:
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
	              message <<"CommandNewTopoOGridOnGeometry impossible, entité "<<getGeomEntity()->getName()
	                      <<" n'est pas d'un type supporté pour le moment";
	              throw TkUtil::Exception(message);
	          }

	          // suppression des entités temporaires (faces, arêtes et sommets communs à 2 blocs)
	          cleanTemporaryEntities();

	          // enregistrement des nouvelles entités dans le TopoManager
	          registerToManagerCreatedEntities();

	          // suppression des parties internes de type save... qui n'ont pas lieu d'être
	          // puisque l'on est en cours de création
	          saveInternalsStats();
	          deleteInternalsStats();

	    }
	}
	else
		throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOGridOnGeometry ne peut se faire sans géométrie", TkUtil::Charset::UTF_8));

//	std::cout<<"Dans CommandNewTopoOGridOnGeometry: "<<std::endl;
//	std::cout<< getInfoCommand() <<std::endl;

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::validGeomEntity()
{
	CommandCreateTopo::validGeomEntity();

    if (getGeomEntity()){
        // s'il y a une entité il faut que ce soit un Volume, une Surface
    	if (getGeomEntity()->getType() != Utils::Entity::GeomSurface
				&& getGeomEntity()->getType() != Utils::Entity::GeomVolume)
    		throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOGridOnGeometry imposible avec une géométrie autre qu'un volume ou une surface", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createCylinderTopo1Block()
{
    Geom::PropertyCylinder* propertyCyl = getGeomEntity()->getGeomProperty()->toPropertyCylinder();
#ifdef _DEBUG2
    std::cout<<"createCylinderTopo1Block avec angle de "<<propertyCyl->getAngle()<<std::endl;
#endif

    double angle = propertyCyl->getAngle();

    if (angle<=135.0){
        createCylinderTopo1BlockQuart(propertyCyl);
    }
    else if (angle>135.0 && angle < 360.0){
        createCylinderTopo1BlockDemi(propertyCyl);
    }
    else if (angle == 360.0){
        createCylinderTopo1BlockPlein(propertyCyl);
    }
    else {
        throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOGridOnGeometry ne peut se faire pour cet angle de cylindre", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createCylinderTopo1BlockPlein(Geom::PropertyCylinder* propertyCyl)
{
    Utils::Math::Point centre = propertyCyl->getCenter();
    double rayon = 0.0;
    double d2 = std::sqrt(2.0)/2.0;
    Utils::Math::Point p1, p2, p3, p4;

    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // calcul de la matrice de transformation du repère actuel
    // avec une topologie d'axe Z vers un repère d'axe celui du cylindre
    Utils::Math::Vector cylAxe = propertyCyl->getAxis();
    cylAxe /= cylAxe.norme(); // on normalise le vecteur des fois que cela ne soit pas fait

    // calcul les angles de ce vecteur dans un repère sphérique
    Utils::Math::Spherical sphericalCoordAxe(cylAxe);

    // angle entre axe et celui des Z
    Utils::Math::Matrix33  rotation = Utils::Math::Matrix33(sphericalCoordAxe.getTheta()*M_PI/180, 1);
    rotation = Utils::Math::Matrix33(sphericalCoordAxe.getPhi()*M_PI/180, 2) * rotation;

    for (uint niv=0; niv<2; niv++){
        rayon = propertyCyl->getRadius()*d2;

        Utils::Math::Vector decentrement(centre);
        if (niv)
            decentrement += cylAxe*propertyCyl->getHeight();

        // calcul les points dans le repère d'axe Z
        p1 = Utils::Math::Point(-rayon, -rayon, 0);
        p2 = Utils::Math::Point(-rayon, +rayon, 0);
        p3 = Utils::Math::Point(+rayon, +rayon, 0);
        p4 = Utils::Math::Point(+rayon, -rayon, 0);

        // changement de repère (Z => cylAxe) avec changement de centre
        p1 = rotation*p1 + Utils::Math::Point(decentrement);
        p2 = rotation*p2 + Utils::Math::Point(decentrement);
        p3 = rotation*p3 + Utils::Math::Point(decentrement);
        p4 = rotation*p4 + Utils::Math::Point(decentrement);

        b1->getVertex(false,false,niv)->setCoord(p1);
        b1->getVertex(false,true,niv)->setCoord(p2);
        b1->getVertex(true,true,niv)->setCoord(p3);
        b1->getVertex(true,false,niv)->setCoord(p4);

    } // end for (uint niv=0; niv<2; niv++)

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 surfaces géométriques mais "
                <<surfaces.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // on considère que la première surface est celle qui enveloppe
    b1->getFace(Block::i_min)->setGeomAssociation(surfaces[0]);
    b1->getFace(Block::i_max)->setGeomAssociation(surfaces[0]);
    b1->getFace(Block::j_min)->setGeomAssociation(surfaces[0]);
    b1->getFace(Block::j_max)->setGeomAssociation(surfaces[0]);

    b1->getFace(Block::i_min)->getEdge(Face::j_min)->setGeomAssociation(surfaces[0]);
    b1->getFace(Block::i_min)->getEdge(Face::j_max)->setGeomAssociation(surfaces[0]);
    b1->getFace(Block::i_max)->getEdge(Face::j_min)->setGeomAssociation(surfaces[0]);
    b1->getFace(Block::i_max)->getEdge(Face::j_max)->setGeomAssociation(surfaces[0]);

    // la deuxième est au sommet du cylindre
    b1->getFace(Block::k_max)->setGeomAssociation(surfaces[1]);

    // la dernière surface est à la base du cylindre
    b1->getFace(Block::k_min)->setGeomAssociation(surfaces[2]);

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 courbes géométriques mais "
                <<curves.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // on considère que la 3ème courbe est celle qui est à la base du cylindre
    b1->getFace(Block::k_min)->getEdge(Face::i_min)->setGeomAssociation(curves[2]);
    b1->getFace(Block::k_min)->getEdge(Face::i_max)->setGeomAssociation(curves[2]);
    b1->getFace(Block::k_min)->getEdge(Face::j_min)->setGeomAssociation(curves[2]);
    b1->getFace(Block::k_min)->getEdge(Face::j_max)->setGeomAssociation(curves[2]);

    b1->getFace(Block::k_min)->getVertex(0)->setGeomAssociation(curves[2]);
    b1->getFace(Block::k_min)->getVertex(1)->setGeomAssociation(curves[2]);
    b1->getFace(Block::k_min)->getVertex(2)->setGeomAssociation(curves[2]);
    b1->getFace(Block::k_min)->getVertex(3)->setGeomAssociation(curves[2]);

    // on considère que la première courbe est celle qui est en z max
    b1->getFace(Block::k_max)->getEdge(Face::i_min)->setGeomAssociation(curves[0]);
    b1->getFace(Block::k_max)->getEdge(Face::i_max)->setGeomAssociation(curves[0]);
    b1->getFace(Block::k_max)->getEdge(Face::j_min)->setGeomAssociation(curves[0]);
    b1->getFace(Block::k_max)->getEdge(Face::j_max)->setGeomAssociation(curves[0]);

    b1->getFace(Block::k_max)->getVertex(0)->setGeomAssociation(curves[0]);
    b1->getFace(Block::k_max)->getVertex(1)->setGeomAssociation(curves[0]);
    b1->getFace(Block::k_max)->getVertex(2)->setGeomAssociation(curves[0]);
    b1->getFace(Block::k_max)->getVertex(3)->setGeomAssociation(curves[0]);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createCylinderTopo1BlockDemi(Geom::PropertyCylinder* propertyCyl)
{
    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, m_nr, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 5){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 5 surfaces géométriques mais "
                <<surfaces.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 9){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 9 courbes géométriques mais "
                <<curves.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // on utilise directement les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 sommets géométriques mais "
                <<vertices.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    Utils::Math::Point p0, p1, p2, p3;

    // p0 est au premier quart de la courbe c0 (entre s0 et s1)
    // p1 au 3 ème quart
    double param1, param2;
    curves[0]->getParameter(vertices[0]->getCoord(), param1);
    curves[0]->getParameter(vertices[1]->getCoord(), param2);
    curves[0]->getPoint(param1+(param2-param1)/4, p0);
    curves[0]->getPoint(param1+(param2-param1)*3/4, p1);

    curves[2]->getParameter(vertices[2]->getCoord(), param1);
    curves[2]->getParameter(vertices[3]->getCoord(), param2);
    curves[2]->getPoint(param1+(param2-param1)/4, p2);
    curves[2]->getPoint(param1+(param2-param1)*3/4, p3);

    b1->getVertex(0)->setCoord(vertices[1]->getCoord());
    b1->getVertex(1)->setCoord(vertices[0]->getCoord());
    b1->getVertex(2)->setCoord(p1);
    b1->getVertex(3)->setCoord(p0);

    b1->getVertex(4)->setCoord(vertices[3]->getCoord());
    b1->getVertex(5)->setCoord(vertices[2]->getCoord());
    b1->getVertex(6)->setCoord(p3);
    b1->getVertex(7)->setCoord(p2);

    // on coupe la face du bloc b1 pour qu'une arête soit sur la courbe 8
    std::vector<Edge* > splitingEdges;
    std::vector<CoFace* > cofaces;
    cofaces.push_back(b1->getFace(Block::j_min)->getCoFace(0));
    TopoHelper::splitFaces(cofaces,
            b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->getCoEdge(0),
            0.5, 0.0, false, false,
            splitingEdges,
            &getInfoCommand());

    Vertex* v0 = TopoHelper::getCommonVertex(splitingEdges[0]->getCoEdge(0),
            b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->getCoEdge(0));
    Vertex* v1 = TopoHelper::getCommonVertex(splitingEdges[0]->getCoEdge(0),
            b1->getFace(Block::k_max)->getEdge(b1->getVertex(4), b1->getVertex(5))->getCoEdge(0));

    v0->setCoord(vertices[4]->getCoord());
    v1->setCoord(vertices[5]->getCoord());

    splitingEdges[0]->getCoEdge(0)->setGeomAssociation(curves[8]);

    // on considère que la première surface est celle qui enveloppe
    b1->getFace(Block::i_max)->setGeomAssociation(surfaces[0]);
    b1->getFace(Block::j_max)->setGeomAssociation(surfaces[0]);
    b1->getFace(Block::i_min)->setGeomAssociation(surfaces[0]);


    // la deuxième est au sommet du cylindre
    b1->getFace(Block::k_min)->setGeomAssociation(surfaces[1]);

    // la troisième est à la base du cylindre
    b1->getFace(Block::k_max)->setGeomAssociation(surfaces[2]);

    CoFace* coface0 = b1->getFace(Block::j_min)->getCoFace(0);
    if (coface0->find(b1->getVertex(1))){
        b1->getFace(Block::j_min)->getCoFace(0)->setGeomAssociation(surfaces[4]);
        b1->getFace(Block::j_min)->getCoFace(1)->setGeomAssociation(surfaces[3]);
    } else {
        b1->getFace(Block::j_min)->getCoFace(1)->setGeomAssociation(surfaces[4]);
        b1->getFace(Block::j_min)->getCoFace(0)->setGeomAssociation(surfaces[3]);
    }

    // on considère que la courbe 0 est celle qui est à au sommet du cylindre
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(1), b1->getVertex(3))->setGeomAssociation(curves[0]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(2), b1->getVertex(3))->setGeomAssociation(curves[0]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(2), b1->getVertex(0))->setGeomAssociation(curves[0]);
    b1->getVertex(3)->setGeomAssociation(curves[0]);
    b1->getVertex(2)->setGeomAssociation(curves[0]);

    // on considère que la courbe 2 est celle qui est à la base du cylindre
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(5), b1->getVertex(7))->setGeomAssociation(curves[2]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(6), b1->getVertex(7))->setGeomAssociation(curves[2]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(6), b1->getVertex(4))->setGeomAssociation(curves[2]);
    b1->getVertex(7)->setGeomAssociation(curves[2]);
    b1->getVertex(6)->setGeomAssociation(curves[2]);

    b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->getCoEdge(v0, b1->getVertex(1))->setGeomAssociation(curves[5]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->getCoEdge(b1->getVertex(0), v0)->setGeomAssociation(curves[4]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(4), b1->getVertex(5))->getCoEdge(v1, b1->getVertex(5))->setGeomAssociation(curves[6]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(4), b1->getVertex(5))->getCoEdge(b1->getVertex(4), v1)->setGeomAssociation(curves[7]);

    b1->getFace(Block::i_max)->getEdge(b1->getVertex(1), b1->getVertex(5))->setGeomAssociation(curves[1]);
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(0), b1->getVertex(4))->setGeomAssociation(curves[3]);
    b1->getFace(Block::j_max)->getEdge(b1->getVertex(3), b1->getVertex(7))->setGeomAssociation(surfaces[0]);
    b1->getFace(Block::j_max)->getEdge(b1->getVertex(2), b1->getVertex(6))->setGeomAssociation(surfaces[0]);

    b1->getVertex(0)->setGeomAssociation(vertices[1]);
    b1->getVertex(1)->setGeomAssociation(vertices[0]);
    b1->getVertex(4)->setGeomAssociation(vertices[3]);
    b1->getVertex(5)->setGeomAssociation(vertices[2]);
    v0->setGeomAssociation(vertices[4]);
    v1->setGeomAssociation(vertices[5]);

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createCylinderTopo1BlockQuart(Geom::PropertyCylinder* propertyCyl)
{
    Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 5){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 5 surfaces géométriques mais "
                <<surfaces.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 9){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 9 courbes géométriques mais "
                <<curves.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // on utilise directement les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 sommets géométriques mais "
                <<vertices.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    Utils::Math::Point p0, p2;

    // p0 est au milieu de la courbe c0 (entre s0 et s1)
    double param1, param2;
    curves[0]->getParameter(vertices[0]->getCoord(), param1);
    curves[0]->getParameter(vertices[1]->getCoord(), param2);
    curves[0]->getPoint((param1+param2)/2, p0);

    curves[2]->getParameter(vertices[2]->getCoord(), param1);
    curves[2]->getParameter(vertices[3]->getCoord(), param2);
    curves[2]->getPoint((param1+param2)/2, p2);


    b1->getVertex(0)->setCoord(vertices[4]->getCoord());
    b1->getVertex(1)->setCoord(vertices[0]->getCoord());
    b1->getVertex(2)->setCoord(vertices[1]->getCoord());
    b1->getVertex(3)->setCoord(p0);

    b1->getVertex(4)->setCoord(vertices[5]->getCoord());
    b1->getVertex(5)->setCoord(vertices[2]->getCoord());
    b1->getVertex(6)->setCoord(vertices[3]->getCoord());
    b1->getVertex(7)->setCoord(p2);

    // on considère que la première surface est celle qui enveloppe
    b1->getFace(Block::i_max)->setGeomAssociation(surfaces[0]);
    b1->getFace(Block::j_max)->setGeomAssociation(surfaces[0]);

    // la deuxième est au sommet du cylindre
    b1->getFace(Block::k_min)->setGeomAssociation(surfaces[1]);

    // la troisième est à la base du cylindre
    b1->getFace(Block::k_max)->setGeomAssociation(surfaces[2]);

    b1->getFace(Block::j_min)->getCoFace(0)->setGeomAssociation(surfaces[4]);
    b1->getFace(Block::i_min)->getCoFace(0)->setGeomAssociation(surfaces[3]);


    // on considère que la courbe 0 est celle qui est à au sommet du cylindre
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(1), b1->getVertex(3))->setGeomAssociation(curves[0]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(2), b1->getVertex(3))->setGeomAssociation(curves[0]);
    b1->getVertex(3)->setGeomAssociation(curves[0]);

    // on considère que la courbe 2 est celle qui est à la base du cylindre
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(5), b1->getVertex(7))->setGeomAssociation(curves[2]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(6), b1->getVertex(7))->setGeomAssociation(curves[2]);
    b1->getVertex(7)->setGeomAssociation(curves[2]);

    b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->setGeomAssociation(curves[5]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(2))->setGeomAssociation(curves[4]);

    b1->getFace(Block::k_max)->getEdge(b1->getVertex(4), b1->getVertex(5))->setGeomAssociation(curves[6]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(4), b1->getVertex(6))->setGeomAssociation(curves[7]);

    b1->getFace(Block::i_max)->getEdge(b1->getVertex(1), b1->getVertex(5))->setGeomAssociation(curves[1]);
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(0), b1->getVertex(4))->setGeomAssociation(curves[8]);
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(2), b1->getVertex(6))->setGeomAssociation(curves[3]);
    b1->getFace(Block::i_max)->getEdge(b1->getVertex(3), b1->getVertex(7))->setGeomAssociation(surfaces[0]);

    b1->getVertex(0)->setGeomAssociation(vertices[4]);
    b1->getVertex(1)->setGeomAssociation(vertices[0]);
    b1->getVertex(2)->setGeomAssociation(vertices[1]);
    b1->getVertex(4)->setGeomAssociation(vertices[5]);
    b1->getVertex(5)->setGeomAssociation(vertices[2]);
    b1->getVertex(6)->setGeomAssociation(vertices[3]);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createCylinderTopoOGrid()
{
    Geom::PropertyCylinder* propertyCyl = getGeomEntity()->getGeomProperty()->toPropertyCylinder();
#ifdef _DEBUG2
    std::cout<<"createCylinderTopoOGrid avec angle de "<<propertyCyl->getAngle()<<std::endl;
#endif

    double angle = propertyCyl->getAngle();

    if (angle<=135.0){
        // création d'une topologie à 1 ou 3 blocs suivant si m_ratio == 0 ou non
        if (m_ratio > 0.0)
            createCylinderTopoOGridQuartNonDeg(propertyCyl);
        else
            createCylinderTopoOGridQuartDeg(propertyCyl);
    }
    else if (angle>135.0 && angle < 360.0){
        // création d'une topologie à 1 ou 4 blocs suivant si m_ratio == 0 ou non
        if (m_ratio > 0.0)
            createCylinderTopoOGridDemiNonDeg(propertyCyl);
        else
            createCylinderTopoOGridQuartDeg(propertyCyl);
    }
    else if (angle == 360.0){
        // création d'une topologie à 4 ou 5 blocs suivant si m_ratio == 0 ou non
        if (m_ratio > 0.0)
            createCylinderTopoOGridPleinNonDeg(propertyCyl);
        else
            createCylinderTopoOGridPleinDeg(propertyCyl);
    }
    else {
        throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOGridOnGeometry ne peut se faire pour cet angle de cylindre", TkUtil::Charset::UTF_8));
    }


#ifdef _DEBUG2
    // LOG de la Topo résultante
	TkUtil::UTF8String	message_result (TkUtil::Charset::UTF_8);
    std::vector<Block* > blocks;
    getBlocks(blocks);
    message_result << "CommandNewTopoOGridOnGeometry donne comme topologie: \n";
    for (std::vector<Block* >::iterator iter = blocks.begin();
            iter != blocks.end(); ++iter)
        message_result << **iter;
    log (TkUtil::TraceLog (message_result, TkUtil::Log::TRACE_4));
#endif
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createCylinderTopoOGridPleinNonDeg(Geom::PropertyCylinder* propertyCyl)
{
#ifdef _DEBUG2
    std::cout<<"createCylinderTopoOGridPleinNonDeg(...)"<<std::endl;
#endif
    Utils::Math::Point centre = propertyCyl->getCenter();
    double rayon = 0.0;
    double d2 = std::sqrt(2.0)/2.0;
    Utils::Math::Point p1, p2, p3, p4;

    Topo::Block* b1 = new Topo::Block(getContext(), m_nr, 2*m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_i);
    Topo::Block* b2 = new Topo::Block(getContext(), 2*m_ni, m_nr, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_j);
    Topo::Block* b3 = new Topo::Block(getContext(), m_nr, 2*m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_i);
    Topo::Block* b4 = new Topo::Block(getContext(), 2*m_ni, m_nr, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_j);
    Topo::Block* b5 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_i);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b3);
    addCreatedBlock(b4);
    addCreatedBlock(b5);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b3->setGeomAssociation(getGeomEntity());
    b4->setGeomAssociation(getGeomEntity());
    b5->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // calcul de la matrice de transformation du repère actuel
    // avec une topologie d'axe Z vers un repère d'axe celui du cylindre
    Utils::Math::Vector cylAxe = propertyCyl->getAxis();
    cylAxe /= cylAxe.norme(); // on normalise le vecteur des fois que cela ne soit pas fait

    // calcul les angles de ce vecteur dans un repère sphérique
    Utils::Math::Spherical sphericalCoordAxe(cylAxe);
    // angle entre axe et celui des Z
    Utils::Math::Matrix33  rotation = Utils::Math::Matrix33(sphericalCoordAxe.getTheta()*M_PI/180, 1);
    rotation = Utils::Math::Matrix33(sphericalCoordAxe.getPhi()*M_PI/180, 2) * rotation;

#ifdef _DEBUG2
    std::cout<<"cylAxe : "<<cylAxe<<std::endl;
    std::cout<<"Theta : "<<sphericalCoordAxe.getTheta()<<std::endl;
    std::cout<<"Phi : "<<sphericalCoordAxe.getPhi()<<std::endl;
#endif

    for (uint niv=0; niv<2; niv++){
        rayon = propertyCyl->getRadius()*d2;

        Utils::Math::Vector decentrement(centre);
        if (niv)
            decentrement += cylAxe*propertyCyl->getHeight();

        // calcul les points dans le repère d'axe Z
        p1 = Utils::Math::Point(-rayon, -rayon, 0);
        p2 = Utils::Math::Point(-rayon, +rayon, 0);
        p3 = Utils::Math::Point(+rayon, +rayon, 0);
        p4 = Utils::Math::Point(+rayon, -rayon, 0);

        // changement de repère (Z => cylAxe) avec changement de centre
        p1 = rotation*p1 + Utils::Math::Point(decentrement);
        p2 = rotation*p2 + Utils::Math::Point(decentrement);
        p3 = rotation*p3 + Utils::Math::Point(decentrement);
        p4 = rotation*p4 + Utils::Math::Point(decentrement);

        // supperpose les points avec la position définitive
        b4->getVertex(false,false,niv)->setCoord(p1);
        b1->getVertex(false,false,niv)->setCoord(p1);

        b1->getVertex(false,true,niv)->setCoord(p2);
        b2->getVertex(false,true,niv)->setCoord(p2);

        b2->getVertex(true,true,niv)->setCoord(p3);
        b3->getVertex(true,true,niv)->setCoord(p3);

        b3->getVertex(true,false,niv)->setCoord(p4);
        b4->getVertex(true,false,niv)->setCoord(p4);

        // on recommence avec les points au sommet de l'o-grid
        rayon = propertyCyl->getRadius()*d2*m_ratio;
        p1 = Utils::Math::Point(-rayon, -rayon, 0);
        p2 = Utils::Math::Point(-rayon, +rayon, 0);
        p3 = Utils::Math::Point(+rayon, +rayon, 0);
        p4 = Utils::Math::Point(+rayon, -rayon, 0);

        // changement de repère (Z => cylAxe) avec changement de centre
        p1 = rotation*p1 + Utils::Math::Point(decentrement);
        p2 = rotation*p2 + Utils::Math::Point(decentrement);
        p3 = rotation*p3 + Utils::Math::Point(decentrement);
        p4 = rotation*p4 + Utils::Math::Point(decentrement);

        b4->getVertex(false,true,niv)->setCoord(p1);
        b1->getVertex(true,false,niv)->setCoord(p1);
        b5->getVertex(false,false,niv)->setCoord(p1);

        b1->getVertex(true,true,niv)->setCoord(p2);
        b2->getVertex(false,false,niv)->setCoord(p2);
        b5->getVertex(false,true,niv)->setCoord(p2);

        b2->getVertex(true,false,niv)->setCoord(p3);
        b3->getVertex(false,true,niv)->setCoord(p3);
        b5->getVertex(true,true,niv)->setCoord(p3);

        b3->getVertex(false,false,niv)->setCoord(p4);
        b4->getVertex(true,true,niv)->setCoord(p4);
        b5->getVertex(true,false,niv)->setCoord(p4);

    } // end for niv

    // fusion des faces avec l'ogrid
    b1->getFace(Block::i_max)->fuse(b5->getFace(Block::i_min), &getInfoCommand());
    b2->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b3->getFace(Block::i_min)->fuse(b5->getFace(Block::i_max), &getInfoCommand());
    b4->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());

    // fusion des faces autour de l'ogrid
    b1->getFace(Block::j_max)->fuse(b2->getFace(Block::i_min), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b3->getFace(Block::j_max), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b4->getFace(Block::i_max), &getInfoCommand());
    b4->getFace(Block::i_min)->fuse(b1->getFace(Block::j_min), &getInfoCommand());

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 surfaces géométriques mais "
                <<surfaces.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // on considère que la première surface est celle qui enveloppe
    b1->getFace(Block::i_min)->setGeomAssociation(surfaces[0]);
    b2->getFace(Block::j_max)->setGeomAssociation(surfaces[0]);
    b3->getFace(Block::i_max)->setGeomAssociation(surfaces[0]);
    b4->getFace(Block::j_min)->setGeomAssociation(surfaces[0]);

    b1->getFace(Block::i_min)->getEdge(Face::j_min)->setGeomAssociation(surfaces[0]);
    b1->getFace(Block::i_min)->getEdge(Face::j_max)->setGeomAssociation(surfaces[0]);
    b3->getFace(Block::i_max)->getEdge(Face::j_min)->setGeomAssociation(surfaces[0]);
    b3->getFace(Block::i_max)->getEdge(Face::j_max)->setGeomAssociation(surfaces[0]);

    // la deuxième est au sommet du cylindre
    b1->getFace(Block::k_max)->setGeomAssociation(surfaces[1]);
    b2->getFace(Block::k_max)->setGeomAssociation(surfaces[1]);
    b3->getFace(Block::k_max)->setGeomAssociation(surfaces[1]);
    b4->getFace(Block::k_max)->setGeomAssociation(surfaces[1]);
    b5->getFace(Block::k_max)->setGeomAssociation(surfaces[1]);

    for (uint i=0; i<4; i++){
        b1->getFace(Block::k_max)->getEdge(i)->setGeomAssociation(surfaces[1]);
        b3->getFace(Block::k_max)->getEdge(i)->setGeomAssociation(surfaces[1]);
        b5->getFace(Block::k_max)->getEdge(i)->setGeomAssociation(surfaces[1]);
        b5->getFace(Block::k_max)->getVertex(i)->setGeomAssociation(surfaces[1]);
    }

    // la dernière surface est à la base du cylindre
    b1->getFace(Block::k_min)->setGeomAssociation(surfaces[2]);
    b2->getFace(Block::k_min)->setGeomAssociation(surfaces[2]);
    b3->getFace(Block::k_min)->setGeomAssociation(surfaces[2]);
    b4->getFace(Block::k_min)->setGeomAssociation(surfaces[2]);
    b5->getFace(Block::k_min)->setGeomAssociation(surfaces[2]);

    for (uint i=0; i<4; i++){
        b1->getFace(Block::k_min)->getEdge(i)->setGeomAssociation(surfaces[2]);
        b3->getFace(Block::k_min)->getEdge(i)->setGeomAssociation(surfaces[2]);
        b5->getFace(Block::k_min)->getEdge(i)->setGeomAssociation(surfaces[2]);
        b5->getFace(Block::k_min)->getVertex(i)->setGeomAssociation(surfaces[2]);
    }

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 courbes géométriques mais "
                <<curves.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // on considère que la 3ème courbe est celle qui est à la base du cylindre
    b1->getFace(Block::i_min)->getEdge(Face::i_min)->setGeomAssociation(curves[2]);
    b2->getFace(Block::j_max)->getEdge(Face::i_min)->setGeomAssociation(curves[2]);
    b3->getFace(Block::i_max)->getEdge(Face::i_min)->setGeomAssociation(curves[2]);
    b4->getFace(Block::j_min)->getEdge(Face::i_min)->setGeomAssociation(curves[2]);

    b1->getFace(Block::i_min)->getEdge(Face::i_min)->getVertex(0)->setGeomAssociation(curves[2]);
    b1->getFace(Block::i_min)->getEdge(Face::i_min)->getVertex(1)->setGeomAssociation(curves[2]);
    b3->getFace(Block::i_max)->getEdge(Face::i_min)->getVertex(0)->setGeomAssociation(curves[2]);
    b3->getFace(Block::i_max)->getEdge(Face::i_min)->getVertex(1)->setGeomAssociation(curves[2]);

    // on considère que la première courbe est celle qui est en z max
    b1->getFace(Block::i_min)->getEdge(Face::i_max)->setGeomAssociation(curves[0]);
    b2->getFace(Block::j_max)->getEdge(Face::i_max)->setGeomAssociation(curves[0]);
    b3->getFace(Block::i_max)->getEdge(Face::i_max)->setGeomAssociation(curves[0]);
    b4->getFace(Block::j_min)->getEdge(Face::i_max)->setGeomAssociation(curves[0]);

    b1->getFace(Block::i_min)->getEdge(Face::i_max)->getVertex(0)->setGeomAssociation(curves[0]);
    b1->getFace(Block::i_min)->getEdge(Face::i_max)->getVertex(1)->setGeomAssociation(curves[0]);
    b3->getFace(Block::i_max)->getEdge(Face::i_max)->getVertex(0)->setGeomAssociation(curves[0]);
    b3->getFace(Block::i_max)->getEdge(Face::i_max)->getVertex(1)->setGeomAssociation(curves[0]);


} // createCylinderTopoOGridPleinNonDeg
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createCylinderTopoOGridPleinDeg(Geom::PropertyCylinder* propertyCyl)
{
#ifdef _DEBUG2
    std::cout<<"createCylinderTopoOGridPleinDeg(...)"<<std::endl;
#endif
    Utils::Math::Point centre = propertyCyl->getCenter();
    double rayon = 0.0;
    double d2 = std::sqrt(2.0)/2.0;
    Utils::Math::Point p1, p2, p3, p4;

    // cas m_ratio == 0
    // ce qui suit ressemble au cas précédent (createCylinderTopoOGridPleinNonDeg),
    // mais ce dernier ne pouvait s'adapter au cas dégénéré,
    // en effet, la dégénérescence se place obligatoirement en z_max

    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b2 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b3);
    addCreatedBlock(b4);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b3->setGeomAssociation(getGeomEntity());
    b4->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // calcul de la matrice de transformation du repère actuel
    // avec une topologie d'axe Z vers un repère d'axe celui du cylindre
    Utils::Math::Vector cylAxe = propertyCyl->getAxis();
    cylAxe /= cylAxe.norme(); // on normalise le vecteur des fois que cela ne soit pas fait

    // calcul les angles de ce vecteur dans un repère sphérique
    Utils::Math::Spherical sphericalCoordAxe(cylAxe);

    // angle entre axe et celui des Z
    Utils::Math::Matrix33  rotation = Utils::Math::Matrix33(sphericalCoordAxe.getTheta()*M_PI/180, 1);
    rotation = Utils::Math::Matrix33(sphericalCoordAxe.getPhi()*M_PI/180, 2) * rotation;

    for (uint niv=0; niv<2; niv++){
        rayon = propertyCyl->getRadius()*d2;

        Utils::Math::Vector decentrement(centre);
        if (niv)
            decentrement += cylAxe*propertyCyl->getHeight();

        // calcul les points dans le repère d'axe Z
        p1 = Utils::Math::Point(-rayon, -rayon, 0);
        p2 = Utils::Math::Point(-rayon, +rayon, 0);
        p3 = Utils::Math::Point(+rayon, +rayon, 0);
        p4 = Utils::Math::Point(+rayon, -rayon, 0);

        // changement de repère (Z => cylAxe) avec changement de centre
        p1 = rotation*p1 + Utils::Math::Point(decentrement);
        p2 = rotation*p2 + Utils::Math::Point(decentrement);
        p3 = rotation*p3 + Utils::Math::Point(decentrement);
        p4 = rotation*p4 + Utils::Math::Point(decentrement);

        // supperpose les points avec la position définitive
        b4->getVertex(true, niv, false)->setCoord(p1);
        b1->getVertex(false,niv, false)->setCoord(p1);

        b1->getVertex(true, niv, false)->setCoord(p2);
        b2->getVertex(false,niv, false)->setCoord(p2);

        b2->getVertex(true, niv, false)->setCoord(p3);
        b3->getVertex(false,niv, false)->setCoord(p3);

        b3->getVertex(true, niv, false)->setCoord(p4);
        b4->getVertex(false,niv, false)->setCoord(p4);

        // on recommence avec les points au centre de l'o-grid dégénéré (axe du cylindre)
        Utils::Math::Point pc = Utils::Math::Point(0, 0, 0);

        // changement de repère (Z => cylAxe) avec changement de centre
        pc = rotation*pc + Utils::Math::Point(decentrement);

        b1->getVertex(false,niv, true)->setCoord(pc);
        b1->getVertex(true ,niv, true)->setCoord(pc);

        b2->getVertex(false,niv, true)->setCoord(pc);
        b2->getVertex(true ,niv, true)->setCoord(pc);

        b3->getVertex(false,niv, true)->setCoord(pc);
        b3->getVertex(true ,niv, true)->setCoord(pc);

        b4->getVertex(false,niv, true)->setCoord(pc);
        b4->getVertex(true ,niv, true)->setCoord(pc);

    } // end for niv

    // dégénéréscence des blocs
    b1->degenerateFaceInEdge(Block::k_max, b1->getVertex(4), b1->getVertex(5), &getInfoCommand());
    b2->degenerateFaceInEdge(Block::k_max, b2->getVertex(4), b2->getVertex(5), &getInfoCommand());
    b3->degenerateFaceInEdge(Block::k_max, b3->getVertex(4), b3->getVertex(5), &getInfoCommand());
    b4->degenerateFaceInEdge(Block::k_max, b4->getVertex(4), b4->getVertex(5), &getInfoCommand());

    // fusion des faces autour de l'ogrid
    b1->getFace(Block::i_max)->fuse(b2->getFace(Block::i_min), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b3->getFace(Block::i_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b4->getFace(Block::i_min), &getInfoCommand());
    b4->getFace(Block::i_max)->fuse(b1->getFace(Block::i_min), &getInfoCommand());


    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 surfaces géométriques mais "
                <<surfaces.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // on considère que la première surface est celle qui enveloppe
    b1->getFace(Block::k_min)->setGeomAssociation(surfaces[0]);
    b2->getFace(Block::k_min)->setGeomAssociation(surfaces[0]);
    b3->getFace(Block::k_min)->setGeomAssociation(surfaces[0]);
    b4->getFace(Block::k_min)->setGeomAssociation(surfaces[0]);

    for (uint i=0; i<4; i++){
        b1->getFace(Block::k_min)->getEdge(i)->setGeomAssociation(surfaces[0]);
        b3->getFace(Block::k_min)->getEdge(i)->setGeomAssociation(surfaces[0]);
    }

    // la deuxième est au sommet du cylindre
    b1->getFace(Block::j_max)->setGeomAssociation(surfaces[1]);
    b2->getFace(Block::j_max)->setGeomAssociation(surfaces[1]);
    b3->getFace(Block::j_max)->setGeomAssociation(surfaces[1]);
    b4->getFace(Block::j_max)->setGeomAssociation(surfaces[1]);

    for (uint i=0; i<3; i++){
        b1->getFace(Block::j_max)->getEdge(i)->setGeomAssociation(surfaces[1]);
        b3->getFace(Block::j_max)->getEdge(i)->setGeomAssociation(surfaces[1]);
    }
    b1->getVertex(5)->setGeomAssociation(surfaces[1]);

    // la dernière surface est à la base du cylindre
    b1->getFace(Block::j_min)->setGeomAssociation(surfaces[2]);
    b2->getFace(Block::j_min)->setGeomAssociation(surfaces[2]);
    b3->getFace(Block::j_min)->setGeomAssociation(surfaces[2]);
    b4->getFace(Block::j_min)->setGeomAssociation(surfaces[2]);

    for (uint i=0; i<3; i++){
        b1->getFace(Block::j_min)->getEdge(i)->setGeomAssociation(surfaces[2]);
        b3->getFace(Block::j_min)->getEdge(i)->setGeomAssociation(surfaces[2]);
    }
    b1->getVertex(4)->setGeomAssociation(surfaces[2]);

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 courbes géométriques mais "
                <<curves.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // on considère que la première courbe est celle qui est en j max
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(2), b1->getVertex(3))->setGeomAssociation(curves[0]);
    b2->getFace(Block::k_min)->getEdge(b2->getVertex(2), b2->getVertex(3))->setGeomAssociation(curves[0]);
    b3->getFace(Block::k_min)->getEdge(b3->getVertex(2), b3->getVertex(3))->setGeomAssociation(curves[0]);
    b4->getFace(Block::k_min)->getEdge(b4->getVertex(2), b4->getVertex(3))->setGeomAssociation(curves[0]);

    b1->getVertex(2)->setGeomAssociation(curves[0]);
    b1->getVertex(3)->setGeomAssociation(curves[0]);
    b3->getVertex(2)->setGeomAssociation(curves[0]);
    b3->getVertex(3)->setGeomAssociation(curves[0]);

    // on considère que la 3ème courbe est celle qui est à la base du cylindre
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->setGeomAssociation(curves[2]);
    b2->getFace(Block::k_min)->getEdge(b2->getVertex(0), b2->getVertex(1))->setGeomAssociation(curves[2]);
    b3->getFace(Block::k_min)->getEdge(b3->getVertex(0), b3->getVertex(1))->setGeomAssociation(curves[2]);
    b4->getFace(Block::k_min)->getEdge(b4->getVertex(0), b4->getVertex(1))->setGeomAssociation(curves[2]);

    b1->getVertex(0)->setGeomAssociation(curves[2]);
    b1->getVertex(1)->setGeomAssociation(curves[2]);
    b3->getVertex(0)->setGeomAssociation(curves[2]);
    b3->getVertex(1)->setGeomAssociation(curves[2]);

} // createCylinderTopoOGridPleinDeg
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createCylinderTopoOGridQuartDeg(Geom::PropertyCylinder* propertyCyl)
{
#ifdef _DEBUG2
    std::cout<<"createCylinderTopoOGridQuartDeg(...)"<<std::endl;
#endif
    // cas m_ratio == 0

    Topo::Block* b1 = new Topo::Block(getContext(), 4*m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques du bloc
    addCreatedBlock(b1);

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // on utilise directement les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 sommets géométriques mais "
                <<vertices.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

//    for (uint i=0; i<6; i++)
//        std::cout<<" i "<<i<<" "<<vertices[i]->getName()<<" en "<<vertices[i]->getCoord()<<std::endl;

    b1->getVertex(false,false,false)->setCoord(vertices[3]->getCoord());
    b1->getVertex(true ,false,false)->setCoord(vertices[2]->getCoord());
    b1->getVertex(false,true ,false)->setCoord(vertices[1]->getCoord());
    b1->getVertex(true ,true ,false)->setCoord(vertices[0]->getCoord());
    b1->getVertex(false,false,true )->setCoord(vertices[5]->getCoord());
    b1->getVertex(true ,false,true )->setCoord(vertices[5]->getCoord());
    b1->getVertex(false,true ,true )->setCoord(vertices[4]->getCoord());
    b1->getVertex(true ,true ,true )->setCoord(vertices[4]->getCoord());

    // dégénéréscence des blocs
    b1->degenerateFaceInEdge(Block::k_max, b1->getVertex(4), b1->getVertex(5), &getInfoCommand());

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 5){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 5 surfaces géométriques mais "
                <<surfaces.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // on considère que la première surface est celle qui enveloppe
    b1->getFace(Block::k_min)->setGeomAssociation(surfaces[0]);

    // la deuxième est au sommet du cylindre
    b1->getFace(Block::j_max)->setGeomAssociation(surfaces[1]);

    // la troisième surface est à la base du cylindre
    b1->getFace(Block::j_min)->setGeomAssociation(surfaces[2]);

    b1->getFace(Block::i_min)->setGeomAssociation(surfaces[3]);
    b1->getFace(Block::i_max)->setGeomAssociation(surfaces[4]);


    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 9){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 9 courbes géométriques mais "
                <<curves.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // on considère que la première courbe est celle qui est en j max
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(2), b1->getVertex(3))->setGeomAssociation(curves[0]);

    b1->getFace(Block::k_min)->getEdge(b1->getVertex(2), b1->getVertex(0))->setGeomAssociation(curves[3]);

    // on considère que la 3ème courbe est celle qui est à la base du cylindre
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->setGeomAssociation(curves[2]);

    b1->getFace(Block::k_min)->getEdge(b1->getVertex(3), b1->getVertex(1))->setGeomAssociation(curves[1]);


    b1->getFace(Block::j_min)->getEdge(b1->getVertex(0), b1->getVertex(4))->setGeomAssociation(curves[7]);
    b1->getFace(Block::j_min)->getEdge(b1->getVertex(1), b1->getVertex(4))->setGeomAssociation(curves[6]);

    b1->getFace(Block::j_max)->getEdge(b1->getVertex(2), b1->getVertex(5))->setGeomAssociation(curves[4]);
    b1->getFace(Block::j_max)->getEdge(b1->getVertex(3), b1->getVertex(5))->setGeomAssociation(curves[5]);

    b1->getFace(Block::i_min)->getEdge(b1->getVertex(4), b1->getVertex(5))->setGeomAssociation(curves[8]);


    b1->getVertex(0)->setGeomAssociation(vertices[3]);
    b1->getVertex(1)->setGeomAssociation(vertices[2]);
    b1->getVertex(2)->setGeomAssociation(vertices[1]);
    b1->getVertex(3)->setGeomAssociation(vertices[0]);
    b1->getVertex(4)->setGeomAssociation(vertices[5]);
    b1->getVertex(5)->setGeomAssociation(vertices[4]);

} // createCylinderTopoOGridQuartDeg
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createCylinderTopoOGridQuartNonDeg(Geom::PropertyCylinder* propertyCyl)
{
#ifdef _DEBUG2
    std::cout<<"createCylinderTopoOGridQuartNonDeg(...)"<<std::endl;
#endif

    Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_i);
    Topo::Block* b2 = new Topo::Block(getContext(), m_nr, m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_i);
    Topo::Block* b3 = new Topo::Block(getContext(), m_ni, m_nr, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_j);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b3);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b3->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 5){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 5 surfaces géométriques mais "
                <<surfaces.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 9){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 9 courbes géométriques mais "
                <<curves.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // on utilise directement les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 sommets géométriques mais "
                <<vertices.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // le point sur la courbe de même indice
    Utils::Math::Point p0, p2, p4, p5, p6, p7;
    // le point au sommet de l'ogrid (sur la surface extrémité du cylindre)
    Utils::Math::Point p1, p3;

    // p0 est au milieu de la courbe c0 (entre s0 et s1)
    double param1, param2;
    curves[0]->getParameter(vertices[0]->getCoord(), param1);
    curves[0]->getParameter(vertices[1]->getCoord(), param2);
    curves[0]->getPoint((param1+param2)/2, p0);

    curves[2]->getParameter(vertices[2]->getCoord(), param1);
    curves[2]->getParameter(vertices[3]->getCoord(), param2);
    curves[2]->getPoint((param1+param2)/2, p2);

    double ratio = m_ratio/std::sqrt(2);

    p5 = vertices[4]->getCoord()+(vertices[0]->getCoord()-vertices[4]->getCoord())*ratio;
    p4 = vertices[4]->getCoord()+(vertices[1]->getCoord()-vertices[4]->getCoord())*ratio;

    p6 = vertices[5]->getCoord()+(vertices[2]->getCoord()-vertices[5]->getCoord())*ratio;
    p7 = vertices[5]->getCoord()+(vertices[3]->getCoord()-vertices[5]->getCoord())*ratio;

    p1 = p5+p4-vertices[4]->getCoord();
    p3 = p6+p7-vertices[5]->getCoord();

    b1->getVertex(0)->setCoord(vertices[4]->getCoord());
    b1->getVertex(1)->setCoord(p5);
    b1->getVertex(2)->setCoord(p4);
    b1->getVertex(3)->setCoord(p1);
    b1->getVertex(4)->setCoord(vertices[5]->getCoord());
    b1->getVertex(5)->setCoord(p6);
    b1->getVertex(6)->setCoord(p7);
    b1->getVertex(7)->setCoord(p3);

    b2->getVertex(0)->setCoord(p5);
    b2->getVertex(1)->setCoord(vertices[0]->getCoord());
    b2->getVertex(2)->setCoord(p1);
    b2->getVertex(3)->setCoord(p0);
    b2->getVertex(4)->setCoord(p6);
    b2->getVertex(5)->setCoord(vertices[2]->getCoord());
    b2->getVertex(6)->setCoord(p3);
    b2->getVertex(7)->setCoord(p2);

    b3->getVertex(0)->setCoord(p4);
    b3->getVertex(1)->setCoord(p1);
    b3->getVertex(2)->setCoord(vertices[1]->getCoord());
    b3->getVertex(3)->setCoord(p0);
    b3->getVertex(4)->setCoord(p7);
    b3->getVertex(5)->setCoord(p3);
    b3->getVertex(6)->setCoord(vertices[3]->getCoord());
    b3->getVertex(7)->setCoord(p2);


    // fusion des faces
    b1->getFace(Block::i_max)->fuse(b2->getFace(Block::i_min), &getInfoCommand());
    b1->getFace(Block::j_max)->fuse(b3->getFace(Block::j_min), &getInfoCommand());
    b2->getFace(Block::j_max)->fuse(b3->getFace(Block::i_max), &getInfoCommand());


    // on considère que la première surface est celle qui enveloppe
    b2->getFace(Block::i_max)->setGeomAssociation(surfaces[0]);
    b3->getFace(Block::j_max)->setGeomAssociation(surfaces[0]);

    // la deuxième est au sommet du cylindre
    b1->getFace(Block::k_min)->setGeomAssociation(surfaces[1]);
    b2->getFace(Block::k_min)->setGeomAssociation(surfaces[1]);
    b3->getFace(Block::k_min)->setGeomAssociation(surfaces[1]);

    // la troisième est à la base du cylindre
    b1->getFace(Block::k_max)->setGeomAssociation(surfaces[2]);
    b2->getFace(Block::k_max)->setGeomAssociation(surfaces[2]);
    b3->getFace(Block::k_max)->setGeomAssociation(surfaces[2]);


    b1->getFace(Block::j_min)->setGeomAssociation(surfaces[4]);
    b2->getFace(Block::j_min)->setGeomAssociation(surfaces[4]);

    b1->getFace(Block::i_min)->setGeomAssociation(surfaces[3]);
    b3->getFace(Block::i_min)->setGeomAssociation(surfaces[3]);


    // on considère que la courbe 0 est celle qui est à au sommet du cylindre
    b2->getFace(Block::k_min)->getEdge(b2->getVertex(1), b2->getVertex(3))->setGeomAssociation(curves[0]);
    b3->getFace(Block::k_min)->getEdge(b3->getVertex(2), b3->getVertex(3))->setGeomAssociation(curves[0]);
    b2->getVertex(3)->setGeomAssociation(curves[0]);

    // on considère que la courbe 2 est celle qui est à la base du cylindre
    b2->getFace(Block::k_max)->getEdge(b2->getVertex(5), b2->getVertex(7))->setGeomAssociation(curves[2]);
    b3->getFace(Block::k_max)->getEdge(b3->getVertex(6), b3->getVertex(7))->setGeomAssociation(curves[2]);
    b2->getVertex(7)->setGeomAssociation(curves[2]);

    b2->getFace(Block::k_min)->getEdge(b2->getVertex(0), b2->getVertex(1))->setGeomAssociation(curves[5]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->setGeomAssociation(curves[5]);
    b1->getVertex(1)->setGeomAssociation(curves[5]);

    b3->getFace(Block::k_min)->getEdge(b3->getVertex(0), b3->getVertex(2))->setGeomAssociation(curves[4]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(2))->setGeomAssociation(curves[4]);
    b1->getVertex(2)->setGeomAssociation(curves[4]);

    b1->getFace(Block::k_min)->getEdge(b1->getVertex(1), b1->getVertex(3))->setGeomAssociation(surfaces[1]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(2), b1->getVertex(3))->setGeomAssociation(surfaces[1]);
    b2->getFace(Block::k_min)->getEdge(b2->getVertex(2), b2->getVertex(3))->setGeomAssociation(surfaces[1]);
    b1->getVertex(3)->setGeomAssociation(surfaces[1]);


    b2->getFace(Block::k_max)->getEdge(b2->getVertex(4), b2->getVertex(5))->setGeomAssociation(curves[6]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(4), b1->getVertex(5))->setGeomAssociation(curves[6]);
    b1->getVertex(5)->setGeomAssociation(curves[6]);

    b3->getFace(Block::k_max)->getEdge(b3->getVertex(4), b3->getVertex(6))->setGeomAssociation(curves[7]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(4), b1->getVertex(6))->setGeomAssociation(curves[7]);
    b1->getVertex(6)->setGeomAssociation(curves[7]);

    b1->getFace(Block::k_max)->getEdge(b1->getVertex(5), b1->getVertex(7))->setGeomAssociation(surfaces[2]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(6), b1->getVertex(7))->setGeomAssociation(surfaces[2]);
    b2->getFace(Block::k_max)->getEdge(b2->getVertex(6), b2->getVertex(7))->setGeomAssociation(surfaces[2]);
    b1->getVertex(7)->setGeomAssociation(surfaces[2]);


    b1->getFace(Block::j_min)->getEdge(b1->getVertex(0), b1->getVertex(4))->setGeomAssociation(curves[8]);
    b1->getFace(Block::j_min)->getEdge(b1->getVertex(1), b1->getVertex(5))->setGeomAssociation(surfaces[4]);
    b2->getFace(Block::j_min)->getEdge(b2->getVertex(1), b2->getVertex(5))->setGeomAssociation(curves[1]);


    b3->getFace(Block::i_min)->getEdge(b3->getVertex(0), b3->getVertex(4))->setGeomAssociation(surfaces[3]);
    b3->getFace(Block::i_min)->getEdge(b3->getVertex(2), b3->getVertex(6))->setGeomAssociation(curves[3]);

    b3->getFace(Block::i_max)->getEdge(b3->getVertex(3), b3->getVertex(7))->setGeomAssociation(surfaces[0]);


    b1->getVertex(0)->setGeomAssociation(vertices[4]);
    b1->getVertex(4)->setGeomAssociation(vertices[5]);
    b2->getVertex(1)->setGeomAssociation(vertices[0]);
    b2->getVertex(5)->setGeomAssociation(vertices[2]);
    b3->getVertex(2)->setGeomAssociation(vertices[1]);
    b3->getVertex(6)->setGeomAssociation(vertices[3]);

} // createCylinderTopoOGridQuartNonDeg
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createCylinderTopoOGridDemiNonDeg(Geom::PropertyCylinder* propertyCyl)
{
#ifdef _DEBUG2
    std::cout<<"createCylinderTopoOGridDemiNonDeg(...)"<<std::endl;
#endif


    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_j);
    Topo::Block* b2 = new Topo::Block(getContext(), m_nr, m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_i);
    Topo::Block* b3 = new Topo::Block(getContext(), 2*m_ni, m_nr, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_j);
    Topo::Block* b4 = new Topo::Block(getContext(), m_nr, m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_i);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b3);
    addCreatedBlock(b4);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b3->setGeomAssociation(getGeomEntity());
    b4->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 5){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 5 surfaces géométriques mais "
                <<surfaces.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 9){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 9 courbes géométriques mais "
                <<curves.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // on utilise directement les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 sommets géométriques mais "
                <<vertices.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    // le point sur la courbe de même indice
    Utils::Math::Point p0, p1, p2, p3, p4, p5, p6, p7;
    // le point au sommet de l'ogrid (sur la surface extrémité du cylindre)
    Utils::Math::Point p8, p9, p10, p11;

    // p0 est au premier quart de la courbe c0 (entre s0 et s1)
    // p1 au 3 ème quart
    double param1, param2;
    curves[0]->getParameter(vertices[0]->getCoord(), param1);
    curves[0]->getParameter(vertices[1]->getCoord(), param2);
    curves[0]->getPoint(param1+(param2-param1)/4, p0);
    curves[0]->getPoint(param1+(param2-param1)*3/4, p1);

    curves[2]->getParameter(vertices[2]->getCoord(), param1);
    curves[2]->getParameter(vertices[3]->getCoord(), param2);
    curves[2]->getPoint(param1+(param2-param1)/4, p2);
    curves[2]->getPoint(param1+(param2-param1)*3/4, p3);

    double ratio = m_ratio/std::sqrt(2);

    p5 = vertices[4]->getCoord()+(vertices[0]->getCoord()-vertices[4]->getCoord())*ratio;
    p4 = vertices[4]->getCoord()+(vertices[1]->getCoord()-vertices[4]->getCoord())*ratio;

    p6 = vertices[5]->getCoord()+(vertices[2]->getCoord()-vertices[5]->getCoord())*ratio;
    p7 = vertices[5]->getCoord()+(vertices[3]->getCoord()-vertices[5]->getCoord())*ratio;

    p8 = vertices[4]->getCoord()+(p0-vertices[4]->getCoord())*m_ratio;
    p9 = vertices[4]->getCoord()+(p1-vertices[4]->getCoord())*m_ratio;

    p10 = vertices[5]->getCoord()+(p2-vertices[5]->getCoord())*m_ratio;
    p11 = vertices[5]->getCoord()+(p3-vertices[5]->getCoord())*m_ratio;


    b1->getVertex(0)->setCoord(p4);
    b1->getVertex(1)->setCoord(p5);
    b1->getVertex(2)->setCoord(p9);
    b1->getVertex(3)->setCoord(p8);
    b1->getVertex(4)->setCoord(p7);
    b1->getVertex(5)->setCoord(p6);
    b1->getVertex(6)->setCoord(p11);
    b1->getVertex(7)->setCoord(p10);

    b2->getVertex(0)->setCoord(p5);
    b2->getVertex(1)->setCoord(vertices[0]->getCoord());
    b2->getVertex(2)->setCoord(p8);
    b2->getVertex(3)->setCoord(p0);
    b2->getVertex(4)->setCoord(p6);
    b2->getVertex(5)->setCoord(vertices[2]->getCoord());
    b2->getVertex(6)->setCoord(p10);
    b2->getVertex(7)->setCoord(p2);

    b3->getVertex(0)->setCoord(p9);
    b3->getVertex(1)->setCoord(p8);
    b3->getVertex(2)->setCoord(p1);
    b3->getVertex(3)->setCoord(p0);
    b3->getVertex(4)->setCoord(p11);
    b3->getVertex(5)->setCoord(p10);
    b3->getVertex(6)->setCoord(p3);
    b3->getVertex(7)->setCoord(p2);

    b4->getVertex(0)->setCoord(vertices[1]->getCoord());
    b4->getVertex(1)->setCoord(p4);
    b4->getVertex(2)->setCoord(p1);
    b4->getVertex(3)->setCoord(p9);
    b4->getVertex(4)->setCoord(vertices[3]->getCoord());
    b4->getVertex(5)->setCoord(p7);
    b4->getVertex(6)->setCoord(p3);
    b4->getVertex(7)->setCoord(p11);


    // fusion des faces
    b1->getFace(Block::i_max)->fuse(b2->getFace(Block::i_min), &getInfoCommand());
    b1->getFace(Block::j_max)->fuse(b3->getFace(Block::j_min), &getInfoCommand());
    b1->getFace(Block::i_min)->fuse(b4->getFace(Block::i_max), &getInfoCommand());
    b2->getFace(Block::j_max)->fuse(b3->getFace(Block::i_max), &getInfoCommand());
    b3->getFace(Block::i_min)->fuse(b4->getFace(Block::j_max), &getInfoCommand());

    // on coupe la face du bloc b1 pour qu'une arête soit sur la courbe 8
    std::vector<Edge* > splitingEdges;
    std::vector<CoFace* > cofaces;
    cofaces.push_back(b1->getFace(Block::j_min)->getCoFace(0));
    TopoHelper::splitFaces(cofaces,
            b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->getCoEdge(0),
            0.5, 0.0, false, false,
            splitingEdges,
            &getInfoCommand());

    Vertex* v0 = TopoHelper::getCommonVertex(splitingEdges[0]->getCoEdge(0),
            b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->getCoEdge(0));
    Vertex* v1 = TopoHelper::getCommonVertex(splitingEdges[0]->getCoEdge(0),
            b1->getFace(Block::k_max)->getEdge(b1->getVertex(4), b1->getVertex(5))->getCoEdge(0));

    v0->setCoord(vertices[4]->getCoord());
    v1->setCoord(vertices[5]->getCoord());

    splitingEdges[0]->getCoEdge(0)->setGeomAssociation(curves[8]);

    // on considère que la première surface est celle qui enveloppe
    b2->getFace(Block::i_max)->setGeomAssociation(surfaces[0]);
    b3->getFace(Block::j_max)->setGeomAssociation(surfaces[0]);
    b4->getFace(Block::i_min)->setGeomAssociation(surfaces[0]);

    // la deuxième est au sommet du cylindre
    b1->getFace(Block::k_min)->setGeomAssociation(surfaces[1]);
    b2->getFace(Block::k_min)->setGeomAssociation(surfaces[1]);
    b3->getFace(Block::k_min)->setGeomAssociation(surfaces[1]);
    b4->getFace(Block::k_min)->setGeomAssociation(surfaces[1]);

    // la troisième est à la base du cylindre
    b1->getFace(Block::k_max)->setGeomAssociation(surfaces[2]);
    b2->getFace(Block::k_max)->setGeomAssociation(surfaces[2]);
    b3->getFace(Block::k_max)->setGeomAssociation(surfaces[2]);
    b4->getFace(Block::k_max)->setGeomAssociation(surfaces[2]);

    b2->getFace(Block::j_min)->setGeomAssociation(surfaces[4]);

    b4->getFace(Block::j_min)->setGeomAssociation(surfaces[3]);

    CoFace* coface0 = b1->getFace(Block::j_min)->getCoFace(0);
    if (coface0->find(b1->getVertex(1))){
        b1->getFace(Block::j_min)->getCoFace(0)->setGeomAssociation(surfaces[4]);
        b1->getFace(Block::j_min)->getCoFace(1)->setGeomAssociation(surfaces[3]);
    } else {
        b1->getFace(Block::j_min)->getCoFace(1)->setGeomAssociation(surfaces[4]);
        b1->getFace(Block::j_min)->getCoFace(0)->setGeomAssociation(surfaces[3]);
    }

    // on considère que la courbe 0 est celle qui est à au sommet du cylindre
    b2->getFace(Block::k_min)->getEdge(b2->getVertex(1), b2->getVertex(3))->setGeomAssociation(curves[0]);
    b3->getFace(Block::k_min)->getEdge(b3->getVertex(2), b3->getVertex(3))->setGeomAssociation(curves[0]);
    b4->getFace(Block::k_min)->getEdge(b4->getVertex(2), b4->getVertex(0))->setGeomAssociation(curves[0]);
    b2->getVertex(3)->setGeomAssociation(curves[0]);
    b3->getVertex(2)->setGeomAssociation(curves[0]);

    // on considère que la courbe 2 est celle qui est à la base du cylindre
    b2->getFace(Block::k_max)->getEdge(b2->getVertex(5), b2->getVertex(7))->setGeomAssociation(curves[2]);
    b3->getFace(Block::k_max)->getEdge(b3->getVertex(6), b3->getVertex(7))->setGeomAssociation(curves[2]);
    b4->getFace(Block::k_max)->getEdge(b4->getVertex(6), b4->getVertex(4))->setGeomAssociation(curves[2]);
    b2->getVertex(7)->setGeomAssociation(curves[2]);
    b3->getVertex(6)->setGeomAssociation(curves[2]);

    b2->getFace(Block::k_min)->getEdge(b2->getVertex(0), b2->getVertex(1))->setGeomAssociation(curves[5]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->getCoEdge(v0, b1->getVertex(1))->setGeomAssociation(curves[5]);
    b1->getVertex(1)->setGeomAssociation(curves[5]);

    b4->getFace(Block::k_min)->getEdge(b4->getVertex(0), b4->getVertex(1))->setGeomAssociation(curves[4]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->getCoEdge(b1->getVertex(0), v0)->setGeomAssociation(curves[4]);
    b1->getVertex(0)->setGeomAssociation(curves[4]);

    b1->getFace(Block::k_min)->getEdge(b1->getVertex(1), b1->getVertex(3))->setGeomAssociation(surfaces[1]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(2), b1->getVertex(3))->setGeomAssociation(surfaces[1]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(2), b1->getVertex(0))->setGeomAssociation(surfaces[1]);
    b2->getFace(Block::k_min)->getEdge(b2->getVertex(2), b2->getVertex(3))->setGeomAssociation(surfaces[1]);
    b4->getFace(Block::k_min)->getEdge(b4->getVertex(2), b4->getVertex(3))->setGeomAssociation(surfaces[1]);
    b1->getVertex(3)->setGeomAssociation(surfaces[1]);
    b1->getVertex(2)->setGeomAssociation(surfaces[1]);


    b2->getFace(Block::k_max)->getEdge(b2->getVertex(4), b2->getVertex(5))->setGeomAssociation(curves[6]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(4), b1->getVertex(5))->getCoEdge(v1, b1->getVertex(5))->setGeomAssociation(curves[6]);
    b1->getVertex(5)->setGeomAssociation(curves[6]);

    b4->getFace(Block::k_max)->getEdge(b4->getVertex(4), b4->getVertex(5))->setGeomAssociation(curves[7]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(4), b1->getVertex(5))->getCoEdge(b1->getVertex(4), v1)->setGeomAssociation(curves[7]);
    b1->getVertex(4)->setGeomAssociation(curves[7]);

    b1->getFace(Block::k_max)->getEdge(b1->getVertex(5), b1->getVertex(7))->setGeomAssociation(surfaces[2]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(6), b1->getVertex(7))->setGeomAssociation(surfaces[2]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(6), b1->getVertex(4))->setGeomAssociation(surfaces[2]);
    b2->getFace(Block::k_max)->getEdge(b2->getVertex(6), b2->getVertex(7))->setGeomAssociation(surfaces[2]);
    b4->getFace(Block::k_max)->getEdge(b4->getVertex(6), b4->getVertex(7))->setGeomAssociation(surfaces[2]);
    b1->getVertex(7)->setGeomAssociation(surfaces[2]);
    b1->getVertex(6)->setGeomAssociation(surfaces[2]);


    b1->getFace(Block::j_min)->getEdge(b1->getVertex(1), b1->getVertex(5))->setGeomAssociation(surfaces[4]);
    b2->getFace(Block::j_min)->getEdge(b2->getVertex(1), b2->getVertex(5))->setGeomAssociation(curves[1]);


    b1->getFace(Block::j_min)->getEdge(b1->getVertex(0), b1->getVertex(4))->setGeomAssociation(surfaces[3]);
    b4->getFace(Block::i_min)->getEdge(b4->getVertex(0), b4->getVertex(4))->setGeomAssociation(curves[3]);

    b3->getFace(Block::j_max)->getEdge(b3->getVertex(3), b3->getVertex(7))->setGeomAssociation(surfaces[0]);
    b3->getFace(Block::j_max)->getEdge(b3->getVertex(2), b3->getVertex(6))->setGeomAssociation(surfaces[0]);

    b2->getVertex(1)->setGeomAssociation(vertices[0]);
    b2->getVertex(5)->setGeomAssociation(vertices[2]);
    b4->getVertex(0)->setGeomAssociation(vertices[1]);
    b4->getVertex(4)->setGeomAssociation(vertices[3]);
    v0->setGeomAssociation(vertices[4]);
    v1->setGeomAssociation(vertices[5]);

} // createCylinderTopoOGridDemiNonDeg
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopo1Block()
{
    Geom::PropertySphere* propertySph = getGeomEntity()->getGeomProperty()->toPropertySphere();
#ifdef _DEBUG2
    std::cout<<"createSphereTopo1Block avec portion de "<<Utils::Portion::getName(propertySph->getPortionType())<<std::endl;
#endif

    Utils::Portion::Type dt = propertySph->getPortionType();

    if (dt == Utils::Portion::ENTIER)
        createSphereTopo1BlockPlein(propertySph);
    else if (dt == Utils::Portion::DEMI)
        createSphereTopo1BlockDemi(propertySph);
    else if (dt == Utils::Portion::QUART)
        createSphereTopo1BlockQuart(propertySph);
    else if (dt == Utils::Portion::HUITIEME)
        createSphereTopo1BlockHuitieme(propertySph);
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOGridOnGeometry ne peut se faire pour cette portion de sphère", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopoOGrid()
{
    Geom::PropertySphere* propertySph = getGeomEntity()->getGeomProperty()->toPropertySphere();
#ifdef _DEBUG2
    std::cout<<"createSphereTopoOGrid avec portion de "<<Utils::Portion::getName(propertySph->getPortionType())<<std::endl;
#endif

    Utils::Portion::Type dt = propertySph->getPortionType();

    if (m_ratio > 0.0){
        if (dt == Utils::Portion::ENTIER)
            createSphereTopoOGridPleinNonDeg(propertySph);
        else if (dt == Utils::Portion::DEMI)
            createSphereTopoOGridDemiNonDeg(propertySph);
        else if (dt == Utils::Portion::QUART)
            createSphereTopoOGridQuartNonDeg(propertySph);
        else if (dt == Utils::Portion::HUITIEME)
            createSphereTopoOGridHuitiemeNonDeg(propertySph);
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOGridOnGeometry ne peut se faire pour cette portion de sphère", TkUtil::Charset::UTF_8));
    }
    else {
        if (dt == Utils::Portion::ENTIER)
            createSphereTopoOGridPleinDeg(propertySph);
        else if (dt == Utils::Portion::DEMI)
            createSphereTopoOGridDemiDeg(propertySph);
        else if (dt == Utils::Portion::QUART)
            createSphereTopoOGridQuartDeg(propertySph);
        else if (dt == Utils::Portion::HUITIEME)
            createSphereTopoOGridHuitiemeDeg(propertySph);
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOGridOnGeometry ne peut se faire pour cette portion de sphère", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopo1BlockPlein(Geom::PropertySphere* propertySph)
{
    Utils::Math::Point centre = propertySph->getCenter();
    double rayon = propertySph->getRadius();
    double d3 = std::sqrt(3.0)/3.0;

    double r2 = rayon*d3;

    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, 2*m_ni,
            BlockMeshingProperty::transfinite);
    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    b1->getVertex(false,false,false)->setCoord(centre+Utils::Math::Point(-r2, -r2, -r2));
    b1->getVertex(true, false,false)->setCoord(centre+Utils::Math::Point( r2, -r2, -r2));
    b1->getVertex(false,true, false)->setCoord(centre+Utils::Math::Point(-r2,  r2, -r2));
    b1->getVertex(true, true, false)->setCoord(centre+Utils::Math::Point( r2,  r2, -r2));
    b1->getVertex(false,false,true )->setCoord(centre+Utils::Math::Point(-r2, -r2,  r2));
    b1->getVertex(true, false,true )->setCoord(centre+Utils::Math::Point( r2, -r2,  r2));
    b1->getVertex(false,true, true )->setCoord(centre+Utils::Math::Point(-r2,  r2,  r2));
    b1->getVertex(true, true, true )->setCoord(centre+Utils::Math::Point( r2,  r2,  r2));

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 1){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 1 surface géométrique mais "
                <<surfaces.size()<<" pour la sphère pleine "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    for (uint i=0; i<6; i++){
        Topo::Face* face = b1->getFace(i);
        face->setGeomAssociation(surfaces[0]);
        for (uint j=0; j<4; j++)
            face->getEdge(j)->setGeomAssociation(surfaces[0]);
    }

    for (uint i=0; i<8; i++)
        b1->getVertex(i)->setGeomAssociation(surfaces[0]);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopo1BlockDemi(Geom::PropertySphere* propertySph)
{
    Utils::Math::Point centre = propertySph->getCenter();
    double rayon = propertySph->getRadius();
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;

    double r2 = rayon*d2;
    double r3 = rayon*d3;

    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, m_ni, 2*m_ni,
            BlockMeshingProperty::transfinite);
    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    b1->getVertex(false,false,false)->setCoord(centre+Utils::Math::Point(-r2,   0, -r2));
    b1->getVertex(true, false,false)->setCoord(centre+Utils::Math::Point( r2,   0, -r2));
    b1->getVertex(false,true, false)->setCoord(centre+Utils::Math::Point(-r3,  r3, -r3));
    b1->getVertex(true, true, false)->setCoord(centre+Utils::Math::Point( r3,  r3, -r3));
    b1->getVertex(false,false,true )->setCoord(centre+Utils::Math::Point(-r2,   0,  r2));
    b1->getVertex(true, false,true )->setCoord(centre+Utils::Math::Point( r2,   0,  r2));
    b1->getVertex(false,true, true )->setCoord(centre+Utils::Math::Point(-r3,  r3,  r3));
    b1->getVertex(true, true, true )->setCoord(centre+Utils::Math::Point( r3,  r3,  r3));


    // on coupe la face du bloc b1 pour qu'une arête soit sur la courbe 2
    std::vector<Edge* > splitingEdges;
    std::vector<CoFace* > cofaces;
    cofaces.push_back(b1->getFace(Block::j_min)->getCoFace(0));
    TopoHelper::splitFaces(cofaces,
            b1->getFace(Block::j_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->getCoEdge(0),
            0.5, 0.0, false, false,
            splitingEdges,
            &getInfoCommand());

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 surfaces géométriques mais "
                <<surfaces.size()<<" pour la demi sphère pleine "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    for (uint i=0; i<6; i++){
        if (i==2)
            continue;
        Topo::Face* face = b1->getFace(i);
        face->setGeomAssociation(surfaces[0]);
        for (uint j=0; j<4; j++)
            face->getEdge(j)->setGeomAssociation(surfaces[0]);
    }

    for (uint i=0; i<8; i++)
        b1->getVertex(i)->setGeomAssociation(surfaces[0]);

    b1->getFace(Block::j_min)->getCoFaces(cofaces);
    if (cofaces[0]->find(b1->getVertex(0))){
        cofaces[0]->setGeomAssociation(surfaces[2]);
        cofaces[1]->setGeomAssociation(surfaces[1]);
    } else if (cofaces[0]->find(b1->getVertex(1))){
        cofaces[0]->setGeomAssociation(surfaces[1]);
        cofaces[1]->setGeomAssociation(surfaces[2]);
    } else
        throw TkUtil::Exception (TkUtil::UTF8String ("Echec pour projeter les CoFaces issues de la coupe de b1", TkUtil::Charset::UTF_8));

    // les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 2 sommets géométriques mais "
                <<vertices.size()<<" pour la demi sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Vertex* v0 = 0;
    Vertex* v1 = 0;
    std::vector<CoEdge* > coedges;
    for (uint i=0; i<splitingEdges.size(); i++)
        coedges.push_back(splitingEdges[i]->getCoEdge(0));
    TopoHelper::getVerticesTip(coedges, v0, v1, false);

    // on met v0 en z max sur la sphère
    if (v0->getCoord().getZ() < v1->getCoord().getZ()){
        Vertex* vtmp =  v0;
        v0 = v1;
        v1 = vtmp;
    }
    v0->setGeomAssociation(vertices[0]);
    v1->setGeomAssociation(vertices[1]);
    v0->setCoord(vertices[0]->getCoord());
    v1->setCoord(vertices[1]->getCoord());

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 courbes géométriques mais "
                <<curves.size()<<" pour la demi sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    for (uint i=0; i<b1->getFace(Block::j_min)->getNbCoFaces(); i++){
        CoFace* coface = b1->getFace(Block::j_min)->getCoFace(i);
        for (uint j=0; j<coface->getNbEdges(); j++)
            coface->getEdge(j)->setGeomAssociation(coface->getGeomAssociation());
    }

    b1->getFace(Block::j_min)->getEdge(v1, b1->getVertex(1))->setGeomAssociation(curves[1]);
    b1->getFace(Block::j_min)->getEdge(b1->getVertex(1), b1->getVertex(5))->setGeomAssociation(curves[1]);
    b1->getFace(Block::j_min)->getEdge(v0, b1->getVertex(5))->setGeomAssociation(curves[1]);

    b1->getFace(Block::j_min)->getEdge(v1, b1->getVertex(0))->setGeomAssociation(curves[0]);
    b1->getFace(Block::j_min)->getEdge(b1->getVertex(0), b1->getVertex(4))->setGeomAssociation(curves[0]);
    b1->getFace(Block::j_min)->getEdge(v0, b1->getVertex(4))->setGeomAssociation(curves[0]);

    b1->getVertex(1)->setGeomAssociation(curves[1]);
    b1->getVertex(5)->setGeomAssociation(curves[1]);

    b1->getVertex(0)->setGeomAssociation(curves[0]);
    b1->getVertex(4)->setGeomAssociation(curves[0]);


    // sur l'axe des Z
    for (uint i=0; i<splitingEdges.size(); i++)
        splitingEdges[i]->setGeomAssociation(curves[2]);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopo1BlockQuart(Geom::PropertySphere* propertySph)
{
    Utils::Math::Point centre = propertySph->getCenter();
    double rayon = propertySph->getRadius();
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;

    double r2 = rayon*d2;
    double r3 = rayon*d3;

    Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_ni, 2*m_ni,
            BlockMeshingProperty::transfinite);
    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    b1->getVertex(false,false,false)->setCoord(centre+Utils::Math::Point(  0,   0, -r2));
    b1->getVertex(true, false,false)->setCoord(centre+Utils::Math::Point( r2,   0, -r2));
    b1->getVertex(false,true, false)->setCoord(centre+Utils::Math::Point(  0,  r3, -r3));
    b1->getVertex(true, true, false)->setCoord(centre+Utils::Math::Point( r3,  r3, -r3));
    b1->getVertex(false,false,true )->setCoord(centre+Utils::Math::Point(  0,   0,  r2));
    b1->getVertex(true, false,true )->setCoord(centre+Utils::Math::Point( r2,   0,  r2));
    b1->getVertex(false,true, true )->setCoord(centre+Utils::Math::Point(  0,  r2,  r2));
    b1->getVertex(true, true, true )->setCoord(centre+Utils::Math::Point( r3,  r3,  r3));


    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 surfaces géométriques mais "
                <<surfaces.size()<<" pour le quart de sphère pleine "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    for (uint i=0; i<6; i++){
        if (i==2 || i==0)
            continue;
        Topo::Face* face = b1->getFace(i);
        face->setGeomAssociation(surfaces[0]);
        for (uint j=0; j<4; j++)
            face->getEdge(j)->setGeomAssociation(surfaces[0]);
    }

    for (uint i=0; i<8; i++)
        b1->getVertex(i)->setGeomAssociation(surfaces[0]);

    b1->getFace(Block::i_min)->setGeomAssociation(surfaces[2]);
    b1->getFace(Block::j_min)->setGeomAssociation(surfaces[1]);

    // les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 2 sommets géométriques mais "
                <<vertices.size()<<" pour le quart de sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Vertex* v0 = b1->getVertex(4);
    Vertex* v1 = b1->getVertex(0);
    v0->setGeomAssociation(vertices[0]);
    v1->setGeomAssociation(vertices[1]);
    v0->setCoord(vertices[0]->getCoord());
    v1->setCoord(vertices[1]->getCoord());

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 courbes géométriques mais "
                <<curves.size()<<" pour le quart de sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    b1->getFace(Block::i_min)->getEdge(b1->getVertex(0), b1->getVertex(2))->setGeomAssociation(curves[0]);
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(2), b1->getVertex(6))->setGeomAssociation(curves[0]);
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(6), b1->getVertex(4))->setGeomAssociation(curves[0]);

    b1->getFace(Block::j_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->setGeomAssociation(curves[1]);
    b1->getFace(Block::j_min)->getEdge(b1->getVertex(1), b1->getVertex(5))->setGeomAssociation(curves[1]);
    b1->getFace(Block::j_min)->getEdge(b1->getVertex(5), b1->getVertex(4))->setGeomAssociation(curves[1]);

    b1->getFace(Block::i_min)->getEdge(b1->getVertex(0), b1->getVertex(4))->setGeomAssociation(curves[2]);

    b1->getVertex(2)->setGeomAssociation(curves[0]);
    b1->getVertex(6)->setGeomAssociation(curves[0]);

    b1->getVertex(1)->setGeomAssociation(curves[1]);
    b1->getVertex(5)->setGeomAssociation(curves[1]);

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopo1BlockHuitieme(Geom::PropertySphere* propertySph)
{
    Utils::Math::Point centre = propertySph->getCenter();
    double rayon = propertySph->getRadius();
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;

    double r2 = rayon*d2;
    double r3 = rayon*d3;

    Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_ni, m_ni,
            BlockMeshingProperty::transfinite);
    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 sommets géométriques mais "
                <<vertices.size()<<" pour le huitième de sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    b1->getVertex(false,false,false)->setCoord(vertices[3]->getCoord());
    b1->getVertex(true, false,false)->setCoord(vertices[2]->getCoord());
    b1->getVertex(false,true, false)->setCoord(vertices[1]->getCoord());
    b1->getVertex(true, true, false)->setCoord(centre+Utils::Math::Point( r2,  r2,   0));
    b1->getVertex(false,false,true )->setCoord(vertices[0]->getCoord());
    b1->getVertex(true, false,true )->setCoord(centre+Utils::Math::Point( r2,   0,  r2));
    b1->getVertex(false,true, true )->setCoord(centre+Utils::Math::Point(  0,  r2,  r2));
    b1->getVertex(true, true, true )->setCoord(centre+Utils::Math::Point( r3,  r3,  r3));

    b1->getVertex(false,false,false)->setGeomAssociation(vertices[3]);
    b1->getVertex(true, false,false)->setGeomAssociation(vertices[2]);
    b1->getVertex(false,true, false)->setGeomAssociation(vertices[1]);
    b1->getVertex(false,false,true )->setGeomAssociation(vertices[0]);

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 surfaces géométriques mais "
                <<surfaces.size()<<" pour le huitième de sphère pleine "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    for (uint i=0; i<6; i++){
        if (i==0 || i==2 || i==4)
            continue;
        Topo::Face* face = b1->getFace(i);
        face->setGeomAssociation(surfaces[0]);
        for (uint j=0; j<4; j++)
            face->getEdge(j)->setGeomAssociation(surfaces[0]);
    }

    b1->getVertex(7)->setGeomAssociation(surfaces[0]);

    b1->getFace(Block::i_min)->setGeomAssociation(surfaces[3]);
    b1->getFace(Block::j_min)->setGeomAssociation(surfaces[2]);
    b1->getFace(Block::k_min)->setGeomAssociation(surfaces[1]);

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 courbes géométriques mais "
                <<curves.size()<<" pour le huitième de sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    b1->getFace(Block::k_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->setGeomAssociation(curves[4]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(2), b1->getVertex(3))->setGeomAssociation(curves[1]);
    b1->getFace(Block::k_min)->getEdge(b1->getVertex(1), b1->getVertex(3))->setGeomAssociation(curves[1]);
    b1->getFace(Block::j_min)->getEdge(b1->getVertex(1), b1->getVertex(5))->setGeomAssociation(curves[2]);
    b1->getFace(Block::j_min)->getEdge(b1->getVertex(4), b1->getVertex(5))->setGeomAssociation(curves[2]);
    b1->getFace(Block::j_min)->getEdge(b1->getVertex(4), b1->getVertex(0))->setGeomAssociation(curves[5]);
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(2), b1->getVertex(6))->setGeomAssociation(curves[0]);
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(4), b1->getVertex(6))->setGeomAssociation(curves[0]);
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(2), b1->getVertex(0))->setGeomAssociation(curves[3]);

    b1->getVertex(3)->setGeomAssociation(curves[1]);
    b1->getVertex(6)->setGeomAssociation(curves[0]);
    b1->getVertex(5)->setGeomAssociation(curves[2]);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopoOGridPleinDeg(Geom::PropertySphere* propertySph)
{
    Utils::Math::Point centre = propertySph->getCenter();
    double rayon = propertySph->getRadius();
    double d3 = std::sqrt(3.0)/3.0;

    double r2 = rayon*d3;

    // constitution des points sur la sphère (position de la topo)
    std::vector<Utils::Math::Point> pts_ext;
    pts_ext.push_back(centre+Utils::Math::Point(-r2, -r2, -r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2, -r2, -r2));
    pts_ext.push_back(centre+Utils::Math::Point(-r2,  r2, -r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,  r2, -r2));
    pts_ext.push_back(centre+Utils::Math::Point(-r2, -r2,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2, -r2,  r2));
    pts_ext.push_back(centre+Utils::Math::Point(-r2,  r2,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,  r2,  r2));

    Topo::Block* b2 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b5 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b6 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b7 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // les blocs externes pour utiliser des boucles
    std::vector<Block*> blocs;
    blocs.push_back(b2);
    blocs.push_back(b3);
    blocs.push_back(b4);
    blocs.push_back(b5);
    blocs.push_back(b6);
    blocs.push_back(b7);

    // stockage dans le manager topologiques des différentes entités
    for (uint j=0; j<6; j++)
        addCreatedBlock(blocs[j]);

    for (uint j=0; j<6; j++)
        blocs[j]->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications


    // on place les sommets
    for (uint j=0; j<6; j++)
        for (uint i=0; i<4; i++){
                blocs[j]->getVertex(i)->setCoord(pts_ext[TopoHelper::tab2IndVtxByFaceOnBlock[j][i]]);
                blocs[j]->getVertex(i+4)->setCoord(centre);
            }

    // dégénéréscence des blocs
    for (uint j=0; j<6; j++)
        blocs[j]->degenerateFaceInVertex(Block::k_max, &getInfoCommand());

    // on fusionne les blocs
    b2->getFace(Block::j_min)->fuse(b4->getFace(Block::j_max), &getInfoCommand());
    b2->getFace(Block::i_min)->fuse(b7->getFace(Block::j_max), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b6->getFace(Block::j_min), &getInfoCommand());
    b4->getFace(Block::i_min)->fuse(b7->getFace(Block::i_max), &getInfoCommand());
    b4->getFace(Block::i_max)->fuse(b6->getFace(Block::i_max), &getInfoCommand());
    b3->getFace(Block::j_max)->fuse(b4->getFace(Block::j_min), &getInfoCommand());
    b3->getFace(Block::i_min)->fuse(b7->getFace(Block::j_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b6->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_min)->fuse(b7->getFace(Block::i_min), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_max)->fuse(b6->getFace(Block::i_min), &getInfoCommand());
    b2->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 1){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 1 surface géométrique mais "
                <<surfaces.size()<<" pour la sphère pleine "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // les projections
    for (uint j=0; j<6; j++){
        Face* face = blocs[j]->getFace(Block::k_min);
        face->setGeomAssociation(surfaces[0]);

        for (uint i=0; i<4; i++){
            face->getEdge(i)->setGeomAssociation(surfaces[0]);
            face->getVertex(i)->setGeomAssociation(surfaces[0]);
        }
    }
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopoOGridPleinNonDeg(Geom::PropertySphere* propertySph)
{
    Utils::Math::Point centre = propertySph->getCenter();
    double rayon = propertySph->getRadius();
    double d3 = std::sqrt(3.0)/3.0;

    double r2 = rayon*d3;

    // constitution des points sur la sphère (position de la topo)
    std::vector<Utils::Math::Point> pts_ext;
    pts_ext.push_back(centre+Utils::Math::Point(-r2, -r2, -r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2, -r2, -r2));
    pts_ext.push_back(centre+Utils::Math::Point(-r2,  r2, -r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,  r2, -r2));
    pts_ext.push_back(centre+Utils::Math::Point(-r2, -r2,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2, -r2,  r2));
    pts_ext.push_back(centre+Utils::Math::Point(-r2,  r2,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,  r2,  r2));

    // constitution de ceux au sommet du bloc central de la topologie
    std::vector<Utils::Math::Point> pts_int;
    r2 = rayon*d3*m_ratio;
    pts_int.push_back(centre+Utils::Math::Point(-r2, -r2, -r2));
    pts_int.push_back(centre+Utils::Math::Point( r2, -r2, -r2));
    pts_int.push_back(centre+Utils::Math::Point(-r2,  r2, -r2));
    pts_int.push_back(centre+Utils::Math::Point( r2,  r2, -r2));
    pts_int.push_back(centre+Utils::Math::Point(-r2, -r2,  r2));
    pts_int.push_back(centre+Utils::Math::Point( r2, -r2,  r2));
    pts_int.push_back(centre+Utils::Math::Point(-r2,  r2,  r2));
    pts_int.push_back(centre+Utils::Math::Point( r2,  r2,  r2));


    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, 2*m_ni,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b2 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b5 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b6 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b7 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // les blocs externes pour utiliser des boucles
    std::vector<Block*> blocs;
    blocs.push_back(b2);
    blocs.push_back(b3);
    blocs.push_back(b4);
    blocs.push_back(b5);
    blocs.push_back(b6);
    blocs.push_back(b7);

    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);
    for (uint j=0; j<6; j++)
        addCreatedBlock(blocs[j]);

    b1->setGeomAssociation(getGeomEntity());
    for (uint j=0; j<6; j++)
        blocs[j]->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications


    // on place les sommets

    // le bloc au centre (b1)
    for (uint i=0; i<8; i++)
        b1->getVertex(i)->setCoord(pts_int[i]);

    for (uint j=0; j<6; j++)
        for (uint i=0; i<4; i++){
                blocs[j]->getVertex(i)->setCoord(pts_ext[TopoHelper::tab2IndVtxByFaceOnBlock[j][i]]);
                blocs[j]->getVertex(i+4)->setCoord(pts_int[TopoHelper::tab2IndVtxByFaceOnBlock[j][i]]);
            }

    // on fusionne les blocs
    for (uint j=0; j<6; j++)
        b1->getFace(j)->fuse(blocs[j]->getFace(Block::k_max), &getInfoCommand());

    b2->getFace(Block::j_min)->fuse(b4->getFace(Block::j_max), &getInfoCommand());
    b2->getFace(Block::i_min)->fuse(b7->getFace(Block::j_max), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b6->getFace(Block::j_min), &getInfoCommand());
    b4->getFace(Block::i_min)->fuse(b7->getFace(Block::i_max), &getInfoCommand());
    b4->getFace(Block::i_max)->fuse(b6->getFace(Block::i_max), &getInfoCommand());
    b3->getFace(Block::j_max)->fuse(b4->getFace(Block::j_min), &getInfoCommand());
    b3->getFace(Block::i_min)->fuse(b7->getFace(Block::j_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b6->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_min)->fuse(b7->getFace(Block::i_min), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_max)->fuse(b6->getFace(Block::i_min), &getInfoCommand());
    b2->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());

//    std::cout<<getTopoManager().getCoFace("Fa0009")->getInfoBlock()
//             <<" fuse avec "<<getTopoManager().getCoFace("Fa0026")->getInfoBlock()<<std::endl;

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 1){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 1 surface géométrique mais "
                <<surfaces.size()<<" pour la sphère pleine "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    // les projections
    for (uint j=0; j<6; j++){
        Face* face = blocs[j]->getFace(Block::k_min);
        face->setGeomAssociation(surfaces[0]);

        for (uint i=0; i<4; i++){
            face->getEdge(i)->setGeomAssociation(surfaces[0]);
            face->getVertex(i)->setGeomAssociation(surfaces[0]);
        }
    }

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopoOGridDemiDeg(Geom::PropertySphere* propertySph)
{
    Utils::Math::Point centre = propertySph->getCenter();
    double rayon = propertySph->getRadius();
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;

    double r2 = rayon*d2;
    double r3 = rayon*d3;

    // la demi-sphère avec OCC se positionne en y>=0, que l'on associe à b1 k_max

    // constitution des points sur la sphère (position de la topo)
    std::vector<Utils::Math::Point> pts_ext;
    pts_ext.push_back(centre+Utils::Math::Point(-r3,  r3, -r3));
    pts_ext.push_back(centre+Utils::Math::Point( r3,  r3, -r3));
    pts_ext.push_back(centre+Utils::Math::Point(-r3,  r3,  r3));
    pts_ext.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts_ext.push_back(centre+Utils::Math::Point(-r2,   0, -r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,   0, -r2));
    pts_ext.push_back(centre+Utils::Math::Point(-r2,   0,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    Topo::Block* b2 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b5 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b6 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // les blocs externes pour utiliser des boucles
    std::vector<Block*> blocs;
    blocs.push_back(b2);
    blocs.push_back(b3);
    blocs.push_back(b4);
    blocs.push_back(b5);
    blocs.push_back(b6);

    // stockage dans le manager topologiques des différentes entités
    for (uint j=0; j<5; j++)
        addCreatedBlock(blocs[j]);

    for (uint j=0; j<5; j++)
        blocs[j]->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications


    // on place les sommets
    for (uint j=0; j<5; j++)
        for (uint i=0; i<4; i++){
                blocs[j]->getVertex(i)->setCoord(pts_ext[TopoHelper::tab2IndVtxByFaceOnBlock[j][i]]);
                blocs[j]->getVertex(i+4)->setCoord(centre);
            }

    // dégénéréscence des blocs
    for (uint j=0; j<5; j++)
        blocs[j]->degenerateFaceInVertex(Block::k_max, &getInfoCommand());

    // on fusionne les blocs
    b2->getFace(Block::j_min)->fuse(b4->getFace(Block::j_max), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b6->getFace(Block::j_min), &getInfoCommand());
    b4->getFace(Block::i_max)->fuse(b6->getFace(Block::i_max), &getInfoCommand());
    b3->getFace(Block::j_max)->fuse(b4->getFace(Block::j_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b6->getFace(Block::j_max), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_max)->fuse(b6->getFace(Block::i_min), &getInfoCommand());
    b2->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());

    // on coupe les faces des blocs b4 et b5 pour qu'une arête soit sur la courbe 2
    std::vector<Edge* > splitingEdges;
    std::vector<CoEdge* > coedges;
    std::vector<CoFace* > cofaces;
    cofaces.push_back(b4->getFace(Block::i_min)->getCoFace(0));
    TopoHelper::splitFaces(cofaces,
            b4->getFace(Block::i_min)->getEdge(b4->getVertex(0), b4->getVertex(2))->getCoEdge(0),
            0.5, 0.0, false, false,
            splitingEdges,
            &getInfoCommand());

    coedges.push_back(splitingEdges[0]->getCoEdge(0));
    cofaces.clear();
    cofaces.push_back(b5->getFace(Block::i_min)->getCoFace(0));
    TopoHelper::splitFaces(cofaces,
            b5->getFace(Block::i_min)->getEdge(b5->getVertex(0), b5->getVertex(2))->getCoEdge(0),
            0.5, 0.0, false, false,
            splitingEdges,
            &getInfoCommand());
    coedges.push_back(splitingEdges[0]->getCoEdge(0));

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 surfaces géométriques mais "
                <<surfaces.size()<<" pour la demi sphère pleine "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // les projections
    for (uint j=0; j<5; j++){
        Face* face = blocs[j]->getFace(Block::k_min);
        face->setGeomAssociation(surfaces[0]);

        for (uint i=0; i<4; i++){
            face->getEdge(i)->setGeomAssociation(surfaces[0]);
            face->getVertex(i)->setGeomAssociation(surfaces[0]);
        }
    }

    b3->getFace(Block::i_min)->setGeomAssociation(surfaces[1]);
    b2->getFace(Block::i_min)->setGeomAssociation(surfaces[2]);

    Vertex* v0 = b4->getVertex(2);
    Vertex* v1 = b4->getVertex(0);
    Vertex* v2 = b5->getVertex(0);
    Vertex* v3 = b5->getVertex(2);

    b4->getFace(Block::i_min)->getCoFaces(cofaces);
    if (cofaces[0]->find(v0)){
        cofaces[0]->setGeomAssociation(surfaces[2]);
        cofaces[1]->setGeomAssociation(surfaces[1]);
    } else if (cofaces[0]->find(v1)){
        cofaces[0]->setGeomAssociation(surfaces[1]);
        cofaces[1]->setGeomAssociation(surfaces[2]);
    } else
        throw TkUtil::Exception (TkUtil::UTF8String ("Echec pour projeter les CoFaces issues de la coupe de b4", TkUtil::Charset::UTF_8));

    b5->getFace(Block::i_min)->getCoFaces(cofaces);
    if (cofaces[0]->find(v2)){
        cofaces[0]->setGeomAssociation(surfaces[2]);
        cofaces[1]->setGeomAssociation(surfaces[1]);
    } else if (cofaces[0]->find(v3)){
        cofaces[0]->setGeomAssociation(surfaces[1]);
        cofaces[1]->setGeomAssociation(surfaces[2]);
    } else
        throw TkUtil::Exception (TkUtil::UTF8String ("Echec pour projeter les CoFaces issues de la coupe de b5", TkUtil::Charset::UTF_8));



    // les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 2 sommets géométriques mais "
                <<vertices.size()<<" pour la demi sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    TopoHelper::getVerticesTip(coedges, v0, v1, false);

    // on met v0 en z max sur la sphère
    if (v0->getCoord().getZ() < v1->getCoord().getZ()){
        Vertex* vtmp =  v0;
        v0 = v1;
        v1 = vtmp;
    }
    v0->setCoord(vertices[0]->getCoord());
    v1->setCoord(vertices[1]->getCoord());


    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 courbes géométriques mais "
                <<curves.size()<<" pour la demi sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    b3->getFace(Block::k_min)->getEdge(b3->getVertex(0), b3->getVertex(2))->setGeomAssociation(curves[1]);
    b3->getVertex(0)->setGeomAssociation(curves[1]);
    b3->getVertex(2)->setGeomAssociation(curves[1]);

    b5->getFace(Block::k_min)->getEdge(b5->getVertex(0), b5->getVertex(2))->getCoEdge(v0, b5->getVertex(2))->setGeomAssociation(curves[1]);
    b5->getFace(Block::k_min)->getEdge(b5->getVertex(0), b5->getVertex(2))->getCoEdge(b5->getVertex(0), v0)->setGeomAssociation(curves[0]);

    b4->getFace(Block::k_min)->getEdge(b4->getVertex(0), b4->getVertex(2))->getCoEdge(b4->getVertex(0), v1)->setGeomAssociation(curves[1]);
    b4->getFace(Block::k_min)->getEdge(b4->getVertex(0), b4->getVertex(2))->getCoEdge(b4->getVertex(2), v1)->setGeomAssociation(curves[0]);

    b2->getFace(Block::k_min)->getEdge(b2->getVertex(0), b2->getVertex(2))->setGeomAssociation(curves[0]);
    b2->getVertex(0)->setGeomAssociation(curves[0]);
    b2->getVertex(2)->setGeomAssociation(curves[0]);


    b3->getFace(Block::i_min)->getEdge(b3->getVertex(0), b3->getVertex(4))->setGeomAssociation(surfaces[1]);
    b3->getFace(Block::i_min)->getEdge(b3->getVertex(2), b3->getVertex(4))->setGeomAssociation(surfaces[1]);

    b2->getFace(Block::i_min)->getEdge(b2->getVertex(0), b2->getVertex(4))->setGeomAssociation(surfaces[2]);
    b2->getFace(Block::i_min)->getEdge(b2->getVertex(2), b2->getVertex(4))->setGeomAssociation(surfaces[2]);

    // sur l'axe des Z
    for (uint i=0; i<splitingEdges.size(); i++){
        splitingEdges[i]->setGeomAssociation(curves[2]);
        for (uint j=0; j<splitingEdges[i]->getNbVertices(); j++)
            splitingEdges[i]->getVertex(j)->setGeomAssociation(curves[2]);
    }
    v0->setGeomAssociation(vertices[0]);
    v1->setGeomAssociation(vertices[1]);

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopoOGridDemiNonDeg(Geom::PropertySphere* propertySph)
{
    Utils::Math::Point centre = propertySph->getCenter();
    double rayon = propertySph->getRadius();
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;

    double r2 = rayon*d2;
    double r3 = rayon*d3;

    // la demi-sphère avec OCC se positionne en y>=0, que l'on associe à b1 k_max

    // constitution des points sur la sphère (position de la topo)
    std::vector<Utils::Math::Point> pts_ext;
    pts_ext.push_back(centre+Utils::Math::Point(-r3,  r3, -r3));
    pts_ext.push_back(centre+Utils::Math::Point( r3,  r3, -r3));
    pts_ext.push_back(centre+Utils::Math::Point(-r3,  r3,  r3));
    pts_ext.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts_ext.push_back(centre+Utils::Math::Point(-r2,   0, -r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,   0, -r2));
    pts_ext.push_back(centre+Utils::Math::Point(-r2,   0,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    // constitution de ceux au sommet du bloc central de la topologie
    std::vector<Utils::Math::Point> pts_int;
    r2 = rayon*d2*m_ratio;
    r3 = rayon*d3*m_ratio;
    pts_int.push_back(centre+Utils::Math::Point(-r3,  r3, -r3));
    pts_int.push_back(centre+Utils::Math::Point( r3,  r3, -r3));
    pts_int.push_back(centre+Utils::Math::Point(-r3,  r3,  r3));
    pts_int.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts_int.push_back(centre+Utils::Math::Point(-r2,   0, -r2));
    pts_int.push_back(centre+Utils::Math::Point( r2,   0, -r2));
    pts_int.push_back(centre+Utils::Math::Point(-r2,   0,  r2));
    pts_int.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_ni,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b2 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b5 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b6 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // les blocs externes pour utiliser des boucles
    std::vector<Block*> blocs;
    blocs.push_back(b2);
    blocs.push_back(b3);
    blocs.push_back(b4);
    blocs.push_back(b5);
    blocs.push_back(b6);

    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);
    for (uint j=0; j<5; j++)
        addCreatedBlock(blocs[j]);

    b1->setGeomAssociation(getGeomEntity());
    for (uint j=0; j<5; j++)
        blocs[j]->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications


    // on place les sommets

    // le bloc au centre (b1)
    for (uint i=0; i<8; i++)
        b1->getVertex(i)->setCoord(pts_int[i]);

    for (uint j=0; j<5; j++)
        for (uint i=0; i<4; i++){
                blocs[j]->getVertex(i)->setCoord(pts_ext[TopoHelper::tab2IndVtxByFaceOnBlock[j][i]]);
                blocs[j]->getVertex(i+4)->setCoord(pts_int[TopoHelper::tab2IndVtxByFaceOnBlock[j][i]]);
            }

    // on fusionne les blocs
    for (uint j=0; j<5; j++)
        b1->getFace(j)->fuse(blocs[j]->getFace(Block::k_max), &getInfoCommand());

    b2->getFace(Block::j_min)->fuse(b4->getFace(Block::j_max), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b6->getFace(Block::j_min), &getInfoCommand());
    b4->getFace(Block::i_max)->fuse(b6->getFace(Block::i_max), &getInfoCommand());
    b3->getFace(Block::j_max)->fuse(b4->getFace(Block::j_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b6->getFace(Block::j_max), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_max)->fuse(b6->getFace(Block::i_min), &getInfoCommand());
    b2->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());

    // on coupe la face du bloc b1 pour qu'une arête soit sur la courbe 2
    // et les faces des blocs b4 et b5
    std::vector<Edge* > splitingEdges;
    std::vector<CoFace* > cofaces;
    cofaces.push_back(b1->getFace(Block::k_max)->getCoFace(0));
    cofaces.push_back(b4->getFace(Block::i_min)->getCoFace(0));
    cofaces.push_back(b5->getFace(Block::i_min)->getCoFace(0));
    TopoHelper::splitFaces(cofaces,
            b4->getFace(Block::i_min)->getEdge(b4->getVertex(0), b4->getVertex(2))->getCoEdge(0),
            0.5, 0.0, false, false,
            splitingEdges,
            &getInfoCommand());


    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 surfaces géométriques mais "
                <<surfaces.size()<<" pour la demi sphère pleine "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    // les projections
    for (uint j=0; j<5; j++){
        Face* face = blocs[j]->getFace(Block::k_min);
        face->setGeomAssociation(surfaces[0]);

        for (uint i=0; i<4; i++){
            face->getEdge(i)->setGeomAssociation(surfaces[0]);
            face->getVertex(i)->setGeomAssociation(surfaces[0]);
        }
    }

    b3->getFace(Block::i_min)->setGeomAssociation(surfaces[1]);
    b2->getFace(Block::i_min)->setGeomAssociation(surfaces[2]);

    Vertex* v4 = b1->getVertex(4);
    Vertex* v5 = b1->getVertex(5);
    Vertex* v6 = b1->getVertex(6);
    Vertex* v7 = b1->getVertex(7);

    b1->getFace(Block::k_max)->getCoFaces(cofaces);
    if (cofaces[0]->find(v4) || cofaces[0]->find(v6)){
        cofaces[0]->setGeomAssociation(surfaces[2]);
        cofaces[1]->setGeomAssociation(surfaces[1]);
    } else if (cofaces[0]->find(v5) || cofaces[0]->find(v7)){
        cofaces[0]->setGeomAssociation(surfaces[1]);
        cofaces[1]->setGeomAssociation(surfaces[2]);
    } else
        throw TkUtil::Exception (TkUtil::UTF8String ("Echec pour projeter les CoFaces issues de la coupe de b1", TkUtil::Charset::UTF_8));

    b4->getFace(Block::i_min)->getCoFaces(cofaces);
    if (cofaces[0]->find(v4) || cofaces[0]->find(v6)){
        cofaces[0]->setGeomAssociation(surfaces[2]);
        cofaces[1]->setGeomAssociation(surfaces[1]);
    } else if (cofaces[0]->find(v5) || cofaces[0]->find(v7)){
        cofaces[0]->setGeomAssociation(surfaces[1]);
        cofaces[1]->setGeomAssociation(surfaces[2]);
    } else
        throw TkUtil::Exception (TkUtil::UTF8String ("Echec pour projeter les CoFaces issues de la coupe de b4", TkUtil::Charset::UTF_8));

    b5->getFace(Block::i_min)->getCoFaces(cofaces);
    if (cofaces[0]->find(v4) || cofaces[0]->find(v6)){
        cofaces[0]->setGeomAssociation(surfaces[2]);
        cofaces[1]->setGeomAssociation(surfaces[1]);
    } else if (cofaces[0]->find(v5) || cofaces[0]->find(v7)){
        cofaces[0]->setGeomAssociation(surfaces[1]);
        cofaces[1]->setGeomAssociation(surfaces[2]);
    } else
        throw TkUtil::Exception (TkUtil::UTF8String ("Echec pour projeter les CoFaces issues de la coupe de b5", TkUtil::Charset::UTF_8));

    // les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 2 sommets géométriques mais "
                <<vertices.size()<<" pour la sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Vertex* v0 = 0;
    Vertex* v1 = 0;
    std::vector<CoEdge* > coedges;
    for (uint i=0; i<splitingEdges.size(); i++)
        coedges.push_back(splitingEdges[i]->getCoEdge(0));
    TopoHelper::getVerticesTip(coedges, v0, v1, false);

    // on met v0 en z max sur la sphère
    if (v0->getCoord().getZ() < v1->getCoord().getZ()){
        Vertex* vtmp =  v0;
        v0 = v1;
        v1 = vtmp;
    }
    v0->setCoord(vertices[0]->getCoord());
    v1->setCoord(vertices[1]->getCoord());

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 courbes géométriques mais "
                <<curves.size()<<" pour la demi sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    b3->getFace(Block::k_min)->getEdge(b3->getVertex(0), b3->getVertex(2))->setGeomAssociation(curves[1]);
    b3->getVertex(0)->setGeomAssociation(curves[1]);
    b3->getVertex(2)->setGeomAssociation(curves[1]);

    b5->getFace(Block::k_min)->getEdge(b5->getVertex(0), b5->getVertex(2))->getCoEdge(v0, b5->getVertex(2))->setGeomAssociation(curves[1]);
    b5->getFace(Block::k_min)->getEdge(b5->getVertex(0), b5->getVertex(2))->getCoEdge(b5->getVertex(0), v0)->setGeomAssociation(curves[0]);

    b4->getFace(Block::k_min)->getEdge(b4->getVertex(0), b4->getVertex(2))->getCoEdge(b4->getVertex(0), v1)->setGeomAssociation(curves[1]);
    b4->getFace(Block::k_min)->getEdge(b4->getVertex(0), b4->getVertex(2))->getCoEdge(b4->getVertex(2), v1)->setGeomAssociation(curves[0]);

    b2->getFace(Block::k_min)->getEdge(b2->getVertex(0), b2->getVertex(2))->setGeomAssociation(curves[0]);
    b2->getVertex(0)->setGeomAssociation(curves[0]);
    b2->getVertex(2)->setGeomAssociation(curves[0]);


    b3->getFace(Block::i_min)->getEdge(b3->getVertex(0), b3->getVertex(4))->setGeomAssociation(surfaces[1]);
    b3->getFace(Block::i_min)->getEdge(b3->getVertex(2), b3->getVertex(6))->setGeomAssociation(surfaces[1]);

    b2->getFace(Block::i_min)->getEdge(b2->getVertex(0), b2->getVertex(4))->setGeomAssociation(surfaces[2]);
    b2->getFace(Block::i_min)->getEdge(b2->getVertex(2), b2->getVertex(6))->setGeomAssociation(surfaces[2]);

    b1->getVertex(5)->setGeomAssociation(surfaces[1]);
    b1->getVertex(7)->setGeomAssociation(surfaces[1]);
    b1->getVertex(4)->setGeomAssociation(surfaces[2]);
    b1->getVertex(6)->setGeomAssociation(surfaces[2]);

    for (uint i=0; i<b1->getFace(Block::k_max)->getNbCoFaces(); i++){
        CoFace* coface = b1->getFace(Block::k_max)->getCoFace(i);
        for (uint j=0; j<coface->getNbEdges(); j++)
            coface->getEdge(j)->setGeomAssociation(coface->getGeomAssociation());
    }

    // sur l'axe des Z
    for (uint i=0; i<splitingEdges.size(); i++){
        splitingEdges[i]->setGeomAssociation(curves[2]);
        for (uint j=0; j<splitingEdges[i]->getNbVertices(); j++)
            splitingEdges[i]->getVertex(j)->setGeomAssociation(curves[2]);
    }
    v0->setGeomAssociation(vertices[0]);
    v1->setGeomAssociation(vertices[1]);

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopoOGridQuartDeg(Geom::PropertySphere* propertySph)
{
    Utils::Math::Point centre = propertySph->getCenter();
    double rayon = propertySph->getRadius();
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;

    double r2 = rayon*d2;
    double r3 = rayon*d3;

    // constitution des points sur la sphère (position de la topo)
    std::vector<Utils::Math::Point> pts_ext;
    pts_ext.push_back(centre+Utils::Math::Point(  0,  r2, -r2));
    pts_ext.push_back(centre+Utils::Math::Point( r3,  r3, -r3));
    pts_ext.push_back(centre+Utils::Math::Point(  0,  r2,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts_ext.push_back(centre+Utils::Math::Point(  0,   0, -r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,   0, -r2));
    pts_ext.push_back(centre+Utils::Math::Point(  0,   0,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    Topo::Block* b3 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b5 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b6 = new Topo::Block(getContext(), 2*m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // les blocs externes pour utiliser des boucles
    std::vector<Block*> blocs;
    blocs.push_back(b3);
    blocs.push_back(b4);
    blocs.push_back(b5);
    blocs.push_back(b6);

    // stockage dans le manager topologiques des différentes entités
    for (uint j=0; j<4; j++)
        addCreatedBlock(blocs[j]);

    for (uint j=0; j<4; j++)
        blocs[j]->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // on place les sommets
    for (uint j=0; j<4; j++)
        for (uint i=0; i<4; i++){
                blocs[j]->getVertex(i)->setCoord(pts_ext[TopoHelper::tab2IndVtxByFaceOnBlock[j+1][i]]);
                blocs[j]->getVertex(i+4)->setCoord(centre);
            }

    // dégénéréscence des blocs
    for (uint j=0; j<4; j++)
        blocs[j]->degenerateFaceInVertex(Block::k_max, &getInfoCommand());

    // on fusionne les blocs
    b4->getFace(Block::i_max)->fuse(b6->getFace(Block::i_max), &getInfoCommand());
    b3->getFace(Block::j_max)->fuse(b4->getFace(Block::j_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b6->getFace(Block::j_max), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_max)->fuse(b6->getFace(Block::i_min), &getInfoCommand());

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 surfaces géométriques mais "
                <<surfaces.size()<<" pour le quart de sphère pleine "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // les projections
    for (uint j=0; j<4; j++){
        Face* face = blocs[j]->getFace(Block::k_min);
        face->setGeomAssociation(surfaces[0]);

        for (uint i=0; i<4; i++){
            face->getEdge(i)->setGeomAssociation(surfaces[0]);
            face->getVertex(i)->setGeomAssociation(surfaces[0]);
        }
    }

    b3->getFace(Block::i_min)->setGeomAssociation(surfaces[1]);
    b4->getFace(Block::i_min)->setGeomAssociation(surfaces[1]);
    b5->getFace(Block::i_min)->setGeomAssociation(surfaces[1]);

    b4->getFace(Block::j_max)->setGeomAssociation(surfaces[2]);
    b5->getFace(Block::j_min)->setGeomAssociation(surfaces[2]);
    b6->getFace(Block::j_min)->setGeomAssociation(surfaces[2]);


    // les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 2 sommets géométriques mais "
                <<vertices.size()<<" pour le quart de sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Vertex* v0 = b5->getVertex(0);
    Vertex* v1 = b4->getVertex(2);
    v0->setGeomAssociation(vertices[0]);
    v1->setGeomAssociation(vertices[1]);
    v0->setCoord(vertices[0]->getCoord());
    v1->setCoord(vertices[1]->getCoord());

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 courbes géométriques mais "
                <<curves.size()<<" pour le quart de sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    b3->getFace(Block::k_min)->getEdge(b3->getVertex(0), b3->getVertex(2))->setGeomAssociation(curves[1]);

    b5->getFace(Block::k_min)->getEdge(b5->getVertex(0), b5->getVertex(2))->setGeomAssociation(curves[1]);
    b5->getFace(Block::k_min)->getEdge(b5->getVertex(0), b5->getVertex(1))->setGeomAssociation(curves[0]);

    b6->getFace(Block::k_min)->getEdge(b6->getVertex(0), b6->getVertex(1))->setGeomAssociation(curves[0]);

    b4->getFace(Block::k_min)->getEdge(b4->getVertex(0), b4->getVertex(2))->setGeomAssociation(curves[1]);
    b4->getFace(Block::k_min)->getEdge(b4->getVertex(2), b4->getVertex(3))->setGeomAssociation(curves[0]);

    b5->getVertex(2)->setGeomAssociation(curves[1]);
    b5->getVertex(1)->setGeomAssociation(curves[0]);
    b4->getVertex(0)->setGeomAssociation(curves[1]);
    b4->getVertex(3)->setGeomAssociation(curves[0]);


    b3->getFace(Block::i_min)->getEdge(b3->getVertex(0), b3->getVertex(4))->setGeomAssociation(surfaces[1]);
    b3->getFace(Block::i_min)->getEdge(b3->getVertex(2), b3->getVertex(4))->setGeomAssociation(surfaces[1]);

    b6->getFace(Block::j_min)->getEdge(b6->getVertex(0), b6->getVertex(4))->setGeomAssociation(surfaces[2]);
    b6->getFace(Block::j_min)->getEdge(b6->getVertex(1), b6->getVertex(4))->setGeomAssociation(surfaces[2]);

    // sur l'axe des Z
    b4->getFace(Block::j_max)->getEdge(b4->getVertex(2), b4->getVertex(4))->setGeomAssociation(curves[2]);
    b5->getFace(Block::j_min)->getEdge(b5->getVertex(0), b5->getVertex(4))->setGeomAssociation(curves[2]);
    b5->getVertex(4)->setGeomAssociation(curves[2]);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopoOGridQuartNonDeg(Geom::PropertySphere* propertySph)
{
    Utils::Math::Point centre = propertySph->getCenter();
    double rayon = propertySph->getRadius();
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;

    double r2 = rayon*d2;
    double r3 = rayon*d3;

    // constitution des points sur la sphère (position de la topo)
    std::vector<Utils::Math::Point> pts_ext;
    pts_ext.push_back(centre+Utils::Math::Point(  0,  r2, -r2));
    pts_ext.push_back(centre+Utils::Math::Point( r3,  r3, -r3));
    pts_ext.push_back(centre+Utils::Math::Point(  0,  r2,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts_ext.push_back(centre+Utils::Math::Point(  0,   0, -r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,   0, -r2));
    pts_ext.push_back(centre+Utils::Math::Point(  0,   0,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    // constitution de ceux au sommet du bloc central de la topologie
    std::vector<Utils::Math::Point> pts_int;
    r2 = rayon*d2*m_ratio;
    r3 = rayon*d3*m_ratio;
    pts_int.push_back(centre+Utils::Math::Point(  0,  r2, -r2));
    pts_int.push_back(centre+Utils::Math::Point( r3,  r3, -r3));
    pts_int.push_back(centre+Utils::Math::Point(  0,  r2,  r2));
    pts_int.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts_int.push_back(centre+Utils::Math::Point(  0,   0, -r2));
    pts_int.push_back(centre+Utils::Math::Point( r2,   0, -r2));
    pts_int.push_back(centre+Utils::Math::Point(  0,   0,  r2));
    pts_int.push_back(centre+Utils::Math::Point( r2,   0,  r2));


    Topo::Block* b1 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_ni,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b5 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b6 = new Topo::Block(getContext(), 2*m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // les blocs externes pour utiliser des boucles
    std::vector<Block*> blocs;
    blocs.push_back(b3);
    blocs.push_back(b4);
    blocs.push_back(b5);
    blocs.push_back(b6);


    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);
    for (uint j=0; j<4; j++)
        addCreatedBlock(blocs[j]);

    b1->setGeomAssociation(getGeomEntity());
    for (uint j=0; j<4; j++)
        blocs[j]->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // le bloc au centre (b1)
    for (uint i=0; i<8; i++)
        b1->getVertex(i)->setCoord(pts_int[i]);

    for (uint j=0; j<4; j++)
        for (uint i=0; i<4; i++){
            blocs[j]->getVertex(i)->setCoord(pts_ext[TopoHelper::tab2IndVtxByFaceOnBlock[j+1][i]]);
            blocs[j]->getVertex(i+4)->setCoord(pts_int[TopoHelper::tab2IndVtxByFaceOnBlock[j+1][i]]);
        }

    // on fusionne les blocs
    for (uint j=0; j<4; j++)
        b1->getFace(j+1)->fuse(blocs[j]->getFace(Block::k_max), &getInfoCommand());

    b4->getFace(Block::i_max)->fuse(b6->getFace(Block::i_max), &getInfoCommand());
    b3->getFace(Block::j_max)->fuse(b4->getFace(Block::j_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b6->getFace(Block::j_max), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_max)->fuse(b6->getFace(Block::i_min), &getInfoCommand());

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 surfaces géométriques mais "
                <<surfaces.size()<<" pour le quart de sphère pleine "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // les projections
    for (uint j=0; j<4; j++){
        Face* face = blocs[j]->getFace(Block::k_min);
        face->setGeomAssociation(surfaces[0]);

        for (uint i=0; i<4; i++){
            face->getEdge(i)->setGeomAssociation(surfaces[0]);
            face->getVertex(i)->setGeomAssociation(surfaces[0]);
        }
    }

    b1->getFace(Block::k_max)->setGeomAssociation(surfaces[1]);
    b3->getFace(Block::i_min)->setGeomAssociation(surfaces[1]);
    b4->getFace(Block::i_min)->setGeomAssociation(surfaces[1]);
    b5->getFace(Block::i_min)->setGeomAssociation(surfaces[1]);

    b1->getFace(Block::i_min)->setGeomAssociation(surfaces[2]);
    b4->getFace(Block::j_max)->setGeomAssociation(surfaces[2]);
    b5->getFace(Block::j_min)->setGeomAssociation(surfaces[2]);
    b6->getFace(Block::j_min)->setGeomAssociation(surfaces[2]);


    // les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 2 sommets géométriques mais "
                <<vertices.size()<<" pour le quart de sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Vertex* v0 = b5->getVertex(0);
    Vertex* v1 = b4->getVertex(2);
    v0->setGeomAssociation(vertices[0]);
    v1->setGeomAssociation(vertices[1]);
    v0->setCoord(vertices[0]->getCoord());
    v1->setCoord(vertices[1]->getCoord());

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 courbes géométriques mais "
                <<curves.size()<<" pour le quart de sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    b3->getFace(Block::k_min)->getEdge(b3->getVertex(0), b3->getVertex(2))->setGeomAssociation(curves[1]);

    b5->getFace(Block::k_min)->getEdge(b5->getVertex(0), b5->getVertex(2))->setGeomAssociation(curves[1]);
    b5->getFace(Block::k_min)->getEdge(b5->getVertex(0), b5->getVertex(1))->setGeomAssociation(curves[0]);

    b6->getFace(Block::k_min)->getEdge(b6->getVertex(0), b6->getVertex(1))->setGeomAssociation(curves[0]);

    b4->getFace(Block::k_min)->getEdge(b4->getVertex(0), b4->getVertex(2))->setGeomAssociation(curves[1]);
    b4->getFace(Block::k_min)->getEdge(b4->getVertex(2), b4->getVertex(3))->setGeomAssociation(curves[0]);

    b5->getVertex(2)->setGeomAssociation(curves[1]);
    b5->getVertex(1)->setGeomAssociation(curves[0]);
    b4->getVertex(0)->setGeomAssociation(curves[1]);
    b4->getVertex(3)->setGeomAssociation(curves[0]);


    b1->getFace(Block::i_min)->getEdge(b1->getVertex(0), b1->getVertex(2))->setGeomAssociation(surfaces[2]);
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(6), b1->getVertex(2))->setGeomAssociation(surfaces[2]);
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(0), b1->getVertex(4))->setGeomAssociation(surfaces[2]);

    b1->getVertex(0)->setGeomAssociation(surfaces[2]);
    b1->getVertex(2)->setGeomAssociation(surfaces[2]);

    b1->getFace(Block::k_max)->getEdge(b1->getVertex(4), b1->getVertex(5))->setGeomAssociation(surfaces[1]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(5), b1->getVertex(7))->setGeomAssociation(surfaces[1]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(7), b1->getVertex(6))->setGeomAssociation(surfaces[1]);

    b1->getVertex(5)->setGeomAssociation(surfaces[1]);
    b1->getVertex(7)->setGeomAssociation(surfaces[1]);

    b3->getFace(Block::i_min)->getEdge(b3->getVertex(0), b3->getVertex(4))->setGeomAssociation(surfaces[1]);
    b3->getFace(Block::i_min)->getEdge(b3->getVertex(2), b3->getVertex(6))->setGeomAssociation(surfaces[1]);

    b6->getFace(Block::j_min)->getEdge(b6->getVertex(0), b6->getVertex(4))->setGeomAssociation(surfaces[2]);
    b6->getFace(Block::j_min)->getEdge(b6->getVertex(1), b6->getVertex(5))->setGeomAssociation(surfaces[2]);

    // sur l'axe des Z
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(4), b1->getVertex(6))->setGeomAssociation(curves[2]);
    b4->getFace(Block::j_max)->getEdge(b4->getVertex(2), b4->getVertex(6))->setGeomAssociation(curves[2]);
    b5->getFace(Block::j_min)->getEdge(b5->getVertex(0), b5->getVertex(4))->setGeomAssociation(curves[2]);
    b1->getVertex(4)->setGeomAssociation(curves[2]);
    b1->getVertex(6)->setGeomAssociation(curves[2]);

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopoOGridHuitiemeDeg(Geom::PropertySphere* propertySph)
{
    Utils::Math::Point centre = propertySph->getCenter();
    double rayon = propertySph->getRadius();
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;

    double r2 = rayon*d2;
    double r3 = rayon*d3;

    // constitution des points sur la sphère (position de la topo)
    std::vector<Utils::Math::Point> pts_ext;
    pts_ext.push_back(centre+Utils::Math::Point(  0,  r2,   0));
    pts_ext.push_back(centre+Utils::Math::Point( r2,  r2,   0));
    pts_ext.push_back(centre+Utils::Math::Point(  0,  r2,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts_ext.push_back(centre+Utils::Math::Point(  0,   0,   0));
    pts_ext.push_back(centre+Utils::Math::Point( r2,   0,   0));
    pts_ext.push_back(centre+Utils::Math::Point(  0,   0,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    Topo::Block* b3 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b5 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b6 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // les blocs externes pour utiliser des boucles
    std::vector<Block*> blocs;
    blocs.push_back(b3);
    blocs.push_back(b5);
    blocs.push_back(b6);

    // stockage dans le manager topologiques des différentes entités
    for (uint j=0; j<3; j++)
        addCreatedBlock(blocs[j]);

    for (uint j=0; j<3; j++)
        blocs[j]->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // on place les sommets
    for (uint j=0; j<3; j++)
        for (uint i=0; i<4; i++){
            uint k = 0;
            if (j==0)
                k=1;
            else if (j==1)
                k=3;
            else if (j==2)
                k=4;
            blocs[j]->getVertex(i)->setCoord(pts_ext[TopoHelper::tab2IndVtxByFaceOnBlock[k][i]]);
            blocs[j]->getVertex(i+4)->setCoord(centre);
        }

    // dégénéréscence des blocs
    for (uint j=0; j<3; j++)
        blocs[j]->degenerateFaceInVertex(Block::k_max, &getInfoCommand());

    // on fusionne les blocs
    b3->getFace(Block::i_max)->fuse(b6->getFace(Block::j_max), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_max)->fuse(b6->getFace(Block::i_min), &getInfoCommand());

    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 surfaces géométriques mais "
                <<surfaces.size()<<" pour le huitième de sphère pleine "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // les projections
    for (uint j=0; j<3; j++){
        Face* face = blocs[j]->getFace(Block::k_min);
        face->setGeomAssociation(surfaces[0]);

        for (uint i=0; i<4; i++){
            face->getEdge(i)->setGeomAssociation(surfaces[0]);
            face->getVertex(i)->setGeomAssociation(surfaces[0]);
        }
    }

    b3->getFace(Block::j_max)->setGeomAssociation(surfaces[1]);
    b6->getFace(Block::i_max)->setGeomAssociation(surfaces[1]);

    b3->getFace(Block::i_min)->setGeomAssociation(surfaces[2]);
    b5->getFace(Block::i_min)->setGeomAssociation(surfaces[2]);

    b5->getFace(Block::j_min)->setGeomAssociation(surfaces[3]);
    b6->getFace(Block::j_min)->setGeomAssociation(surfaces[3]);


    // les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 sommets géométriques mais "
                <<vertices.size()<<" pour le huitième de sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Vertex* v0 = b5->getVertex(0);
    Vertex* v1 = b6->getVertex(1);
    Vertex* v2 = b3->getVertex(2);
    Vertex* v3 = b3->getVertex(4);

    v0->setGeomAssociation(vertices[0]);
    v1->setGeomAssociation(vertices[1]);
    v2->setGeomAssociation(vertices[2]);
    v3->setGeomAssociation(vertices[3]);
    v0->setCoord(vertices[0]->getCoord());
    v1->setCoord(vertices[1]->getCoord());
    v2->setCoord(vertices[2]->getCoord());
    v3->setCoord(vertices[3]->getCoord());

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 courbes géométriques mais "
                <<curves.size()<<" pour le huitième de sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    b3->getFace(Block::k_min)->getEdge(b3->getVertex(2), b3->getVertex(3))->setGeomAssociation(curves[1]);
    b3->getFace(Block::k_min)->getEdge(b3->getVertex(0), b3->getVertex(2))->setGeomAssociation(curves[2]);

    b5->getFace(Block::k_min)->getEdge(b5->getVertex(0), b5->getVertex(2))->setGeomAssociation(curves[2]);
    b5->getFace(Block::k_min)->getEdge(b5->getVertex(0), b5->getVertex(1))->setGeomAssociation(curves[0]);

    b6->getFace(Block::k_min)->getEdge(b6->getVertex(1), b6->getVertex(3))->setGeomAssociation(curves[1]);
    b6->getFace(Block::k_min)->getEdge(b6->getVertex(0), b6->getVertex(1))->setGeomAssociation(curves[0]);

    b3->getVertex(3)->setGeomAssociation(curves[1]);
    b5->getVertex(2)->setGeomAssociation(curves[2]);
    b6->getVertex(0)->setGeomAssociation(curves[0]);

    b3->getFace(Block::j_max)->getEdge(b3->getVertex(4), b3->getVertex(3))->setGeomAssociation(surfaces[1]);
    b3->getFace(Block::i_min)->getEdge(b3->getVertex(0), b3->getVertex(4))->setGeomAssociation(surfaces[2]);
    b5->getFace(Block::j_min)->getEdge(b5->getVertex(1), b5->getVertex(4))->setGeomAssociation(surfaces[3]);

    b3->getFace(Block::j_max)->getEdge(b3->getVertex(2), b3->getVertex(4))->setGeomAssociation(curves[4]);
    b5->getFace(Block::j_min)->getEdge(b5->getVertex(0), b5->getVertex(4))->setGeomAssociation(curves[5]);
    b6->getFace(Block::i_max)->getEdge(b6->getVertex(1), b6->getVertex(4))->setGeomAssociation(curves[3]);

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createSphereTopoOGridHuitiemeNonDeg(Geom::PropertySphere* propertySph)
{
    Utils::Math::Point centre = propertySph->getCenter();
    double rayon = propertySph->getRadius();
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;

    double r2 = rayon*d2;
    double r3 = rayon*d3;

    // constitution des points sur la sphère (position de la topo)
    std::vector<Utils::Math::Point> pts_ext;
    pts_ext.push_back(centre+Utils::Math::Point(  0,  r2,   0));
    pts_ext.push_back(centre+Utils::Math::Point( r2,  r2,   0));
    pts_ext.push_back(centre+Utils::Math::Point(  0,  r2,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts_ext.push_back(centre+Utils::Math::Point(  0,   0,   0));
    pts_ext.push_back(centre+Utils::Math::Point( r2,   0,   0));
    pts_ext.push_back(centre+Utils::Math::Point(  0,   0,  r2));
    pts_ext.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    // constitution de ceux au sommet du bloc central de la topologie
    std::vector<Utils::Math::Point> pts_int;
    r2 = rayon*d2*m_ratio;
    r3 = rayon*d3*m_ratio;
    pts_int.push_back(centre+Utils::Math::Point(  0,  r2,   0));
    pts_int.push_back(centre+Utils::Math::Point( r2,  r2,   0));
    pts_int.push_back(centre+Utils::Math::Point(  0,  r2,  r2));
    pts_int.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts_int.push_back(centre+Utils::Math::Point(  0,   0,   0));
    pts_int.push_back(centre+Utils::Math::Point( r2,   0,   0));
    pts_int.push_back(centre+Utils::Math::Point(  0,   0,  r2));
    pts_int.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_ni, m_ni,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b5 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b6 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);


    // les blocs externes pour utiliser des boucles
    std::vector<Block*> blocs;
    blocs.push_back(b3);
    blocs.push_back(b5);
    blocs.push_back(b6);

    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);
    for (uint j=0; j<3; j++)
        addCreatedBlock(blocs[j]);

    b1->setGeomAssociation(getGeomEntity());
    for (uint j=0; j<3; j++)
        blocs[j]->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications


    // le bloc au centre (b1)
    for (uint i=0; i<8; i++)
        b1->getVertex(i)->setCoord(pts_int[i]);

    for (uint j=0; j<3; j++)
        for (uint i=0; i<4; i++){
            uint k = 0;
            if (j==0)
                k=1;
            else if (j==1)
                k=3;
            else if (j==2)
                k=4;
            blocs[j]->getVertex(i)->setCoord(pts_ext[TopoHelper::tab2IndVtxByFaceOnBlock[k][i]]);
            blocs[j]->getVertex(i+4)->setCoord(pts_int[TopoHelper::tab2IndVtxByFaceOnBlock[k][i]]);
        }


    // on fusionne les blocs
    for (uint j=0; j<3; j++){
        uint k = 0;
        if (j==0)
            k=1;
        else if (j==1)
            k=3;
        else if (j==2)
            k=4;
        b1->getFace(k)->fuse(blocs[j]->getFace(Block::k_max), &getInfoCommand());
    }

    b3->getFace(Block::i_max)->fuse(b6->getFace(Block::j_max), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_max)->fuse(b6->getFace(Block::i_min), &getInfoCommand());


    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 surfaces géométriques mais "
                <<surfaces.size()<<" pour le huitième de sphère pleine "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // les projections
    for (uint j=0; j<3; j++){
        Face* face = blocs[j]->getFace(Block::k_min);
        face->setGeomAssociation(surfaces[0]);

        for (uint i=0; i<4; i++){
            face->getEdge(i)->setGeomAssociation(surfaces[0]);
            face->getVertex(i)->setGeomAssociation(surfaces[0]);
        }
    }

    b1->getFace(Block::j_min)->setGeomAssociation(surfaces[1]);
    b3->getFace(Block::j_max)->setGeomAssociation(surfaces[1]);
    b6->getFace(Block::i_max)->setGeomAssociation(surfaces[1]);

    b1->getFace(Block::k_max)->setGeomAssociation(surfaces[2]);
    b3->getFace(Block::i_min)->setGeomAssociation(surfaces[2]);
    b5->getFace(Block::i_min)->setGeomAssociation(surfaces[2]);

    b1->getFace(Block::i_min)->setGeomAssociation(surfaces[3]);
    b5->getFace(Block::j_min)->setGeomAssociation(surfaces[3]);
    b6->getFace(Block::j_min)->setGeomAssociation(surfaces[3]);


    // les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 sommets géométriques mais "
                <<vertices.size()<<" pour le huitième de sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Vertex* v0 = b5->getVertex(0);
    Vertex* v1 = b6->getVertex(1);
    Vertex* v2 = b3->getVertex(2);
    Vertex* v3 = b1->getVertex(4);

    v0->setGeomAssociation(vertices[0]);
    v1->setGeomAssociation(vertices[1]);
    v2->setGeomAssociation(vertices[2]);
    v3->setGeomAssociation(vertices[3]);
    v0->setCoord(vertices[0]->getCoord());
    v1->setCoord(vertices[1]->getCoord());
    v2->setCoord(vertices[2]->getCoord());
    v3->setCoord(vertices[3]->getCoord());


    //std::cout<<getTopoManager().getCoFace("Fa0000")->getInfoBlock()<<std::endl;

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 courbes géométriques mais "
                <<curves.size()<<" pour le huitième de sphère "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

//    for (uint i=0; i<curves.size(); i++)
//        std::cout<<"curves["<<i<<"] = "<<(curves[i]?curves[i]->getName():0)<<std::endl;

    b3->getFace(Block::k_min)->getEdge(b3->getVertex(2), b3->getVertex(3))->setGeomAssociation(curves[1]);
    b3->getFace(Block::k_min)->getEdge(b3->getVertex(0), b3->getVertex(2))->setGeomAssociation(curves[2]);

    b5->getFace(Block::k_min)->getEdge(b5->getVertex(0), b5->getVertex(2))->setGeomAssociation(curves[2]);
    b5->getFace(Block::k_min)->getEdge(b5->getVertex(0), b5->getVertex(1))->setGeomAssociation(curves[0]);

    b6->getFace(Block::k_min)->getEdge(b6->getVertex(1), b6->getVertex(3))->setGeomAssociation(curves[1]);
    b6->getFace(Block::k_min)->getEdge(b6->getVertex(0), b6->getVertex(1))->setGeomAssociation(curves[0]);

    b3->getVertex(3)->setGeomAssociation(curves[1]);
    b5->getVertex(2)->setGeomAssociation(curves[2]);
    b6->getVertex(0)->setGeomAssociation(curves[0]);

    b1->getFace(Block::i_min)->getEdge(b1->getVertex(0), b1->getVertex(2))->setGeomAssociation(surfaces[3]);
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(2), b1->getVertex(6))->setGeomAssociation(surfaces[3]);

    b1->getFace(Block::j_min)->getEdge(b1->getVertex(0), b1->getVertex(1))->setGeomAssociation(surfaces[1]);
    b1->getFace(Block::j_min)->getEdge(b1->getVertex(1), b1->getVertex(5))->setGeomAssociation(surfaces[1]);

    b1->getFace(Block::k_max)->getEdge(b1->getVertex(5), b1->getVertex(7))->setGeomAssociation(surfaces[2]);
    b1->getFace(Block::k_max)->getEdge(b1->getVertex(6), b1->getVertex(7))->setGeomAssociation(surfaces[2]);

    b3->getFace(Block::j_max)->getEdge(b3->getVertex(7), b3->getVertex(3))->setGeomAssociation(surfaces[1]);
    b3->getFace(Block::i_min)->getEdge(b3->getVertex(0), b3->getVertex(4))->setGeomAssociation(surfaces[2]);
    b5->getFace(Block::j_min)->getEdge(b5->getVertex(1), b5->getVertex(5))->setGeomAssociation(surfaces[3]);

    b1->getFace(Block::i_min)->getEdge(b1->getVertex(4), b1->getVertex(0))->setGeomAssociation(curves[3]);
    b1->getFace(Block::i_min)->getEdge(b1->getVertex(4), b1->getVertex(6))->setGeomAssociation(curves[5]);
    b1->getFace(Block::j_min)->getEdge(b1->getVertex(4), b1->getVertex(5))->setGeomAssociation(curves[4]);

    b3->getFace(Block::j_max)->getEdge(b3->getVertex(2), b3->getVertex(6))->setGeomAssociation(curves[4]);
    b5->getFace(Block::j_min)->getEdge(b5->getVertex(0), b5->getVertex(4))->setGeomAssociation(curves[5]);
    b6->getFace(Block::i_max)->getEdge(b6->getVertex(1), b6->getVertex(5))->setGeomAssociation(curves[3]);

    b1->getVertex(0)->setGeomAssociation(curves[3]);
    b1->getVertex(6)->setGeomAssociation(curves[5]);
    b1->getVertex(5)->setGeomAssociation(curves[4]);

    b1->getVertex(2)->setGeomAssociation(surfaces[3]);
    b1->getVertex(1)->setGeomAssociation(surfaces[1]);
    b1->getVertex(7)->setGeomAssociation(surfaces[2]);

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createHollowCylinderTopo()
{
    Geom::PropertyHollowCylinder* propertyCyl = getGeomEntity()->getGeomProperty()->toPropertyHollowCylinder();
#ifdef _DEBUG2
    std::cout<<"createHollowCylinderTopo avec angle de "<<propertyCyl->getAngle()<<std::endl;
#endif

    double angle = propertyCyl->getAngle();

    if (angle<=135.0){
    	createHollowCylinderTopoQuart(propertyCyl);
    }
    else if (angle>135.0 && angle < 360.0){
    	createHollowCylinderTopoDemi(propertyCyl);
    }
    else if (angle == 360.0){
    	createHollowCylinderTopoEntier(propertyCyl);
    }
    else {
    	throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOGridOnGeometry ne peut se faire pour cet angle de cylindre creux", TkUtil::Charset::UTF_8));
    }


#ifdef _DEBUG2
    // LOG de la Topo résultante
	TkUtil::UTF8String	message_result (TkUtil::Charset::UTF_8);
    std::vector<Block* > blocks;
    getBlocks(blocks);
    message_result << "CommandNewTopoOGridOnGeometry donne comme topologie: \n";
    for (std::vector<Block* >::iterator iter = blocks.begin();
            iter != blocks.end(); ++iter)
        message_result << **iter;
    log (TkUtil::TraceLog (message_result, TkUtil::Log::TRACE_4));
#endif
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createHollowCylinderTopoQuart(Geom::PropertyHollowCylinder* propertyCyl)
{
#ifdef _DEBUG2
	std::cout<<"createHollowCylinderTopoQuart(...)"<<std::endl;
#endif

	// création de 1 bloc
    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques du bloc
    addCreatedBlock(b1);

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // on utilise directement les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 8){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 8 sommets géométriques mais "
                <<vertices.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    b1->getVertex(false,false,false)->setGeomAssociation(vertices[7]);
    b1->getVertex(true ,false,false)->setGeomAssociation(vertices[6]);
    b1->getVertex(false,true ,false)->setGeomAssociation(vertices[4]);
    b1->getVertex(true ,true ,false)->setGeomAssociation(vertices[5]);
    b1->getVertex(false,false,true )->setGeomAssociation(vertices[3]);
    b1->getVertex(true ,false,true )->setGeomAssociation(vertices[2]);
    b1->getVertex(false,true ,true )->setGeomAssociation(vertices[1]);
    b1->getVertex(true ,true ,true )->setGeomAssociation(vertices[0]);

    snapVertices();

    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createHollowCylinderTopoDemi(Geom::PropertyHollowCylinder* propertyCyl)
{
#ifdef _DEBUG2
	std::cout<<"createHollowCylinderTopoQuart(...)"<<std::endl;
#endif

	// création de 1 bloc
    Topo::Block* b1 = new Topo::Block(getContext(), 4*m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques du bloc
    addCreatedBlock(b1);

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    // on utilise directement les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 8){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 8 sommets géométriques mais "
                <<vertices.size()<<" pour le cylindre "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    b1->getVertex(false,false,false)->setGeomAssociation(vertices[7]);
    b1->getVertex(true ,false,false)->setGeomAssociation(vertices[6]);
    b1->getVertex(false,true ,false)->setGeomAssociation(vertices[4]);
    b1->getVertex(true ,true ,false)->setGeomAssociation(vertices[5]);
    b1->getVertex(false,false,true )->setGeomAssociation(vertices[3]);
    b1->getVertex(true ,false,true )->setGeomAssociation(vertices[2]);
    b1->getVertex(false,true ,true )->setGeomAssociation(vertices[1]);
    b1->getVertex(true ,true ,true )->setGeomAssociation(vertices[0]);

    snapVertices();

    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createHollowCylinderTopoEntier(Geom::PropertyHollowCylinder* propertyCyl)
{
#ifdef _DEBUG2
    std::cout<<"createHollowCylinderTopoEntier(...)"<<std::endl;
#endif
    Utils::Math::Point centre = propertyCyl->getCenter();
    double d2 = std::sqrt(2.0)/2.0;

	// CP NEW CODE v 2.2.2
    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr, BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b2 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr, BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr, BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr, BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    // !CP NEW CODE
/* CP OLD CODE
    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, m_nr, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b2 = new Topo::Block(getContext(), 2*m_ni, m_nr, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), 2*m_ni, m_nr, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), 2*m_ni, m_nr, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
*/

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b3);
    addCreatedBlock(b4);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b3->setGeomAssociation(getGeomEntity());
    b4->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);

    if (curves.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 courbes géométriques mais "
                <<curves.size()<<" pour le cylindre creux "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Utils::Math::Point pt;
    Geom::Curve* crv;

    // pour la première courbe interne (j min, x min si c'est l'axe)
    crv = curves[4];
    crv->getPoint(0.125, pt, true);
    b4->getVertex(1)->setCoord(pt);
    b1->getVertex(0)->setCoord(pt);

    crv->getPoint(0.375, pt, true);
    b3->getVertex(1)->setCoord(pt);
    b4->getVertex(0)->setCoord(pt);

    crv->getPoint(0.625, pt, true);
    b2->getVertex(1)->setCoord(pt);
    b3->getVertex(0)->setCoord(pt);

    crv->getPoint(0.875, pt, true);
    b1->getVertex(1)->setCoord(pt);
    b2->getVertex(0)->setCoord(pt);

    b1->getVertex(1)->setGeomAssociation(crv);
    b2->getVertex(1)->setGeomAssociation(crv);
    b3->getVertex(1)->setGeomAssociation(crv);
    b4->getVertex(1)->setGeomAssociation(crv);

    // pour la première courbe externe
    crv = curves[2];
    crv->getPoint(0.125, pt, true);
    b4->getVertex(5)->setCoord(pt);
    b1->getVertex(4)->setCoord(pt);

    crv->getPoint(0.375, pt, true);
    b3->getVertex(5)->setCoord(pt);
    b4->getVertex(4)->setCoord(pt);

    crv->getPoint(0.625, pt, true);
    b2->getVertex(5)->setCoord(pt);
    b3->getVertex(4)->setCoord(pt);

    crv->getPoint(0.875, pt, true);
    b1->getVertex(5)->setCoord(pt);
    b2->getVertex(4)->setCoord(pt);

    b1->getVertex(5)->setGeomAssociation(crv);
    b2->getVertex(5)->setGeomAssociation(crv);
    b3->getVertex(5)->setGeomAssociation(crv);
    b4->getVertex(5)->setGeomAssociation(crv);

    // pour la deuxième courbe interne (j max, x max si c'est l'axe)
    crv = curves[3];
    crv->getPoint(0.125, pt, true);
    b4->getVertex(3)->setCoord(pt);
    b1->getVertex(2)->setCoord(pt);

    crv->getPoint(0.375, pt, true);
    b3->getVertex(3)->setCoord(pt);
    b4->getVertex(2)->setCoord(pt);

    crv->getPoint(0.625, pt, true);
    b2->getVertex(3)->setCoord(pt);
    b3->getVertex(2)->setCoord(pt);

    crv->getPoint(0.875, pt, true);
    b1->getVertex(3)->setCoord(pt);
    b2->getVertex(2)->setCoord(pt);

    b1->getVertex(3)->setGeomAssociation(crv);
    b2->getVertex(3)->setGeomAssociation(crv);
    b3->getVertex(3)->setGeomAssociation(crv);
    b4->getVertex(3)->setGeomAssociation(crv);

    // pour la deuxième courbe externe
    crv = curves[0];
    crv->getPoint(0.125, pt, true);
    b4->getVertex(7)->setCoord(pt);
    b1->getVertex(6)->setCoord(pt);

    crv->getPoint(0.375, pt, true);
    b3->getVertex(7)->setCoord(pt);
    b4->getVertex(6)->setCoord(pt);

    crv->getPoint(0.625, pt, true);
    b2->getVertex(7)->setCoord(pt);
    b3->getVertex(6)->setCoord(pt);

    crv->getPoint(0.875, pt, true);
    b1->getVertex(7)->setCoord(pt);
    b2->getVertex(6)->setCoord(pt);

    b1->getVertex(7)->setGeomAssociation(crv);
    b2->getVertex(7)->setGeomAssociation(crv);
    b3->getVertex(7)->setGeomAssociation(crv);
    b4->getVertex(7)->setGeomAssociation(crv);

    // fusion des faces
    b1->getFace(Block::i_max)->fuse(b2->getFace(Block::i_min), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b3->getFace(Block::i_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b4->getFace(Block::i_min), &getInfoCommand());
    b4->getFace(Block::i_max)->fuse(b1->getFace(Block::i_min), &getInfoCommand());

    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createHollowSphereTopo()
{
    Geom::PropertyHollowSphere* propertySph = getGeomEntity()->getGeomProperty()->toPropertyHollowSphere();
#ifdef _DEBUG2
    std::cout<<"createHollowSphereTopo avec portion de "<<Utils::Portion::getName(propertySph->getPortionType())<<std::endl;
#endif

    Utils::Portion::Type dt = propertySph->getPortionType();

    if (dt == Utils::Portion::ENTIER)
    	createHollowSphereTopoEntier(propertySph);
    else if (dt == Utils::Portion::DEMI)
    	createHollowSphereTopoDemi(propertySph);
    else if (dt == Utils::Portion::QUART)
    	createHollowSphereTopoQuart(propertySph);
    else if (dt == Utils::Portion::HUITIEME)
    	createHollowSphereTopoHuitieme(propertySph);
    else
    	throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOGridOnGeometry ne peut se faire pour cette portion de sphère creuse", TkUtil::Charset::UTF_8));

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createHollowSphereTopoHuitieme(Geom::PropertyHollowSphere* propertySph)
{
    Topo::Block* b3 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b5 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b6 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // les blocs externes pour utiliser des boucles
    std::vector<Block*> blocs;
    blocs.push_back(b3);
    blocs.push_back(b5);
    blocs.push_back(b6);

    // stockage dans le manager topologiques des différentes entités
    for (uint j=0; j<3; j++)
        addCreatedBlock(blocs[j]);

    for (uint j=0; j<3; j++)
        blocs[j]->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    Utils::Math::Point centre = propertySph->getCenter();
    double r1;
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;
    double r2, r3;
    std::vector<Utils::Math::Point> pts;

    // on traite les points à l'extérieur
    r1 = propertySph->getRadiusExt();
    r2 = r1*d2;
    r3 = r1*d3;

    // constitution des points sur la sphère (position de la topo)
    pts.clear();
    pts.push_back(centre+Utils::Math::Point(  0,  r1,   0)); // 0
    pts.push_back(centre+Utils::Math::Point( r2,  r2,   0)); // 1
    pts.push_back(centre+Utils::Math::Point(  0,  r2,  r2)); // 2
    pts.push_back(centre+Utils::Math::Point( r3,  r3,  r3)); // 3
    pts.push_back(centre+Utils::Math::Point(  0,   0,   0)); // 4
    pts.push_back(centre+Utils::Math::Point( r1,   0,   0)); // 5
    pts.push_back(centre+Utils::Math::Point(  0,   0,  r1)); // 6
    pts.push_back(centre+Utils::Math::Point( r2,   0,  r2)); // 7

    // on place les sommets
    for (uint j=0; j<3; j++)
        for (uint i=0; i<4; i++){
            uint k = 0;
            if (j==0)
                k=1;
            else if (j==1)
                k=3;
            else if (j==2)
                k=4;
            blocs[j]->getVertex(i)->setCoord(pts[TopoHelper::tab2IndVtxByFaceOnBlock[k][i]]);
        }


    // on traite les points à l'intérieur
    r1 = propertySph->getRadiusInt();
    r2 = r1*d2;
    r3 = r1*d3;

    // constitution des points sur la sphère (position de la topo)
    pts.clear();
    pts.push_back(centre+Utils::Math::Point(  0,  r1,   0));
    pts.push_back(centre+Utils::Math::Point( r2,  r2,   0));
    pts.push_back(centre+Utils::Math::Point(  0,  r2,  r2));
    pts.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts.push_back(centre+Utils::Math::Point(  0,   0,   0));
    pts.push_back(centre+Utils::Math::Point( r1,   0,   0));
    pts.push_back(centre+Utils::Math::Point(  0,   0,  r1));
    pts.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    // on place les sommets
    for (uint j=0; j<3; j++)
        for (uint i=0; i<4; i++){
            uint k = 0;
            if (j==0)
                k=1;
            else if (j==1)
                k=3;
            else if (j==2)
                k=4;
            blocs[j]->getVertex(i+4)->setCoord(pts[TopoHelper::tab2IndVtxByFaceOnBlock[k][i]]);
        }

    // on fusionne les blocs
    b3->getFace(Block::i_max)->fuse(b6->getFace(Block::j_max), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_max)->fuse(b6->getFace(Block::i_min), &getInfoCommand());

    // les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 sommets géométriques mais "
                <<vertices.size()<<" pour le huitième de sphère creuse "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    b5->getVertex(0)->setGeomAssociation(vertices[0]);
    b6->getVertex(1)->setGeomAssociation(vertices[1]);
    b3->getVertex(2)->setGeomAssociation(vertices[2]);
    b5->getVertex(4)->setGeomAssociation(vertices[3]);
    b6->getVertex(5)->setGeomAssociation(vertices[4]);
    b3->getVertex(6)->setGeomAssociation(vertices[5]);


    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 9){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 9 courbes géométriques mais "
                <<curves.size()<<" pour le huitième de sphère creuse "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    b6->getVertex(0)->setGeomAssociation(curves[0]);
    b3->getVertex(3)->setGeomAssociation(curves[1]);
    b5->getVertex(2)->setGeomAssociation(curves[2]);
    b6->getVertex(4)->setGeomAssociation(curves[5]);
    b3->getVertex(7)->setGeomAssociation(curves[7]);
    b5->getVertex(6)->setGeomAssociation(curves[8]);

    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 5){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 5 surfaces géométriques mais "
                <<surfaces.size()<<" pour le huitième de sphère creuse "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    b6->getVertex(2)->setGeomAssociation(surfaces[0]);
    b6->getVertex(6)->setGeomAssociation(surfaces[4]);


    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createHollowSphereTopoQuart(Geom::PropertyHollowSphere* propertySph)
{
    Topo::Block* b3 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b5 = new Topo::Block(getContext(), m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b6 = new Topo::Block(getContext(), 2*m_ni, m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    std::vector<Block*> blocs;
    blocs.push_back(b3);
    blocs.push_back(b4);
    blocs.push_back(b5);
    blocs.push_back(b6);

    // stockage dans le manager topologiques des différentes entités
    for (uint j=0; j<4; j++)
        addCreatedBlock(blocs[j]);

    for (uint j=0; j<4; j++)
        blocs[j]->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    Utils::Math::Point centre = propertySph->getCenter();
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;
    double r1, r2, r3;
    std::vector<Utils::Math::Point> pts;

    // on traite les points à l'extérieur
    r1 = propertySph->getRadiusExt();
    r2 = r1*d2;
    r3 = r1*d3;

    // constitution des points sur la sphère (position de la topo)
    pts.clear();
    pts.push_back(centre+Utils::Math::Point(  0,  r2, -r2));
    pts.push_back(centre+Utils::Math::Point( r3,  r3, -r3));
    pts.push_back(centre+Utils::Math::Point(  0,  r2,  r2));
    pts.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts.push_back(centre+Utils::Math::Point(  0,   0, -r1));
    pts.push_back(centre+Utils::Math::Point( r2,   0, -r2));
    pts.push_back(centre+Utils::Math::Point(  0,   0,  r1));
    pts.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    // on place les sommets
    for (uint j=0; j<4; j++)
        for (uint i=0; i<4; i++){
                blocs[j]->getVertex(i)->setCoord(pts[TopoHelper::tab2IndVtxByFaceOnBlock[j+1][i]]);
            }


    // on traite les points à l'intérieur
    r1 = propertySph->getRadiusInt();
    r2 = r1*d2;
    r3 = r1*d3;

    // constitution des points sur la sphère (position de la topo)
    pts.clear();
    pts.push_back(centre+Utils::Math::Point(  0,  r2, -r2));
    pts.push_back(centre+Utils::Math::Point( r3,  r3, -r3));
    pts.push_back(centre+Utils::Math::Point(  0,  r2,  r2));
    pts.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts.push_back(centre+Utils::Math::Point(  0,   0, -r1));
    pts.push_back(centre+Utils::Math::Point( r2,   0, -r2));
    pts.push_back(centre+Utils::Math::Point(  0,   0,  r1));
    pts.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    // on place les sommets
    for (uint j=0; j<4; j++)
        for (uint i=0; i<4; i++){
                blocs[j]->getVertex(i+4)->setCoord(pts[TopoHelper::tab2IndVtxByFaceOnBlock[j+1][i]]);
            }

    // on fusionne les blocs
    b4->getFace(Block::i_max)->fuse(b6->getFace(Block::i_max), &getInfoCommand());
    b3->getFace(Block::j_max)->fuse(b4->getFace(Block::j_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b6->getFace(Block::j_max), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_max)->fuse(b6->getFace(Block::i_min), &getInfoCommand());


    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 sommets géométriques mais "
                <<vertices.size()<<" pour le quart de sphère creuse "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    b5->getVertex(0)->setGeomAssociation(vertices[0]);
    b4->getVertex(2)->setGeomAssociation(vertices[1]);
    b5->getVertex(4)->setGeomAssociation(vertices[3]);
    b4->getVertex(6)->setGeomAssociation(vertices[2]);

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 courbes géométriques mais "
                <<curves.size()<<" pour le quart de sphère creuse "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    b3->getVertex(0)->setGeomAssociation(curves[1]);
    b3->getVertex(2)->setGeomAssociation(curves[1]);
    b3->getVertex(4)->setGeomAssociation(curves[5]);
    b3->getVertex(6)->setGeomAssociation(curves[5]);

    b6->getVertex(0)->setGeomAssociation(curves[0]);
    b6->getVertex(1)->setGeomAssociation(curves[0]);
    b6->getVertex(4)->setGeomAssociation(curves[3]);
    b6->getVertex(5)->setGeomAssociation(curves[3]);

    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 surfaces géométriques mais "
                <<surfaces.size()<<" pour le quart de sphère creuse "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    b3->getVertex(1)->setGeomAssociation(surfaces[0]);
    b3->getVertex(3)->setGeomAssociation(surfaces[0]);
    b3->getVertex(5)->setGeomAssociation(surfaces[3]);
    b3->getVertex(7)->setGeomAssociation(surfaces[3]);

    projectEdgesOnCurves();

    projectFacesOnSurfaces();

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createHollowSphereTopoDemi(Geom::PropertyHollowSphere* propertySph)
{
    Topo::Block* b2 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b5 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b6 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // les blocs externes pour utiliser des boucles
    std::vector<Block*> blocs;
    blocs.push_back(b2);
    blocs.push_back(b3);
    blocs.push_back(b4);
    blocs.push_back(b5);
    blocs.push_back(b6);

    // stockage dans le manager topologiques des différentes entités
    for (uint j=0; j<5; j++)
        addCreatedBlock(blocs[j]);

    for (uint j=0; j<5; j++)
        blocs[j]->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    Utils::Math::Point centre = propertySph->getCenter();
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;
    double r1, r2, r3;
    std::vector<Utils::Math::Point> pts;

    // on traite les points à l'extérieur
    r1 = propertySph->getRadiusExt();
    r2 = r1*d2;
    r3 = r1*d3;

    // constitution des points sur la sphère (position de la topo)
    pts.clear();
    pts.push_back(centre+Utils::Math::Point(-r3,  r3, -r3));
    pts.push_back(centre+Utils::Math::Point( r3,  r3, -r3));
    pts.push_back(centre+Utils::Math::Point(-r3,  r3,  r3));
    pts.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts.push_back(centre+Utils::Math::Point(-r2,   0, -r2));
    pts.push_back(centre+Utils::Math::Point( r2,   0, -r2));
    pts.push_back(centre+Utils::Math::Point(-r2,   0,  r2));
    pts.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    // on place les sommets
    for (uint j=0; j<5; j++)
        for (uint i=0; i<4; i++){
                blocs[j]->getVertex(i)->setCoord(pts[TopoHelper::tab2IndVtxByFaceOnBlock[j][i]]);
            }

    // on traite les points à l'intérieur
    r1 = propertySph->getRadiusInt();
    r2 = r1*d2;
    r3 = r1*d3;

    // constitution des points sur la sphère (position de la topo)
    pts.clear();
    pts.push_back(centre+Utils::Math::Point(-r3,  r3, -r3));
    pts.push_back(centre+Utils::Math::Point( r3,  r3, -r3));
    pts.push_back(centre+Utils::Math::Point(-r3,  r3,  r3));
    pts.push_back(centre+Utils::Math::Point( r3,  r3,  r3));
    pts.push_back(centre+Utils::Math::Point(-r2,   0, -r2));
    pts.push_back(centre+Utils::Math::Point( r2,   0, -r2));
    pts.push_back(centre+Utils::Math::Point(-r2,   0,  r2));
    pts.push_back(centre+Utils::Math::Point( r2,   0,  r2));

    // on place les sommets
    for (uint j=0; j<5; j++)
        for (uint i=0; i<4; i++){
                blocs[j]->getVertex(i+4)->setCoord(pts[TopoHelper::tab2IndVtxByFaceOnBlock[j][i]]);
            }

    // on fusionne les blocs
    b2->getFace(Block::j_min)->fuse(b4->getFace(Block::j_max), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b6->getFace(Block::j_min), &getInfoCommand());
    b4->getFace(Block::i_max)->fuse(b6->getFace(Block::i_max), &getInfoCommand());
    b3->getFace(Block::j_max)->fuse(b4->getFace(Block::j_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b6->getFace(Block::j_max), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_max)->fuse(b6->getFace(Block::i_min), &getInfoCommand());
    b2->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());

    // on coupe les faces des blocs b4 et b5 pour qu'une arête soit sur la courbe 2
    std::vector<Edge* > splitingEdges;
    std::vector<CoEdge* > coedges;
    std::vector<CoFace* > cofaces;
    cofaces.push_back(b4->getFace(Block::i_min)->getCoFace(0));
    TopoHelper::splitFaces(cofaces,
            b4->getFace(Block::i_min)->getEdge(b4->getVertex(0), b4->getVertex(2))->getCoEdge(0),
            0.5, 0.0, false, false,
            splitingEdges,
            &getInfoCommand());

    coedges.push_back(splitingEdges[0]->getCoEdge(0));
    cofaces.clear();
    cofaces.push_back(b5->getFace(Block::i_min)->getCoFace(0));
    TopoHelper::splitFaces(cofaces,
            b5->getFace(Block::i_min)->getEdge(b5->getVertex(0), b5->getVertex(2))->getCoEdge(0),
            0.5, 0.0, false, false,
            splitingEdges,
            &getInfoCommand());
    coedges.push_back(splitingEdges[0]->getCoEdge(0));


    // les sommets géométriques
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    if (vertices.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 sommets géométriques mais "
                <<vertices.size()<<" pour la demi sphère creuse "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    coedges[0]->getVertex(0)->setGeomAssociation(vertices[1]);
    coedges[0]->getVertex(1)->setGeomAssociation(vertices[2]);
    coedges[1]->getVertex(0)->setGeomAssociation(vertices[0]);
    coedges[1]->getVertex(1)->setGeomAssociation(vertices[3]);

    coedges[0]->getVertex(0)->setCoord(vertices[1]->getCoord());
    coedges[0]->getVertex(1)->setCoord(vertices[2]->getCoord());
    coedges[1]->getVertex(0)->setCoord(vertices[0]->getCoord());
    coedges[1]->getVertex(1)->setCoord(vertices[3]->getCoord());


    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    if (curves.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 courbes géométriques mais "
                <<curves.size()<<" pour la demi sphère creuse "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    b2->getVertex(0)->setGeomAssociation(curves[0]);
    b2->getVertex(2)->setGeomAssociation(curves[0]);
    b2->getVertex(4)->setGeomAssociation(curves[3]);
    b2->getVertex(6)->setGeomAssociation(curves[3]);

    b3->getVertex(0)->setGeomAssociation(curves[1]);
    b3->getVertex(2)->setGeomAssociation(curves[1]);
    b3->getVertex(4)->setGeomAssociation(curves[5]);
    b3->getVertex(6)->setGeomAssociation(curves[5]);

    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 surfaces géométriques mais "
                <<surfaces.size()<<" pour la demi sphère creuse "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    b2->getVertex(1)->setGeomAssociation(surfaces[0]);
    b2->getVertex(3)->setGeomAssociation(surfaces[0]);
    b2->getVertex(5)->setGeomAssociation(surfaces[3]);
    b2->getVertex(7)->setGeomAssociation(surfaces[3]);

    b3->getVertex(1)->setGeomAssociation(surfaces[0]);
    b3->getVertex(3)->setGeomAssociation(surfaces[0]);
    b3->getVertex(5)->setGeomAssociation(surfaces[3]);
    b3->getVertex(7)->setGeomAssociation(surfaces[3]);

    projectEdgesOnCurves();

    projectFacesOnSurfaces();

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createHollowSphereTopoEntier(Geom::PropertyHollowSphere* propertySph)
{
    Topo::Block* b2 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b5 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b6 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b7 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // les blocs externes pour utiliser des boucles
    std::vector<Block*> blocs;
    blocs.push_back(b2);
    blocs.push_back(b3);
    blocs.push_back(b4);
    blocs.push_back(b5);
    blocs.push_back(b6);
    blocs.push_back(b7);

    // stockage dans le manager topologiques des différentes entités
    for (uint j=0; j<6; j++)
        addCreatedBlock(blocs[j]);

    for (uint j=0; j<6; j++)
        blocs[j]->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    Utils::Math::Point centre = propertySph->getCenter();
    double d2 = std::sqrt(2.0)/2.0;
    double d3 = std::sqrt(3.0)/3.0;
    double r1, r2, r3;
    std::vector<Utils::Math::Point> pts;

    // on traite les points à l'extérieur
    r1 = propertySph->getRadiusExt();
    r2 = r1*d2;
    r3 = r1*d3;

    // constitution des points sur la sphère (position de la topo)
    pts.clear();
    pts.push_back(centre+Utils::Math::Point(-r3, -r3, -r3));
    pts.push_back(centre+Utils::Math::Point( r3, -r3, -r3));
    pts.push_back(centre+Utils::Math::Point(-r3,  r3, -r3));
    pts.push_back(centre+Utils::Math::Point( r3,  r3, -r3));
    pts.push_back(centre+Utils::Math::Point(-r3, -r3,  r3));
    pts.push_back(centre+Utils::Math::Point( r3, -r3,  r3));
    pts.push_back(centre+Utils::Math::Point(-r3,  r3,  r3));
    pts.push_back(centre+Utils::Math::Point( r3,  r3,  r3));

    // on place les sommets
    for (uint j=0; j<6; j++)
        for (uint i=0; i<4; i++){
                blocs[j]->getVertex(i)->setCoord(pts[TopoHelper::tab2IndVtxByFaceOnBlock[j][i]]);
            }

    // on traite les points à l'intérieur
    r1 = propertySph->getRadiusInt();
    r2 = r1*d2;
    r3 = r1*d3;

    // constitution des points sur la sphère (position de la topo)
    pts.clear();
    pts.push_back(centre+Utils::Math::Point(-r3, -r3, -r3));
    pts.push_back(centre+Utils::Math::Point( r3, -r3, -r3));
    pts.push_back(centre+Utils::Math::Point(-r3,  r3, -r3));
    pts.push_back(centre+Utils::Math::Point( r3,  r3, -r3));
    pts.push_back(centre+Utils::Math::Point(-r3, -r3,  r3));
    pts.push_back(centre+Utils::Math::Point( r3, -r3,  r3));
    pts.push_back(centre+Utils::Math::Point(-r3,  r3,  r3));
    pts.push_back(centre+Utils::Math::Point( r3,  r3,  r3));

    // on place les sommets
    for (uint j=0; j<6; j++)
        for (uint i=0; i<4; i++){
                blocs[j]->getVertex(i+4)->setCoord(pts[TopoHelper::tab2IndVtxByFaceOnBlock[j][i]]);
            }


    // on fusionne les blocs
    b2->getFace(Block::j_min)->fuse(b4->getFace(Block::j_max), &getInfoCommand());
    b2->getFace(Block::i_min)->fuse(b7->getFace(Block::j_max), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b6->getFace(Block::j_min), &getInfoCommand());
    b4->getFace(Block::i_min)->fuse(b7->getFace(Block::i_max), &getInfoCommand());
    b4->getFace(Block::i_max)->fuse(b6->getFace(Block::i_max), &getInfoCommand());
    b3->getFace(Block::j_max)->fuse(b4->getFace(Block::j_min), &getInfoCommand());
    b3->getFace(Block::i_min)->fuse(b7->getFace(Block::j_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b6->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_min)->fuse(b7->getFace(Block::i_min), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b5->getFace(Block::i_max)->fuse(b6->getFace(Block::i_min), &getInfoCommand());
    b2->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());


    // les projections sur la géométrie
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);
    if (surfaces.size() != 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 2 surface géométrique mais "
                <<surfaces.size()<<" pour la sphère creuse "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    // les projections
    for (uint j=0; j<6; j++){
        Face* face = blocs[j]->getFace(Block::k_min);
        face->setGeomAssociation(surfaces[0]);

        for (uint i=0; i<4; i++){
            face->getEdge(i)->setGeomAssociation(surfaces[0]);
            face->getVertex(i)->setGeomAssociation(surfaces[0]);
        }
    }
    for (uint j=0; j<6; j++){
        Face* face = blocs[j]->getFace(Block::k_max);
        face->setGeomAssociation(surfaces[1]);

        for (uint i=0; i<4; i++){
            face->getEdge(i)->setGeomAssociation(surfaces[1]);
            face->getVertex(i)->setGeomAssociation(surfaces[1]);
        }
    }

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
snapVertices()
{
	// on déplace uniquement les sommets projetés sur des points de la géométrie
	std::vector<Block*> blocks;
	getBlocks(blocks);

	for (uint i=0; i<blocks.size(); i++){
		std::vector<Vertex*> vertices;
		blocks[i]->getVertices(vertices);
		for (uint j=0; j<vertices.size(); j++){
			Vertex* vtx = vertices[j];
			if (vtx->getGeomAssociation()){
				Geom::GeomEntity* ge = vtx->getGeomAssociation();
				if (ge->getType() == Utils::Entity::GeomVertex){
					Utils::Math::Point pt1 = vtx->getCoord();
					Utils::Math::Point pt2;
					ge->project(pt1, pt2);
					vtx->setCoord(pt2);
				}
			}
		}
	}
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
projectEdgesOnCurves()
{
	std::vector<Block*> blocks;
	getBlocks(blocks);

	for (uint i=0; i<blocks.size(); i++){
		std::vector<CoEdge*> coedges;
		blocks[i]->getCoEdges(coedges);
		for (uint j=0; j<coedges.size(); j++){
			CoEdge* coedge = coedges[j];

			// pour éviter de refaire 2 fois la même chose
			if (coedge->getGeomAssociation() != 0)
				continue;

			// recherche des courbes associées au premier sommet topo
			std::vector<Geom::Curve*> curves1;
			Internal::EntitiesHelper::getAssociatedCurves(coedge->getVertex(0), curves1);

			// idem avec le 2ème sommet
			std::vector<Geom::Curve*> curves2;
			Internal::EntitiesHelper::getAssociatedCurves(coedge->getVertex(1), curves2);

			// recherche de la courbe commune entre les deux groupes
			Geom::GeomEntity* ge = Geom::GeomHelper::getCommonCurve(curves1, curves2);

			if (ge == 0){
				// on tente avec une surface
				// recherche des surfaces associées au premier sommet topo
				std::vector<Geom::Surface*> surf1;
				Internal::EntitiesHelper::getAssociatedSurfaces(coedge->getVertex(0), surf1);

				// idem avec le 2ème sommet
				std::vector<Geom::Surface*> surf2;
				Internal::EntitiesHelper::getAssociatedSurfaces(coedge->getVertex(1), surf2);

				// recherche de la surface commune entre les deux groupes
				ge = Geom::GeomHelper::getCommonSurface(surf1, surf2);
			}

			if (ge)
				coedge->setGeomAssociation(ge);

		} // end for j
	} // end for i
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
projectFacesOnSurfaces()
{
	std::vector<Block*> blocks;
	getBlocks(blocks);

	for (uint i=0; i<blocks.size(); i++){
		std::vector<CoFace*> cofaces;
		blocks[i]->getCoFaces(cofaces);
		for (uint j=0; j<cofaces.size(); j++){
			CoFace* coface = cofaces[j];

			// pour éviter de refaire 2 fois la même chose
			if (coface->getGeomAssociation() != 0)
				continue;

			Geom::Surface* surface = Internal::EntitiesHelper::getCommonSurface(coface);

			if (surface)
				coface->setGeomAssociation(surface);

		} // end for j
	} // end for i

}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationCoedgeDisplayModified(dr);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createConeTopo1Block()
{
    Geom::PropertyCone* propertyCone = getGeomEntity()->getGeomProperty()->toPropertyCone();
    double angle = propertyCone->getAngle();
    double r1 = propertyCone->getRadius1();
#ifdef _DEBUG2
    std::cout<<"createConeTopo1Block avec angle de "<<angle<<" et rayon 1 "<<r1<<std::endl;
#endif

    if (angle<=135.0){
    	if (r1 == 0)
    		createConeTopo1BlockQuartR0();
    	else
    		createConeTopo1BlockQuart();
    }
    else if (angle>135.0 && angle < 360.0){
    	if (r1 == 0)
    		createConeTopo1BlockDemiR0();
    	else
    		createConeTopo1BlockDemi();
    }
    else if (angle == 360.0){
    	if (r1 == 0)
    		createConeTopo1BlockPleinR0();
    	else
    		createConeTopo1BlockPlein();
    }
    else {
        throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOGridOnGeometry ne peut se faire pour cet angle de cône", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::
createConeTopoOGrid()
{
    Geom::PropertyCone* propertyCone = getGeomEntity()->getGeomProperty()->toPropertyCone();
    double angle = propertyCone->getAngle();
    double r1 = propertyCone->getRadius1();
#ifdef _DEBUG2
    std::cout<<"createConeTopoOGrid avec angle de "<<angle<<" et rayon 1 "<<r1<<std::endl;
#endif

    if (angle<=135.0){
        // création d'une topologie à 1 ou 3 blocs suivant si m_ratio == 0 ou non
        if (m_ratio > 0.0)
        	if (r1 == 0)
        		createConeTopoOGridQuartNonDegR0();
        	else
        		createConeTopoOGridQuartNonDeg();
        else
        	if (r1 == 0)
        		createConeTopoOGridQuartDegR0();
        	else
        		createConeTopoOGridQuartDeg();
    }
    else if (angle>135.0 && angle < 360.0){
        // création d'une topologie à 1 ou 4 blocs suivant si m_ratio == 0 ou non
    	if (m_ratio > 0.0)
    		if (r1 == 0)
    			createConeTopoOGridDemiNonDegR0();
    		else
    			createConeTopoOGridDemiNonDeg();
    	else
    		if (r1 == 0)
    			createConeTopoOGridDemiDegR0();
    		else
    			createConeTopoOGridDemiDeg();
    }
    else if (angle == 360.0){
        // création d'une topologie à 4 ou 5 blocs suivant si m_ratio == 0 ou non
    	if (m_ratio > 0.0)
    		if (r1 == 0)
    			createConeTopoOGridPleinNonDegR0();
    		else
    			createConeTopoOGridPleinNonDeg();
    	else
    		if (r1 == 0)
    			createConeTopoOGridPleinDegR0();
    		else
    			createConeTopoOGridPleinDeg();
    }
    else {
        throw TkUtil::Exception (TkUtil::UTF8String ("CommandNewTopoOGridOnGeometry ne peut se faire pour cet angle de cylindre", TkUtil::Charset::UTF_8));
    }


#ifdef _DEBUG2
    // LOG de la Topo résultante
	TkUtil::UTF8String	message_result (TkUtil::Charset::UTF_8);
    std::vector<Block* > blocks;
    getBlocks(blocks);
    message_result << "CommandNewTopoOGridOnGeometry donne comme topologie: \n";
    for (std::vector<Block* >::iterator iter = blocks.begin();
            iter != blocks.end(); ++iter)
        message_result << **iter;
    log (TkUtil::TraceLog (message_result, TkUtil::Log::TRACE_4));
#endif
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopo1BlockQuartR0()
{
    Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);

    b1->degenerateFaceInVertex(5, &getInfoCommand());

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);

    if (curves.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 courbes géométriques mais "
                <<curves.size()<<" pour le quart de cône avec un rayon nul "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (vertices.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 sommets géométriques mais "
                <<vertices.size()<<" pour le quart de cône avec un rayon nul "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Utils::Math::Point pt;
    Geom::Curve* crv;

    // x min, k max
    pt = vertices[2]->getCoord();
    b1->getVertex(4)->setCoord(pt);
    b1->getVertex(4)->setGeomAssociation(vertices[2]);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.5, pt, true);
    b1->getVertex(1)->setCoord(pt);
    b1->getVertex(1)->setGeomAssociation(crv);

    pt = vertices[0]->getCoord();
    b1->getVertex(0)->setCoord(pt);
    b1->getVertex(0)->setGeomAssociation(vertices[0]);

    pt = vertices[3]->getCoord();
    b1->getVertex(2)->setCoord(pt);
    b1->getVertex(2)->setGeomAssociation(vertices[3]);

    pt = vertices[1]->getCoord();
    b1->getVertex(3)->setCoord(pt);
    b1->getVertex(3)->setGeomAssociation(vertices[1]);

    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopo1BlockQuart()
{
    Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);

    if (curves.size() != 9){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 9 courbes géométriques mais "
                <<curves.size()<<" pour le quart de cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (vertices.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 sommets géométriques mais "
                <<vertices.size()<<" pour le quart de cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Utils::Math::Point pt;
    Geom::Curve* crv;

    // x min, k max
    crv = curves[2];
    crv->getPoint(0.5, pt, true);
    b1->getVertex(5)->setCoord(pt);
    b1->getVertex(5)->setGeomAssociation(crv);

    pt = vertices[2]->getCoord();
    b1->getVertex(4)->setCoord(pt);
    b1->getVertex(4)->setGeomAssociation(vertices[2]);

    pt = vertices[5]->getCoord();
    b1->getVertex(6)->setCoord(pt);
    b1->getVertex(6)->setGeomAssociation(vertices[5]);

    pt = vertices[3]->getCoord();
    b1->getVertex(7)->setCoord(pt);
    b1->getVertex(7)->setGeomAssociation(vertices[3]);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.5, pt, true);
    b1->getVertex(1)->setCoord(pt);
    b1->getVertex(1)->setGeomAssociation(crv);

    pt = vertices[0]->getCoord();
    b1->getVertex(0)->setCoord(pt);
    b1->getVertex(0)->setGeomAssociation(vertices[0]);

    pt = vertices[4]->getCoord();
    b1->getVertex(2)->setCoord(pt);
    b1->getVertex(2)->setGeomAssociation(vertices[4]);

    pt = vertices[1]->getCoord();
    b1->getVertex(3)->setCoord(pt);
    b1->getVertex(3)->setGeomAssociation(vertices[1]);

    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopo1BlockDemiR0()
{
    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);

    b1->degenerateFaceInVertex(5, &getInfoCommand());

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);

    if (curves.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 courbes géométriques mais "
                <<curves.size()<<" pour le demi cône avec un rayon nul "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (vertices.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 sommets géométriques mais "
                <<vertices.size()<<" pour le demi cône avec un rayon nul "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Utils::Math::Point pt;
    Geom::Curve* crv;

    // x min, k max
    pt = vertices[2]->getCoord();
    b1->getVertex(4)->setCoord(pt);
    b1->getVertex(4)->setGeomAssociation(vertices[2]);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.75, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b1->getVertex(0)->setGeomAssociation(crv);

    crv->getPoint(0.25, pt, true);
    b1->getVertex(1)->setCoord(pt);
    b1->getVertex(1)->setGeomAssociation(crv);

    pt = vertices[0]->getCoord();
    b1->getVertex(2)->setCoord(pt);
    b1->getVertex(2)->setGeomAssociation(vertices[0]);

    pt = vertices[1]->getCoord();
    b1->getVertex(3)->setCoord(pt);
    b1->getVertex(3)->setGeomAssociation(vertices[1]);

    // découpage de la face
    std::vector<Edge* > splitingEdges;
    std::vector<CoFace* > cofaces;
    cofaces.push_back(b1->getFace(Block::j_max)->getCoFace(0));
    TopoHelper::splitFaces(cofaces,
    		cofaces[0]->getEdge(b1->getVertex(2), b1->getVertex(3))->getCoEdge(0),
			0.5, 0.0, false, false,
			splitingEdges,
			&getInfoCommand());

    pt = vertices[3]->getCoord();
    splitingEdges[0]->getVertex(0)->setCoord(pt);
    splitingEdges[0]->getVertex(0)->setGeomAssociation(vertices[3]);

    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopo1BlockDemi()
{
    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);

    if (curves.size() != 9){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 9 courbes géométriques mais "
                <<curves.size()<<" pour le demi cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (vertices.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 sommets géométriques mais "
                <<vertices.size()<<" pour le demi cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Utils::Math::Point pt;
    Geom::Curve* crv;

    // x min, k max
    crv = curves[2];
    crv->getPoint(0.75, pt, true);
    b1->getVertex(4)->setCoord(pt);
    b1->getVertex(4)->setGeomAssociation(crv);

    crv->getPoint(0.25, pt, true);
    b1->getVertex(5)->setCoord(pt);
    b1->getVertex(5)->setGeomAssociation(crv);

    pt = vertices[2]->getCoord();
    b1->getVertex(6)->setCoord(pt);
    b1->getVertex(6)->setGeomAssociation(vertices[2]);

    pt = vertices[3]->getCoord();
    b1->getVertex(7)->setCoord(pt);
    b1->getVertex(7)->setGeomAssociation(vertices[3]);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.75, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b1->getVertex(0)->setGeomAssociation(crv);

    crv->getPoint(0.25, pt, true);
    b1->getVertex(1)->setCoord(pt);
    b1->getVertex(1)->setGeomAssociation(crv);

    pt = vertices[0]->getCoord();
    b1->getVertex(2)->setCoord(pt);
    b1->getVertex(2)->setGeomAssociation(vertices[0]);

    pt = vertices[1]->getCoord();
    b1->getVertex(3)->setCoord(pt);
    b1->getVertex(3)->setGeomAssociation(vertices[1]);

    // découpage de la face
    std::vector<Edge* > splitingEdges;
    std::vector<CoFace* > cofaces;
    cofaces.push_back(b1->getFace(Block::j_max)->getCoFace(0));
    TopoHelper::splitFaces(cofaces,
    		cofaces[0]->getEdge(b1->getVertex(2), b1->getVertex(3))->getCoEdge(0),
			0.5, 0.0, false, false,
			splitingEdges,
			&getInfoCommand());

    pt = vertices[4]->getCoord();
    splitingEdges[0]->getVertex(0)->setCoord(pt);
    splitingEdges[0]->getVertex(0)->setGeomAssociation(vertices[4]);

    pt = vertices[5]->getCoord();
    splitingEdges[0]->getVertex(1)->setCoord(pt);
    splitingEdges[0]->getVertex(1)->setGeomAssociation(vertices[5]);

    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopo1BlockPleinR0()
{
    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);

    b1->degenerateFaceInVertex(5, &getInfoCommand());

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);

    if (curves.size() != 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 2 courbes géométriques mais "
                <<curves.size()<<" pour le cône avec un rayon nul "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (vertices.size() != 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 2 sommets géométriques mais "
                <<vertices.size()<<" pour le cône avec un rayon nul "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Utils::Math::Point pt;
    Geom::Curve* crv;

    // x min, k max
    pt = vertices[1]->getCoord();
    b1->getVertex(4)->setCoord(pt);
    b1->getVertex(4)->setGeomAssociation(vertices[1]);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.125, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b1->getVertex(0)->setGeomAssociation(crv);

    crv->getPoint(0.375, pt, true);
    b1->getVertex(1)->setCoord(pt);
    b1->getVertex(1)->setGeomAssociation(crv);

    crv->getPoint(0.625, pt, true);
    b1->getVertex(3)->setCoord(pt);
    b1->getVertex(3)->setGeomAssociation(crv);

    crv->getPoint(0.875, pt, true);
    b1->getVertex(2)->setCoord(pt);
    b1->getVertex(2)->setGeomAssociation(crv);

    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopo1BlockPlein()
{
    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_naxe,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités
    addCreatedBlock(b1);

    b1->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);

    if (curves.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 courbes géométriques mais "
                <<curves.size()<<" pour le cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Utils::Math::Point pt;
    Geom::Curve* crv;

    // x min, k max
    crv = curves[2];
    crv->getPoint(0.125, pt, true);
    b1->getVertex(4)->setCoord(pt);
    b1->getVertex(4)->setGeomAssociation(crv);

    crv->getPoint(0.375, pt, true);
    b1->getVertex(5)->setCoord(pt);
    b1->getVertex(5)->setGeomAssociation(crv);

    crv->getPoint(0.625, pt, true);
    b1->getVertex(7)->setCoord(pt);
    b1->getVertex(7)->setGeomAssociation(crv);

    crv->getPoint(0.875, pt, true);
    b1->getVertex(6)->setCoord(pt);
    b1->getVertex(6)->setGeomAssociation(crv);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.125, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b1->getVertex(0)->setGeomAssociation(crv);

    crv->getPoint(0.375, pt, true);
    b1->getVertex(1)->setCoord(pt);
    b1->getVertex(1)->setGeomAssociation(crv);

    crv->getPoint(0.625, pt, true);
    b1->getVertex(3)->setCoord(pt);
    b1->getVertex(3)->setGeomAssociation(crv);

    crv->getPoint(0.875, pt, true);
    b1->getVertex(2)->setCoord(pt);
    b1->getVertex(2)->setGeomAssociation(crv);

    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopoOGridQuartDegR0()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"Cône avec 2 dégénerescences, configuration de maillage structuré non prévu actuellement.";
    throw TkUtil::Exception(message);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopoOGridQuartNonDegR0()
{
	Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_nr, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b2 = new Topo::Block(getContext(), m_nr, m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b5 = new Topo::Block(getContext(), m_ni, m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b5);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b5->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    b1->degenerateFaceInVertex(5, &getInfoCommand());
    b2->degenerateFaceInVertex(5, &getInfoCommand());
    b5->degenerateFaceInVertex(5, &getInfoCommand());

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);

    if (curves.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 courbes géométriques mais "
                <<curves.size()<<" pour le quart de cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (vertices.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 sommets géométriques mais "
                <<vertices.size()<<" pour le quart de cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (surfaces.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 surfaces géométriques mais "
                <<surfaces.size()<<" pour le quart de cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    Utils::Math::Point pt;
    Geom::Curve* crv;
    // x min, k max
    b1->getVertex(4)->setCoord(pt);
    b2->getVertex(4)->setCoord(pt);
    b5->getVertex(4)->setCoord(pt);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.5, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b2->getVertex(0)->setCoord(pt);

    pt = vertices[0]->getCoord();
    b1->getVertex(1)->setCoord(pt);
    b1->getVertex(1)->setGeomAssociation(vertices[0]);

    pt = vertices[1]->getCoord();
    b2->getVertex(2)->setCoord(pt);
    b2->getVertex(2)->setGeomAssociation(vertices[1]);

    // les sommets au centre de l'ogrid
    double unSurRac2 = 1/sqrt(2);
    Utils::Math::Point pt_centre;
    pt_centre = vertices[3]->getCoord();
    pt = pt_centre+(b1->getVertex(0)->getCoord()-pt_centre)*m_ratio;
    b1->getVertex(2)->setCoord(pt);
    b2->getVertex(1)->setCoord(pt);
    b5->getVertex(0)->setCoord(pt);

    pt = pt_centre+(b1->getVertex(1)->getCoord()-pt_centre)*m_ratio*unSurRac2;
    b1->getVertex(3)->setCoord(pt);
    b5->getVertex(1)->setCoord(pt);

    pt = vertices[3]->getCoord();
    b5->getVertex(3)->setCoord(pt);

    pt = pt_centre+(b2->getVertex(2)->getCoord()-pt_centre)*m_ratio*unSurRac2;
    b2->getVertex(3)->setCoord(pt);
    b5->getVertex(2)->setCoord(pt);

    // fusion des faces avec l'ogrid
    b1->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b5->getFace(Block::i_min), &getInfoCommand());

    // fusion des faces autour de l'ogrid
    b1->getFace(Block::i_min)->fuse(b2->getFace(Block::j_min), &getInfoCommand());

    // les associations
    crv = curves[0];
    b1->getVertex(0)->setGeomAssociation(crv);

    crv = curves[4];
    b1->getVertex(3)->setGeomAssociation(crv);
    crv = curves[3];
    b2->getVertex(3)->setGeomAssociation(crv);

    Geom::Surface* surf;
    surf = surfaces[1];
    b1->getVertex(2)->setGeomAssociation(surf);

    b5->getVertex(4)->setGeomAssociation(vertices[2]);
    b5->getVertex(3)->setGeomAssociation(vertices[3]);


    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopoOGridQuartDeg()
{
    Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b2 = new Topo::Block(getContext(), m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);

    if (curves.size() != 9){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 9 courbes géométriques mais "
                <<curves.size()<<" pour le quart de cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (vertices.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 sommets géométriques mais "
                <<vertices.size()<<" pour le quart de cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Utils::Math::Point pt;
    Geom::Curve* crv;
   // x min, k max
    crv = curves[2];
    crv->getPoint(0.5, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b2->getVertex(1)->setCoord(pt);

    pt = vertices[2]->getCoord();
    b1->getVertex(1)->setCoord(pt);
    b1->getVertex(1)->setGeomAssociation(vertices[2]);

    pt = vertices[3]->getCoord();
    b2->getVertex(0)->setCoord(pt);
    b2->getVertex(0)->setGeomAssociation(vertices[3]);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.5, pt, true);
    b1->getVertex(2)->setCoord(pt);
    b2->getVertex(3)->setCoord(pt);

    pt = vertices[0]->getCoord();
    b1->getVertex(3)->setCoord(pt);
    b1->getVertex(3)->setGeomAssociation(vertices[0]);

    pt = vertices[1]->getCoord();
    b2->getVertex(2)->setCoord(pt);
    b2->getVertex(2)->setGeomAssociation(vertices[1]);

    // dégénéréscence des blocs
    b1->degenerateFaceInEdge(Block::k_max, b1->getVertex(4), b1->getVertex(5), &getInfoCommand());
    b2->degenerateFaceInEdge(Block::k_max, b2->getVertex(4), b2->getVertex(5), &getInfoCommand());

    // les sommets au centre de l'ogrid
    pt = vertices[5]->getCoord();
    b1->getVertex(4)->setCoord(pt);
    b2->getVertex(4)->setCoord(pt);

    pt = vertices[4]->getCoord();
    b1->getVertex(5)->setCoord(pt);
    b2->getVertex(5)->setCoord(pt);


    // fusion des faces autour de l'ogrid
    b1->getFace(Block::i_min)->fuse(b2->getFace(Block::i_max), &getInfoCommand());

    // les associations
    b1->getVertex(4)->setGeomAssociation(vertices[5]);
    b1->getVertex(5)->setGeomAssociation(vertices[4]);

    crv = curves[2];
    b1->getVertex(0)->setGeomAssociation(crv);
    crv = curves[0];
    b1->getVertex(2)->setGeomAssociation(crv);


    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopoOGridQuartNonDeg()
{
	Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_nr, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b2 = new Topo::Block(getContext(), m_nr, m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b5 = new Topo::Block(getContext(), m_ni, m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b5);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b5->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);

    if (curves.size() != 9){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 9 courbes géométriques mais "
                <<curves.size()<<" pour le quart de cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (vertices.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 sommets géométriques mais "
                <<vertices.size()<<" pour le quart de cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (surfaces.size() != 5){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 5 surfaces géométriques mais "
                <<surfaces.size()<<" pour le quart de cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    Utils::Math::Point pt;
    Geom::Curve* crv;
   // x min, k max
    crv = curves[2];
    crv->getPoint(0.5, pt, true);
    b1->getVertex(4)->setCoord(pt);
    b2->getVertex(4)->setCoord(pt);

    pt = vertices[2]->getCoord();
    b1->getVertex(5)->setCoord(pt);
    b1->getVertex(5)->setGeomAssociation(vertices[2]);

    pt = vertices[3]->getCoord();
    b2->getVertex(6)->setCoord(pt);
    b2->getVertex(6)->setGeomAssociation(vertices[3]);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.5, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b2->getVertex(0)->setCoord(pt);

    pt = vertices[0]->getCoord();
    b1->getVertex(1)->setCoord(pt);
    b1->getVertex(1)->setGeomAssociation(vertices[0]);

    pt = vertices[1]->getCoord();
    b2->getVertex(2)->setCoord(pt);
    b2->getVertex(2)->setGeomAssociation(vertices[1]);

    // les sommets au centre de l'ogrid
    double unSurRac2 = 1/sqrt(2);
    Utils::Math::Point pt_centre = vertices[5]->getCoord();
    pt = pt_centre+(b1->getVertex(4)->getCoord()-pt_centre)*m_ratio;
    b1->getVertex(6)->setCoord(pt);
    b2->getVertex(5)->setCoord(pt);
    b5->getVertex(4)->setCoord(pt);

    pt = pt_centre+(b1->getVertex(5)->getCoord()-pt_centre)*m_ratio*unSurRac2;
    b1->getVertex(7)->setCoord(pt);
    b5->getVertex(5)->setCoord(pt);

    pt = vertices[5]->getCoord();
    b5->getVertex(7)->setCoord(pt);

    pt = pt_centre+(b2->getVertex(6)->getCoord()-pt_centre)*m_ratio*unSurRac2;
    b2->getVertex(7)->setCoord(pt);
    b5->getVertex(6)->setCoord(pt);

    pt_centre = vertices[4]->getCoord();
    pt = pt_centre+(b1->getVertex(0)->getCoord()-pt_centre)*m_ratio;
    b1->getVertex(2)->setCoord(pt);
    b2->getVertex(1)->setCoord(pt);
    b5->getVertex(0)->setCoord(pt);

    pt = pt_centre+(b1->getVertex(1)->getCoord()-pt_centre)*m_ratio*unSurRac2;
    b1->getVertex(3)->setCoord(pt);
    b5->getVertex(1)->setCoord(pt);

    pt = vertices[4]->getCoord();
    b5->getVertex(3)->setCoord(pt);

    pt = pt_centre+(b2->getVertex(2)->getCoord()-pt_centre)*m_ratio*unSurRac2;
    b2->getVertex(3)->setCoord(pt);
    b5->getVertex(2)->setCoord(pt);

    // fusion des faces avec l'ogrid
    b1->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b5->getFace(Block::i_min), &getInfoCommand());

    // fusion des faces autour de l'ogrid
    b1->getFace(Block::i_min)->fuse(b2->getFace(Block::j_min), &getInfoCommand());

    // les associations
    crv = curves[2];
    b1->getVertex(4)->setGeomAssociation(crv);
    crv = curves[0];
    b1->getVertex(0)->setGeomAssociation(crv);

    crv = curves[6];
    b1->getVertex(7)->setGeomAssociation(crv);
    crv = curves[7];
    b2->getVertex(7)->setGeomAssociation(crv);
    crv = curves[5];
    b1->getVertex(3)->setGeomAssociation(crv);
    crv = curves[4];
    b2->getVertex(3)->setGeomAssociation(crv);

    Geom::Surface* surf;
    surf = surfaces[2];
    b1->getVertex(6)->setGeomAssociation(surf);
    surf = surfaces[1];
    b1->getVertex(2)->setGeomAssociation(surf);

    b5->getVertex(7)->setGeomAssociation(vertices[5]);
    b5->getVertex(3)->setGeomAssociation(vertices[4]);


    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopoOGridDemiDegR0()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"Cône avec 2 dégénerescences, configuration de maillage structuré non prévu actuellement.";
    throw TkUtil::Exception(message);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopoOGridDemiNonDegR0()
{
	Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_nr, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b2 = new Topo::Block(getContext(), m_nr, 2*m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b3 = new Topo::Block(getContext(), m_ni, m_nr, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b5 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b3);
    addCreatedBlock(b5);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b3->setGeomAssociation(getGeomEntity());
    b5->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    b1->degenerateFaceInVertex(5, &getInfoCommand());
    b2->degenerateFaceInVertex(5, &getInfoCommand());
    b3->degenerateFaceInVertex(5, &getInfoCommand());
    b5->degenerateFaceInVertex(5, &getInfoCommand());

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);

    if (curves.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 courbes géométriques mais "
                <<curves.size()<<" pour le demi cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (vertices.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 sommets géométriques mais "
                <<vertices.size()<<" pour le demi cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (surfaces.size() != 4){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 4 surfaces géométriques mais "
                <<surfaces.size()<<" pour le demi cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    Utils::Math::Point pt;
    Geom::Curve* crv;
    // x min, k max
    pt = vertices[2]->getCoord();

    b1->getVertex(4)->setCoord(pt);
    b2->getVertex(4)->setCoord(pt);
    b3->getVertex(4)->setCoord(pt);
    b5->getVertex(4)->setCoord(pt);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.75, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b2->getVertex(0)->setCoord(pt);

    crv->getPoint(0.25, pt, true);
    b2->getVertex(2)->setCoord(pt);
    b3->getVertex(2)->setCoord(pt);

    pt = vertices[0]->getCoord();
    b1->getVertex(1)->setCoord(pt);
    b1->getVertex(1)->setGeomAssociation(vertices[0]);

    pt = vertices[1]->getCoord();
    b3->getVertex(3)->setCoord(pt);
    b3->getVertex(3)->setGeomAssociation(vertices[1]);

    // les sommets au centre de l'ogrid
    double unSurRac2 = 1/sqrt(2);
    Utils::Math::Point pt_centre;
    pt_centre = vertices[3]->getCoord();
    pt = pt_centre+(b1->getVertex(0)->getCoord()-pt_centre)*m_ratio;
    b1->getVertex(2)->setCoord(pt);
    b2->getVertex(1)->setCoord(pt);
    b5->getVertex(0)->setCoord(pt);

    pt = pt_centre+(b1->getVertex(1)->getCoord()-pt_centre)*m_ratio*unSurRac2;
    b1->getVertex(3)->setCoord(pt);
    b5->getVertex(1)->setCoord(pt);

    pt = pt_centre+(b3->getVertex(3)->getCoord()-pt_centre)*m_ratio*unSurRac2;
    b3->getVertex(1)->setCoord(pt);
    b5->getVertex(3)->setCoord(pt);

    pt = pt_centre+(b3->getVertex(2)->getCoord()-pt_centre)*m_ratio;
    b3->getVertex(0)->setCoord(pt);
    b2->getVertex(3)->setCoord(pt);
    b5->getVertex(2)->setCoord(pt);

    // découpage de la face
    std::vector<Edge* > splitingEdges;
    std::vector<CoFace* > cofaces;
    cofaces.push_back(b5->getFace(Block::i_max)->getCoFace(0));
    TopoHelper::splitFaces(cofaces,
    		cofaces[0]->getEdge(b5->getVertex(1), b5->getVertex(3))->getCoEdge(0),
			0.5, 0.0, false, false,
			splitingEdges,
			&getInfoCommand());

    // fusion des faces avec l'ogrid
    b1->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b5->getFace(Block::i_min), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());

    // fusion des faces autour de l'ogrid
    b1->getFace(Block::i_min)->fuse(b2->getFace(Block::j_min), &getInfoCommand());
    b2->getFace(Block::j_max)->fuse(b3->getFace(Block::i_min), &getInfoCommand());

    // les associations
    crv = curves[0];
    b1->getVertex(0)->setGeomAssociation(crv);
    b3->getVertex(2)->setGeomAssociation(crv);

    crv = curves[4];
    b1->getVertex(3)->setGeomAssociation(crv);
    crv = curves[3];
    b3->getVertex(1)->setGeomAssociation(crv);

    Geom::Surface* surf;
    surf = surfaces[1];
    b1->getVertex(2)->setGeomAssociation(surf);
    b3->getVertex(0)->setGeomAssociation(surf);

    pt = vertices[3]->getCoord();
    splitingEdges[0]->getVertex(0)->setCoord(pt);
    splitingEdges[0]->getVertex(0)->setGeomAssociation(vertices[3]);

    pt = vertices[2]->getCoord();
    splitingEdges[0]->getVertex(1)->setCoord(pt);
    splitingEdges[0]->getVertex(1)->setGeomAssociation(vertices[2]);


    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopoOGridDemiDeg()
{
    Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b2 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b3);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b3->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);

    if (curves.size() != 9){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 9 courbes géométriques mais "
                <<curves.size()<<" pour le demi cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (vertices.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 sommets géométriques mais "
                <<vertices.size()<<" pour le demi cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Utils::Math::Point pt;
    Geom::Curve* crv;
   // x min, k max
    crv = curves[2];
    crv->getPoint(0.75, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b2->getVertex(1)->setCoord(pt);

    crv->getPoint(0.25, pt, true);
    b2->getVertex(0)->setCoord(pt);
    b3->getVertex(1)->setCoord(pt);

    pt = vertices[2]->getCoord();
    b1->getVertex(1)->setCoord(pt);
    b1->getVertex(1)->setGeomAssociation(vertices[2]);

    pt = vertices[3]->getCoord();
    b3->getVertex(0)->setCoord(pt);
    b3->getVertex(0)->setGeomAssociation(vertices[3]);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.75, pt, true);
    b1->getVertex(2)->setCoord(pt);
    b2->getVertex(3)->setCoord(pt);

    crv->getPoint(0.25, pt, true);
    b2->getVertex(2)->setCoord(pt);
    b3->getVertex(3)->setCoord(pt);

    pt = vertices[0]->getCoord();
    b1->getVertex(3)->setCoord(pt);
    b1->getVertex(3)->setGeomAssociation(vertices[0]);

    pt = vertices[1]->getCoord();
    b3->getVertex(2)->setCoord(pt);
    b3->getVertex(2)->setGeomAssociation(vertices[1]);

    // dégénéréscence des blocs
    b1->degenerateFaceInEdge(Block::k_max, b1->getVertex(4), b1->getVertex(5), &getInfoCommand());
    b2->degenerateFaceInEdge(Block::k_max, b2->getVertex(4), b2->getVertex(5), &getInfoCommand());
    b3->degenerateFaceInEdge(Block::k_max, b3->getVertex(4), b3->getVertex(5), &getInfoCommand());

    // les sommets au centre de l'ogrid
    pt = vertices[5]->getCoord();
    b1->getVertex(4)->setCoord(pt);
    b2->getVertex(4)->setCoord(pt);
    b3->getVertex(4)->setCoord(pt);

    pt = vertices[4]->getCoord();
    b1->getVertex(5)->setCoord(pt);
    b2->getVertex(5)->setCoord(pt);
    b3->getVertex(5)->setCoord(pt);


    // fusion des faces autour de l'ogrid
    b1->getFace(Block::i_min)->fuse(b2->getFace(Block::i_max), &getInfoCommand());
    b2->getFace(Block::i_min)->fuse(b3->getFace(Block::i_max), &getInfoCommand());

    // les associations
    b1->getVertex(4)->setGeomAssociation(vertices[5]);
    b1->getVertex(5)->setGeomAssociation(vertices[4]);

    crv = curves[2];
    b1->getVertex(0)->setGeomAssociation(crv);
    b2->getVertex(0)->setGeomAssociation(crv);
    crv = curves[0];
    b1->getVertex(2)->setGeomAssociation(crv);
    b2->getVertex(2)->setGeomAssociation(crv);


    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopoOGridDemiNonDeg()
{
	Topo::Block* b1 = new Topo::Block(getContext(), m_ni, m_nr, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b2 = new Topo::Block(getContext(), m_nr, 2*m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b3 = new Topo::Block(getContext(), m_ni, m_nr, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b5 = new Topo::Block(getContext(), m_ni, 2*m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b3);
    addCreatedBlock(b5);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b3->setGeomAssociation(getGeomEntity());
    b5->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);

    if (curves.size() != 9){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 9 courbes géométriques mais "
                <<curves.size()<<" pour le demi cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (vertices.size() != 6){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 6 sommets géométriques mais "
                <<vertices.size()<<" pour le demi cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (surfaces.size() != 5){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 5 surfaces géométriques mais "
                <<surfaces.size()<<" pour le demi cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    Utils::Math::Point pt;
    Geom::Curve* crv;
   // x min, k max
    crv = curves[2];
    crv->getPoint(0.75, pt, true);
    b1->getVertex(4)->setCoord(pt);
    b2->getVertex(4)->setCoord(pt);

    crv->getPoint(0.25, pt, true);
    b2->getVertex(6)->setCoord(pt);
    b3->getVertex(6)->setCoord(pt);

    pt = vertices[2]->getCoord();
    b1->getVertex(5)->setCoord(pt);
    b1->getVertex(5)->setGeomAssociation(vertices[2]);

    pt = vertices[3]->getCoord();
    b3->getVertex(7)->setCoord(pt);
    b3->getVertex(7)->setGeomAssociation(vertices[3]);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.75, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b2->getVertex(0)->setCoord(pt);

    crv->getPoint(0.25, pt, true);
    b2->getVertex(2)->setCoord(pt);
    b3->getVertex(2)->setCoord(pt);

    pt = vertices[0]->getCoord();
    b1->getVertex(1)->setCoord(pt);
    b1->getVertex(1)->setGeomAssociation(vertices[0]);

    pt = vertices[1]->getCoord();
    b3->getVertex(3)->setCoord(pt);
    b3->getVertex(3)->setGeomAssociation(vertices[1]);

    // les sommets au centre de l'ogrid
    double unSurRac2 = 1/sqrt(2);
    Utils::Math::Point pt_centre = vertices[5]->getCoord();
    pt = pt_centre+(b1->getVertex(4)->getCoord()-pt_centre)*m_ratio;
    b1->getVertex(6)->setCoord(pt);
    b2->getVertex(5)->setCoord(pt);
    b5->getVertex(4)->setCoord(pt);

    pt = pt_centre+(b1->getVertex(5)->getCoord()-pt_centre)*m_ratio*unSurRac2;
    b1->getVertex(7)->setCoord(pt);
    b5->getVertex(5)->setCoord(pt);

    pt = pt_centre+(b3->getVertex(7)->getCoord()-pt_centre)*m_ratio*unSurRac2;
    b3->getVertex(5)->setCoord(pt);
    b5->getVertex(7)->setCoord(pt);

    pt = pt_centre+(b3->getVertex(6)->getCoord()-pt_centre)*m_ratio;
    b3->getVertex(4)->setCoord(pt);
    b2->getVertex(7)->setCoord(pt);
    b5->getVertex(6)->setCoord(pt);

    pt_centre = vertices[4]->getCoord();
    pt = pt_centre+(b1->getVertex(0)->getCoord()-pt_centre)*m_ratio;
    b1->getVertex(2)->setCoord(pt);
    b2->getVertex(1)->setCoord(pt);
    b5->getVertex(0)->setCoord(pt);

    pt = pt_centre+(b1->getVertex(1)->getCoord()-pt_centre)*m_ratio*unSurRac2;
    b1->getVertex(3)->setCoord(pt);
    b5->getVertex(1)->setCoord(pt);

    pt = pt_centre+(b3->getVertex(3)->getCoord()-pt_centre)*m_ratio*unSurRac2;
    b3->getVertex(1)->setCoord(pt);
    b5->getVertex(3)->setCoord(pt);

    pt = pt_centre+(b3->getVertex(2)->getCoord()-pt_centre)*m_ratio;
    b3->getVertex(0)->setCoord(pt);
    b2->getVertex(3)->setCoord(pt);
    b5->getVertex(2)->setCoord(pt);

    // découpage de la face
    std::vector<Edge* > splitingEdges;
    std::vector<CoFace* > cofaces;
    cofaces.push_back(b5->getFace(Block::i_max)->getCoFace(0));
    TopoHelper::splitFaces(cofaces,
    		cofaces[0]->getEdge(b5->getVertex(1), b5->getVertex(3))->getCoEdge(0),
			0.5, 0.0, false, false,
			splitingEdges,
			&getInfoCommand());

    // fusion des faces avec l'ogrid
    b1->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b5->getFace(Block::i_min), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());

    // fusion des faces autour de l'ogrid
    b1->getFace(Block::i_min)->fuse(b2->getFace(Block::j_min), &getInfoCommand());
    b2->getFace(Block::j_max)->fuse(b3->getFace(Block::i_min), &getInfoCommand());

    // les associations
    crv = curves[2];
    b1->getVertex(4)->setGeomAssociation(crv);
    b3->getVertex(6)->setGeomAssociation(crv);
    crv = curves[0];
    b1->getVertex(0)->setGeomAssociation(crv);
    b3->getVertex(2)->setGeomAssociation(crv);

    crv = curves[6];
    b1->getVertex(7)->setGeomAssociation(crv);
    crv = curves[7];
    b3->getVertex(5)->setGeomAssociation(crv);
    crv = curves[5];
    b1->getVertex(3)->setGeomAssociation(crv);
    crv = curves[4];
    b3->getVertex(1)->setGeomAssociation(crv);

    Geom::Surface* surf;
    surf = surfaces[2];
    b1->getVertex(6)->setGeomAssociation(surf);
    b3->getVertex(4)->setGeomAssociation(surf);
    surf = surfaces[1];
    b1->getVertex(2)->setGeomAssociation(surf);
    b3->getVertex(0)->setGeomAssociation(surf);

    pt = vertices[4]->getCoord();
    splitingEdges[0]->getVertex(0)->setCoord(pt);
    splitingEdges[0]->getVertex(0)->setGeomAssociation(vertices[4]);

    pt = vertices[5]->getCoord();
    splitingEdges[0]->getVertex(1)->setCoord(pt);
    splitingEdges[0]->getVertex(1)->setGeomAssociation(vertices[5]);


    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopoOGridPleinDegR0()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message <<"Cône avec 2 dégénerescences, configuration de maillage structuré non prévu actuellement.";
    throw TkUtil::Exception(message);
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopoOGridPleinNonDegR0()
{
	Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, m_nr, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b2 = new Topo::Block(getContext(), m_nr, 2*m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b3 = new Topo::Block(getContext(), 2*m_ni, m_nr, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b4 = new Topo::Block(getContext(), m_nr, 2*m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b5 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b3);
    addCreatedBlock(b4);
    addCreatedBlock(b5);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b3->setGeomAssociation(getGeomEntity());
    b4->setGeomAssociation(getGeomEntity());
    b5->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    b1->degenerateFaceInVertex(5, &getInfoCommand());
    b2->degenerateFaceInVertex(5, &getInfoCommand());
    b3->degenerateFaceInVertex(5, &getInfoCommand());
    b4->degenerateFaceInVertex(5, &getInfoCommand());
    b5->degenerateFaceInVertex(5, &getInfoCommand());

    std::vector<Geom::Vertex*> vertices;
    getGeomEntity()->get(vertices);
    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);

    if (vertices.size() != 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 2 sommets géométriques mais "
                <<vertices.size()<<" pour le cône avec un rayon nul "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (curves.size() != 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 2 courbes géométriques mais "
                <<curves.size()<<" pour le cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (surfaces.size() != 2){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 2 surfaces géométriques mais "
                <<surfaces.size()<<" pour le cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    Utils::Math::Point pt;
    Geom::Curve* crv;
    // x min, k max
    pt = vertices[1]->getCoord();
    b1->getVertex(4)->setCoord(pt);
    b2->getVertex(4)->setCoord(pt);
    b3->getVertex(4)->setCoord(pt);
    b4->getVertex(4)->setCoord(pt);
    b5->getVertex(4)->setCoord(pt);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.125, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b2->getVertex(0)->setCoord(pt);

    crv->getPoint(0.375, pt, true);
    b1->getVertex(1)->setCoord(pt);
    b4->getVertex(1)->setCoord(pt);

    crv->getPoint(0.625, pt, true);
    b3->getVertex(3)->setCoord(pt);
    b4->getVertex(3)->setCoord(pt);

    crv->getPoint(0.875, pt, true);
    b3->getVertex(2)->setCoord(pt);
    b2->getVertex(2)->setCoord(pt);


    // les sommets au centre de l'ogrid
    Utils::Math::Point pt_centre;

    pt_centre = (b1->getVertex(1)->getCoord()+b3->getVertex(2)->getCoord())/2;
    pt = pt_centre+(b1->getVertex(0)->getCoord()-pt_centre)*m_ratio;
    b1->getVertex(2)->setCoord(pt);
    b2->getVertex(1)->setCoord(pt);
    b5->getVertex(0)->setCoord(pt);

    pt = pt_centre+(b1->getVertex(1)->getCoord()-pt_centre)*m_ratio;
    b1->getVertex(3)->setCoord(pt);
    b4->getVertex(0)->setCoord(pt);
    b5->getVertex(1)->setCoord(pt);

    pt = pt_centre+(b3->getVertex(3)->getCoord()-pt_centre)*m_ratio;
    b3->getVertex(1)->setCoord(pt);
    b4->getVertex(2)->setCoord(pt);
    b5->getVertex(3)->setCoord(pt);

    pt = pt_centre+(b3->getVertex(2)->getCoord()-pt_centre)*m_ratio;
    b3->getVertex(0)->setCoord(pt);
    b2->getVertex(3)->setCoord(pt);
    b5->getVertex(2)->setCoord(pt);


    // fusion des faces avec l'ogrid
    b1->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b5->getFace(Block::i_min), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b4->getFace(Block::i_min)->fuse(b5->getFace(Block::i_max), &getInfoCommand());

    // fusion des faces autour de l'ogrid
    b1->getFace(Block::i_min)->fuse(b2->getFace(Block::j_min), &getInfoCommand());
    b2->getFace(Block::j_max)->fuse(b3->getFace(Block::i_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b4->getFace(Block::j_max), &getInfoCommand());
    b4->getFace(Block::j_min)->fuse(b1->getFace(Block::i_max), &getInfoCommand());

    // les associations
    b1->getVertex(4)->setGeomAssociation(vertices[1]);
    crv = curves[0];
    b1->getVertex(0)->setGeomAssociation(crv);
    b1->getVertex(1)->setGeomAssociation(crv);
    b3->getVertex(3)->setGeomAssociation(crv);
    b3->getVertex(2)->setGeomAssociation(crv);

    Geom::Surface* surf;
    surf = surfaces[1];
    b1->getVertex(2)->setGeomAssociation(surf);
    b1->getVertex(3)->setGeomAssociation(surf);
    b3->getVertex(1)->setGeomAssociation(surf);
    b3->getVertex(0)->setGeomAssociation(surf);


    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopoOGridPleinDeg()
{
    Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b2 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b3 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
    Topo::Block* b4 = new Topo::Block(getContext(), 2*m_ni, m_naxe, m_nr,
            BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b3);
    addCreatedBlock(b4);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b3->setGeomAssociation(getGeomEntity());
    b4->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications

    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);

    if (curves.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 courbes géométriques mais "
                <<curves.size()<<" pour le cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (surfaces.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 surfaces géométriques mais "
                <<surfaces.size()<<" pour le cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }

    Utils::Math::Point pt;
    Geom::Curve* crv;
    // x min, k max
    crv = curves[2];
    crv->getPoint(0.125, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b2->getVertex(1)->setCoord(pt);

    crv->getPoint(0.375, pt, true);
    b2->getVertex(0)->setCoord(pt);
    b3->getVertex(1)->setCoord(pt);

    crv->getPoint(0.625, pt, true);
    b3->getVertex(0)->setCoord(pt);
    b4->getVertex(1)->setCoord(pt);

    crv->getPoint(0.875, pt, true);
    b4->getVertex(0)->setCoord(pt);
    b1->getVertex(1)->setCoord(pt);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.125, pt, true);
    b1->getVertex(2)->setCoord(pt);
    b2->getVertex(3)->setCoord(pt);

    crv->getPoint(0.375, pt, true);
    b2->getVertex(2)->setCoord(pt);
    b3->getVertex(3)->setCoord(pt);

    crv->getPoint(0.625, pt, true);
    b3->getVertex(2)->setCoord(pt);
    b4->getVertex(3)->setCoord(pt);

    crv->getPoint(0.875, pt, true);
    b4->getVertex(2)->setCoord(pt);
    b1->getVertex(3)->setCoord(pt);

    // dégénéréscence des blocs
    b1->degenerateFaceInEdge(Block::k_max, b1->getVertex(4), b1->getVertex(5), &getInfoCommand());
    b2->degenerateFaceInEdge(Block::k_max, b2->getVertex(4), b2->getVertex(5), &getInfoCommand());
    b3->degenerateFaceInEdge(Block::k_max, b3->getVertex(4), b3->getVertex(5), &getInfoCommand());
    b4->degenerateFaceInEdge(Block::k_max, b4->getVertex(4), b4->getVertex(5), &getInfoCommand());

    // les sommets au centre de l'ogrid
    pt = (b1->getVertex(0)->getCoord()+b3->getVertex(0)->getCoord())/2;
    b1->getVertex(4)->setCoord(pt);
    b2->getVertex(4)->setCoord(pt);
    b3->getVertex(4)->setCoord(pt);
    b4->getVertex(4)->setCoord(pt);

    pt = (b1->getVertex(2)->getCoord()+b3->getVertex(2)->getCoord())/2;
    b1->getVertex(5)->setCoord(pt);
    b2->getVertex(5)->setCoord(pt);
    b3->getVertex(5)->setCoord(pt);
    b4->getVertex(5)->setCoord(pt);

    // fusion des faces autour de l'ogrid
    b1->getFace(Block::i_min)->fuse(b2->getFace(Block::i_max), &getInfoCommand());
    b2->getFace(Block::i_min)->fuse(b3->getFace(Block::i_max), &getInfoCommand());
    b3->getFace(Block::i_min)->fuse(b4->getFace(Block::i_max), &getInfoCommand());
    b4->getFace(Block::i_min)->fuse(b1->getFace(Block::i_max), &getInfoCommand());

    // les associations
    crv = curves[2];
    b1->getVertex(0)->setGeomAssociation(crv);
    b2->getVertex(0)->setGeomAssociation(crv);
    b3->getVertex(0)->setGeomAssociation(crv);
    b4->getVertex(0)->setGeomAssociation(crv);
    crv = curves[0];
    b1->getVertex(2)->setGeomAssociation(crv);
    b2->getVertex(2)->setGeomAssociation(crv);
    b3->getVertex(2)->setGeomAssociation(crv);
    b4->getVertex(2)->setGeomAssociation(crv);

    Geom::Surface* surf;
    surf = surfaces[2];
    b1->getVertex(4)->setGeomAssociation(surf);
    surf = surfaces[1];
    b1->getVertex(5)->setGeomAssociation(surf);


    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
void CommandNewTopoOGridOnGeometry::createConeTopoOGridPleinNonDeg()
{
	Topo::Block* b1 = new Topo::Block(getContext(), 2*m_ni, m_nr, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b2 = new Topo::Block(getContext(), m_nr, 2*m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b3 = new Topo::Block(getContext(), 2*m_ni, m_nr, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b4 = new Topo::Block(getContext(), m_nr, 2*m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);
	Topo::Block* b5 = new Topo::Block(getContext(), 2*m_ni, 2*m_ni, m_naxe,
			BlockMeshingProperty::directional, BlockMeshingProperty::dir_k);

    // stockage dans le manager topologiques des différentes entités (bloc et niveau inférieur)
    addCreatedBlock(b1);
    addCreatedBlock(b2);
    addCreatedBlock(b3);
    addCreatedBlock(b4);
    addCreatedBlock(b5);

    b1->setGeomAssociation(getGeomEntity());
    b2->setGeomAssociation(getGeomEntity());
    b3->setGeomAssociation(getGeomEntity());
    b4->setGeomAssociation(getGeomEntity());
    b5->setGeomAssociation(getGeomEntity());

    split(); // split à faire avant les modifications


    std::vector<Geom::Curve*> curves;
    getGeomEntity()->get(curves);
    std::vector<Geom::Surface*> surfaces;
    getGeomEntity()->get(surfaces);

    if (curves.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 courbes géométriques mais "
                <<curves.size()<<" pour le cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }
    if (surfaces.size() != 3){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message <<"CommandNewTopoOGridOnGeometry ne trouve pas 3 surfaces géométriques mais "
                <<surfaces.size()<<" pour le cône "<<getGeomEntity()->getName();
        throw TkUtil::Exception(message);
    }


    Utils::Math::Point pt;
    Geom::Curve* crv;
    // x min, k max
    crv = curves[2];
    crv->getPoint(0.125, pt, true);
    b1->getVertex(4)->setCoord(pt);
    b2->getVertex(4)->setCoord(pt);

    crv->getPoint(0.375, pt, true);
    b1->getVertex(5)->setCoord(pt);
    b4->getVertex(5)->setCoord(pt);

    crv->getPoint(0.625, pt, true);
    b3->getVertex(7)->setCoord(pt);
    b4->getVertex(7)->setCoord(pt);

    crv->getPoint(0.875, pt, true);
    b3->getVertex(6)->setCoord(pt);
    b2->getVertex(6)->setCoord(pt);

    // x max, k min
    crv = curves[0];
    crv->getPoint(0.125, pt, true);
    b1->getVertex(0)->setCoord(pt);
    b2->getVertex(0)->setCoord(pt);

    crv->getPoint(0.375, pt, true);
    b1->getVertex(1)->setCoord(pt);
    b4->getVertex(1)->setCoord(pt);

    crv->getPoint(0.625, pt, true);
    b3->getVertex(3)->setCoord(pt);
    b4->getVertex(3)->setCoord(pt);

    crv->getPoint(0.875, pt, true);
    b3->getVertex(2)->setCoord(pt);
    b2->getVertex(2)->setCoord(pt);


    // les sommets au centre de l'ogrid
    Utils::Math::Point pt_centre = (b1->getVertex(5)->getCoord()+b3->getVertex(6)->getCoord())/2;
    pt = pt_centre+(b1->getVertex(4)->getCoord()-pt_centre)*m_ratio;
    b1->getVertex(6)->setCoord(pt);
    b2->getVertex(5)->setCoord(pt);
    b5->getVertex(4)->setCoord(pt);

    pt = pt_centre+(b1->getVertex(5)->getCoord()-pt_centre)*m_ratio;
    b1->getVertex(7)->setCoord(pt);
    b4->getVertex(4)->setCoord(pt);
    b5->getVertex(5)->setCoord(pt);

    pt = pt_centre+(b3->getVertex(7)->getCoord()-pt_centre)*m_ratio;
    b3->getVertex(5)->setCoord(pt);
    b4->getVertex(6)->setCoord(pt);
    b5->getVertex(7)->setCoord(pt);

    pt = pt_centre+(b3->getVertex(6)->getCoord()-pt_centre)*m_ratio;
    b3->getVertex(4)->setCoord(pt);
    b2->getVertex(7)->setCoord(pt);
    b5->getVertex(6)->setCoord(pt);


    pt_centre = (b1->getVertex(1)->getCoord()+b3->getVertex(2)->getCoord())/2;
    pt = pt_centre+(b1->getVertex(0)->getCoord()-pt_centre)*m_ratio;
    b1->getVertex(2)->setCoord(pt);
    b2->getVertex(1)->setCoord(pt);
    b5->getVertex(0)->setCoord(pt);

    pt = pt_centre+(b1->getVertex(1)->getCoord()-pt_centre)*m_ratio;
    b1->getVertex(3)->setCoord(pt);
    b4->getVertex(0)->setCoord(pt);
    b5->getVertex(1)->setCoord(pt);

    pt = pt_centre+(b3->getVertex(3)->getCoord()-pt_centre)*m_ratio;
    b3->getVertex(1)->setCoord(pt);
    b4->getVertex(2)->setCoord(pt);
    b5->getVertex(3)->setCoord(pt);

    pt = pt_centre+(b3->getVertex(2)->getCoord()-pt_centre)*m_ratio;
    b3->getVertex(0)->setCoord(pt);
    b2->getVertex(3)->setCoord(pt);
    b5->getVertex(2)->setCoord(pt);


    // fusion des faces avec l'ogrid
    b1->getFace(Block::j_max)->fuse(b5->getFace(Block::j_min), &getInfoCommand());
    b2->getFace(Block::i_max)->fuse(b5->getFace(Block::i_min), &getInfoCommand());
    b3->getFace(Block::j_min)->fuse(b5->getFace(Block::j_max), &getInfoCommand());
    b4->getFace(Block::i_min)->fuse(b5->getFace(Block::i_max), &getInfoCommand());

    // fusion des faces autour de l'ogrid
    b1->getFace(Block::i_min)->fuse(b2->getFace(Block::j_min), &getInfoCommand());
    b2->getFace(Block::j_max)->fuse(b3->getFace(Block::i_min), &getInfoCommand());
    b3->getFace(Block::i_max)->fuse(b4->getFace(Block::j_max), &getInfoCommand());
    b4->getFace(Block::j_min)->fuse(b1->getFace(Block::i_max), &getInfoCommand());

    // les associations
    crv = curves[2];
    b1->getVertex(4)->setGeomAssociation(crv);
    b1->getVertex(5)->setGeomAssociation(crv);
    b3->getVertex(7)->setGeomAssociation(crv);
    b3->getVertex(6)->setGeomAssociation(crv);
    crv = curves[0];
    b1->getVertex(0)->setGeomAssociation(crv);
    b1->getVertex(1)->setGeomAssociation(crv);
    b3->getVertex(3)->setGeomAssociation(crv);
    b3->getVertex(2)->setGeomAssociation(crv);

    Geom::Surface* surf;
    surf = surfaces[2];
    b1->getVertex(6)->setGeomAssociation(surf);
    b1->getVertex(7)->setGeomAssociation(surf);
    b3->getVertex(5)->setGeomAssociation(surf);
    b3->getVertex(4)->setGeomAssociation(surf);
    surf = surfaces[1];
    b1->getVertex(2)->setGeomAssociation(surf);
    b1->getVertex(3)->setGeomAssociation(surf);
    b3->getVertex(1)->setGeomAssociation(surf);
    b3->getVertex(0)->setGeomAssociation(surf);


    projectEdgesOnCurves();

    projectFacesOnSurfaces();
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
