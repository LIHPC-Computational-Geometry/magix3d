/*----------------------------------------------------------------------------*/
/** \file Face.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 19/11/2010
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <cstdio>
#include <string.h>
#include <limits.h>	// UINT_MAX sur Bull
/*----------------------------------------------------------------------------*/
#include "Topo/TopoHelper.h"
#include "Topo/Face.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/TopoDisplayRepresentation.h"
#include "Topo/CommandEditTopo.h"
#include "Mesh/CommandCreateMesh.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Utils/SerializedRepresentation.h"
#include "Internal/InfoCommand.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/MemoryError.h>
#include "GMDS/IG/Node.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
const char* Face::typeNameTopoFace = "TopoFace";
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MEMORY
Face::
Face(Internal::Context& ctx,
        CoFace* coface)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoFace),
        ctx.newDisplayProperties(Utils::Entity::TopoFace))
, m_topo_property(new FaceTopoProperty())
, m_save_topo_property(0)
, m_mesh_property(0)
, m_save_mesh_property(0)
{
    if (coface == 0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Tentative de création d'une face avec coface vide", TkUtil::Charset::UTF_8));

    m_mesh_property = new FaceMeshingProperty(coface->isStructured());

    m_topo_property->getCoFaceContainer().add(coface);

    const std::vector<Vertex* > & vertices = coface->getVertices();
    m_topo_property->getVertexContainer().add(vertices);

    coface->addFace(this);
	ctx.newGraphicalRepresentation (*this);

#ifdef _DEBUG_MEMORY
	std::cout << "Face::Face("<<coface->getName()<<") de "<<getName()<<std::endl;
//	std::cout <<*this;
#endif
}
/*----------------------------------------------------------------------------*/
Face::
Face(Internal::Context& ctx,
        std::vector<CoFace* >& cofaces,
        std::vector<Vertex* >& vertices,
        bool isStr)
: TopoEntity(ctx,
        ctx.newProperty(Utils::Entity::TopoFace),
        ctx.newDisplayProperties(Utils::Entity::TopoFace))
, m_topo_property(new FaceTopoProperty())
, m_save_topo_property(0)
, m_mesh_property(new FaceMeshingProperty(isStr))
, m_save_mesh_property(0)
{
    if (cofaces.empty())
         throw TkUtil::Exception (TkUtil::UTF8String ("Tentative de création d'une face sans coface", TkUtil::Charset::UTF_8));

    m_topo_property->getCoFaceContainer().add(cofaces);
    m_topo_property->getVertexContainer().add(vertices);
    for (uint i=0; i<getNbCoFaces(); i++)
        getCoFace(i)->addFace(this);
	ctx.newGraphicalRepresentation (*this);
#ifdef _DEBUG_MEMORY
	std::cout << "Face::Face( avec "<<cofaces.size()<<" cofaces et "<<vertices.size()<<" vertices)"<<std::endl;
	std::cout << " cofaces:";
	for (uint i=0; i<cofaces.size(); i++)
	    std::cout <<" "<<cofaces[i]->getName();
	std::cout <<std::endl;
    for (uint i=0; i<vertices.size(); i++)
        std::cout <<" "<<vertices[i]->getName();
    std::cout <<std::endl;
    //std::cout <<*this;
#endif
}
/*----------------------------------------------------------------------------*/
Face::
Face(const Face& f)
: TopoEntity(f.getContext(), 0, 0)
, m_topo_property(0)
, m_save_topo_property(0)
, m_mesh_property(0)
, m_save_mesh_property(0)
{
    MGX_FORBIDDEN("Constructeur de copie pour Face");
}
/*----------------------------------------------------------------------------*/
Face::
~Face()
{
#ifdef _DEBUG
    if (m_topo_property == 0)
    	std::cerr<<"Serait-on passé deux fois dans le destructeur ? pour "<<getName()<<std::endl;
#endif

#ifdef _DEBUG_MEMORY
    std::cout<<"Face::~Face() de "<<getName()<<std::endl;
#endif

    delete m_topo_property;
    delete m_mesh_property;

#ifdef _DEBUG
    m_topo_property = 0;
    m_mesh_property = 0;

    if (m_save_topo_property)
    	std::cerr<<"La sauvegarde du FaceTopoProperty a été oubliée pour "<<getName()<<std::endl;
    if (m_save_mesh_property)
    	std::cerr<<"La sauvegarde du FaceMeshingProperty a été oubliée pour "<<getName()<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void Face::
replace(Vertex* v1, Vertex* v2, bool propagate_up, bool propagate_down, Internal::InfoCommand* icmd)
{
//    std::cout<<"Face::replace("<<v1->getName()<<", "<<v2->getName()<<") dans "<<getName()<<std::endl;
//    std::cout<<"Face initiale: "<<*this<<std::endl;

//    // on ne fait rien en cas de Face en cours de destruction
//    if (isDestroyed())
//        return;

    for (uint i=0; i<getNbVertices(); i++)
        if (v1 == getVertex(i)){
            saveFaceTopoProperty(icmd);

            // 2 cas de figure
            // soit v2 est déjà présent, donc la face commune va être dégénérée (v1 disparait)
            // soit v2 est nouveau, on remplace v1 par v2
            if (m_topo_property->getVertexContainer().find(v2)){
                if (isStructured()){
                    if (getNbVertices() == 4){
                        // il faut mettre en premier le sommet conservé
                        // et en dernier le sommet qui va disparaitre
                        _permuteToFirstAndLastVertices(v2, v1, icmd);

                        m_topo_property->getVertexContainer().resize(3);
                    }
                    else {
                        m_topo_property->getVertexContainer().remove(v1, true);
                    }
                }
                else {
                    _permuteToFirstAndLastVertices(v2, v1, icmd);
                    uint new_size = m_topo_property->getVertexContainer().getNb()-1;
                    m_topo_property->getVertexContainer().resize(new_size);
                }

            } else
                m_topo_property->getVertexContainer().set(i, v2);

        } // end if (v1 == getVertex(i))

    // transmet aux entités de niveau supérieur
    if (propagate_up)
    	for (uint i=0; i<getNbBlocks(); i++)
    		getBlock(i)->replace(v1, v2, propagate_up, propagate_down, icmd);
    if (propagate_down)
    	for (uint i=0; i<getNbCoFaces(); i++)
    		getCoFace(i)->replace(v1, v2, propagate_up, propagate_down, icmd);

    // destruction de la face si elle est dégénérée en une arête ou moins
    if (getNbVertices()<3){
        setDestroyed(true);
        if (icmd)
            icmd->addTopoInfoEntity(this,Internal::InfoCommand::DELETED);
    }

//    std::cout<<"Face finale: "<<*this<<std::endl;
}
/*----------------------------------------------------------------------------*/
void Face::
replace(CoFace* cf1, CoFace* cf2, Internal::InfoCommand* icmd)
{
    // on ne fait rien en cas de Face en cours de destruction
    if (isDestroyed())
        return;

    for (uint i=0; i<getNbCoFaces(); i++)
        if (cf1 == getCoFace(i)){
            saveFaceTopoProperty(icmd);
            m_topo_property->getCoFaceContainer().set(i, cf2);

            cf1->saveCoFaceTopoProperty(icmd);
            cf2->saveCoFaceTopoProperty(icmd);
            cf1->removeFace(this);
            cf2->addFace(this);
        }
}
/*---------------------------------------------------------------------------*/
void Face::
replace(CoEdge* e1, CoEdge* e2, Internal::InfoCommand* icmd)
{
    for (uint i=0; i<getNbCoFaces(); i++)
    	getCoFace(i)->replace(e1, e2, icmd);
}
/*----------------------------------------------------------------------------*/
void Face::
merge(Face* fa, Internal::InfoCommand* icmd)
{
    if (fa == this)
        return;

    // on remplace fa dans les blocs associées à fa
    // On travaille sur une copie car il y a une mise à jour en même temps de m_blocks
//    std::vector<Block* > blocs;
//    fa->getBlocks(blocs);
//
//    for (std::vector<Block* >::iterator iter=blocs.begin();
//            iter != blocs.end(); ++iter)
//        (*iter)->replace(fa, this, icmd);
//
//    // suppression de la face parmi la liste des faces des arêtes.
//    for (uint i=0; i<getNbEdges(); i++){
//        getEdge(i)->saveEdgeTopoProperty(icmd);
//        getEdge(i)->removeFace(fa);
//    }
//
//    fa->setDestroyed(true);
//    if (icmd)
//        icmd->addTopoInfoEntity(fa,Internal::InfoCommand::DELETED);

    MGX_NOT_YET_IMPLEMENTED("Face::merge à faire ...");

    // il faut libérer les CoFaces
//    std::vector<CoEdge* > coedges;
//    getCoEdges(coedges);
//    for (std::vector<CoEdge* >::iterator iter = coedges.begin();
//            iter != coedges.end(); ++iter){
//        (*iter)->setDestroyed(true);
//        if (icmd)
//            icmd->addTopoInfoEntity(*iter,Internal::InfoCommand::DELETED);
//        (*iter)->saveCoEdgeTopoProperty(icmd);
//        (*iter).clearDependancy();
//    }


}
/*----------------------------------------------------------------------------*/
uint Face::
getNbVertices() const
{
#ifdef _DEBUG
    if (isDestroyed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "Face::getNbVertices() pour face "<<getName()<<" DETRUITE !!!";
        log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_4));
    }
#endif
    return m_topo_property->getVertexContainer().getNb();
}
/*----------------------------------------------------------------------------*/
void Face::getAllVertices(std::vector<Vertex* >& vertices, const bool unique) const
{
    vertices.clear();
    std::list<Topo::Vertex*> l_v;
    for(uint j=0; j < getNbCoFaces();j++) {
        CoFace* coface = getCoFace(j);
        for(uint k=0; k < coface->getNbEdges();k++) {
            Edge* edge = coface->getEdge(k);
            for(uint l=0; l < edge->getNbCoEdges(); l++){
                CoEdge* coedge = edge->getCoEdge(l);

                const std::vector<Vertex* > & local_vertices = coedge->getVertices();

                l_v.insert(l_v.end(), local_vertices.begin(), local_vertices.end());
            }
        }
    }

    l_v.sort(Utils::Entity::compareEntity);
    l_v.unique();

    vertices.insert(vertices.end(),l_v.begin(),l_v.end());
}
/*----------------------------------------------------------------------------*/
Block* Face::
getOppositeBlock(Block* b) const
{
    const std::vector<Topo::Block* > & blocs = getBlocks();
    if (blocs.size() == 2){
        if (b == blocs[0])
            return blocs[1];
        else if (b == blocs[1])
            return blocs[0];
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, Face::getOppositeBlock ne trouve pas le bloc de départ", TkUtil::Charset::UTF_8));
    }
    else
        return 0;
}
/*----------------------------------------------------------------------------*/
void Face::
getRepresentation(Utils::DisplayRepresentation& dr, bool checkDestroyed) const
{
    if (dr.getDisplayType()!= Utils::DisplayRepresentation::DISPLAY_TOPO)
        throw TkUtil::Exception (TkUtil::UTF8String ("Invalid display type entity", TkUtil::Charset::UTF_8));

    if (isDestroyed() && (true == checkDestroyed)){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Interdit d'afficher une entité détruite ("<<getName()<<")";
        throw TkUtil::Exception(messErr);
    }

    TopoDisplayRepresentation* tdr =
            dynamic_cast<Topo::TopoDisplayRepresentation*>(&dr);

    CHECK_NULL_PTR_ERROR(tdr);

    std::vector<Utils::Math::Point>& points = tdr->getPoints();
    std::vector<size_t>& indicesFilaire = tdr->getCurveDiscretization();
    std::vector<size_t>& indicesSurf = tdr->getSurfaceDiscretization();

    // recherche du barycentre
    Utils::Math::Point barycentre = getBarycentre();

    // modif des coordonnées des sommets de la représentation en fonction du shrink
    points.clear();
    for (uint i=0; i<getNbVertices(); i++)
        points.push_back(barycentre + (getVertex(i)->getCoord() -  barycentre) * tdr->getShrink());

    if (tdr->hasRepresentation(Utils::DisplayRepresentation::WIRE)){
        indicesFilaire.clear();

        for (uint i=0; i<getNbVertices(); i++){
            indicesFilaire.push_back(i);
            indicesFilaire.push_back((i+1)%getNbVertices());
        }
    }
    else if (tdr->hasRepresentation(Utils::DisplayRepresentation::SOLID)) {
        if (getNbVertices() == 4){
            // représentation à l'aide de 2 triangles
            indicesSurf.push_back(0);
            indicesSurf.push_back(1);
            indicesSurf.push_back(2);

            indicesSurf.push_back(2);
            indicesSurf.push_back(3);
            indicesSurf.push_back(0);
        }
        else if (getNbVertices() == 3){
            // représentation à l'aide d'1 triangle
            indicesSurf.push_back(0);
            indicesSurf.push_back(1);
            indicesSurf.push_back(2);
        }
        else if (getNbVertices() < 3){
            // pas de représentation solide prévue
        }
        else {
            // TODO [EB] : Faire Edge::getRepresentation pour le cas non structuré (passer par un maillage minimaliste)
            MGX_NOT_YET_IMPLEMENTED("Faire Edge::getRepresentation pour le cas non structuré à plus de 4 côtés en mode solide")
        }
    }

}
/*----------------------------------------------------------------------------*/
Utils::SerializedRepresentation* Face::
getDescription (bool alsoComputed) const
{
    std::unique_ptr<Utils::SerializedRepresentation>   description (
            TopoEntity::getDescription (alsoComputed));

    // les propriétés spécifiques à ce type d'entité

    Utils::SerializedRepresentation  topoProprietes ("Propriétés topologiques", "");

#ifdef _DEBUG
    topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("En cours d'édition", isEdited()));
#endif


     topoProprietes.addProperty (
            Utils::SerializedRepresentation::Property ("Méthode maillage structurée", isStructured()));

    if (isStructured()){
         uint nbI, nbJ;
         try {
        	 getNbMeshingEdges(nbI, nbJ);

        	 topoProprietes.addProperty (
        			 Utils::SerializedRepresentation::Property ("Nombre de maille 1ère direction", (long)nbI));
        	 topoProprietes.addProperty (
        			 Utils::SerializedRepresentation::Property ("Nombre de maille 2ème direction", (long)nbJ));
         }
         catch(TkUtil::Exception& exc){
        	 topoProprietes.addProperty (
        			 Utils::SerializedRepresentation::Property ("Nombre de maille par direction", std::string("indisponible")));
         }
     }

    std::vector<CoFace* > fa;
    getCoFaces(fa);

    for (std::vector<Topo::CoFace*>::iterator iter = fa.begin( ); fa.end( )!=iter; iter++){
    	if (getRatio(*iter,0) > 1)
    		topoProprietes.addProperty (
    				Utils::SerializedRepresentation::Property (
    						std::string("ratio ") + (*iter)->getName ( ) + " dir I",  (long)getRatio(*iter,0)));
    	if (getRatio(*iter,1) > 1)
    		topoProprietes.addProperty (
    				Utils::SerializedRepresentation::Property (
    						std::string("ratio ") + (*iter)->getName ( ) + " dir J",  (long)getRatio(*iter,1)));

    }



    description->addPropertiesSet (topoProprietes);

    // les relations vers les autres types d'entités topologiques
    std::vector<Topo::Vertex* > vtx;
    getVertices(vtx);

    Utils::SerializedRepresentation  topoRelation ("Relations topologiques", "");

    Utils::SerializedRepresentation  vertices ("Sommets topologiques",
            TkUtil::NumericConversions::toStr(vtx.size()));
    for (std::vector<Topo::Vertex*>::iterator iter = vtx.begin( ); vtx.end( )!=iter; iter++)
        vertices.addProperty (
                Utils::SerializedRepresentation::Property (
                        (*iter)->getName ( ),  *(*iter)));
    topoRelation.addPropertiesSet (vertices);


    Utils::SerializedRepresentation  cofaces ("Faces communes topologiques",
            TkUtil::NumericConversions::toStr(fa.size()));
    for (std::vector<Topo::CoFace*>::iterator iter = fa.begin( ); fa.end( )!=iter; iter++)
        cofaces.addProperty (
                Utils::SerializedRepresentation::Property (
                        (*iter)->getName ( ),  *(*iter)));
    topoRelation.addPropertiesSet (cofaces);


    std::vector<Block* > bl;
    getBlocks(bl);

    Utils::SerializedRepresentation  blocks ("Blocs topologiques",
    		TkUtil::NumericConversions::toStr(bl.size()));
    for (std::vector<Topo::Block*>::iterator iter = bl.begin( ); bl.end( )!=iter; iter++)
    	blocks.addProperty (
    			Utils::SerializedRepresentation::Property (
    					(*iter)->getName ( ),  *(*iter)));
    topoRelation.addPropertiesSet (blocks);


    description->addPropertiesSet (topoRelation);


    return description.release ( );
}
/*----------------------------------------------------------------------------*/
bool Face::
isStructured() const
{
    return m_mesh_property->isStructured();
}
/*----------------------------------------------------------------------------*/
bool Face::
structurable()
{
//   if (!isStructured()) {
//       // il faut que les CoFaces le soit
//       bool res = true;
//       for(uint i=0; i < getNbCoFaces();i++)
//           res = res && getCoFace(i)->structurable();
//       return res;
//   }
//   return true;
    if (getNbCoFaces() == 1)
        return getCoFace(0)->structurable();
    else {
        MGX_NOT_YET_IMPLEMENTED("Face structurable à implémenter")
    }

    return false;
}
/*----------------------------------------------------------------------------*/
bool Face::
unstructurable()
{
    // il faut que toutes les faces communes soient unstructurable
    for(uint i=0; i < getNbCoFaces();i++)
        if (!getCoFace(i)->unstructurable())
            return false;
    return true;
}
/*----------------------------------------------------------------------------*/
void Face::
structure(Internal::InfoCommand* icmd)
{
    if (!structurable())
        throw TkUtil::Exception (TkUtil::UTF8String ("Structuration d'une Face impossible", TkUtil::Charset::UTF_8));

    for(uint i=0; i < getNbCoFaces();i++)
        getCoFace(i)->structure(icmd);

    m_mesh_property->setStructured(true);
}
/*----------------------------------------------------------------------------*/
void Face::
unstructure(Internal::InfoCommand* icmd)
{
    if (!unstructurable())
       throw TkUtil::Exception (TkUtil::UTF8String ("Destructuration d'une Face impossible", TkUtil::Charset::UTF_8));

    for(uint i=0; i < getNbCoFaces();i++)
        getCoFace(i)->unstructure(icmd);

    m_mesh_property->setStructured(false);
}
/*----------------------------------------------------------------------------*/
void Face::setStructured(Internal::InfoCommand* icmd, bool str)
{
    if (str)
        throw TkUtil::Exception (TkUtil::UTF8String ("Structuration d'une Face commune non prévue", TkUtil::Charset::UTF_8));

    if (isStructured()){
        saveFaceMeshingProperty(icmd);
        m_mesh_property->setStructured(str);
        std::vector<Block*> blocs;
        getBlocks(blocs);
        for (std::vector<Block*>::iterator iter = blocs.begin(); iter != blocs.end(); ++iter)
            (*iter)->setStructured(icmd, false);
    }
}
/*----------------------------------------------------------------------------*/
void Face::
check() const
{
#ifdef _DEBUG2
	std::cout<<"Face::check() pour face "<<*this<<std::endl;
#endif

   if (isDestroyed()){
        std::cerr<<"Face::check() pour la face "<<getName()<<std::endl;
        throw TkUtil::Exception (TkUtil::UTF8String ("On utilise une face détruite", TkUtil::Charset::UTF_8));
    }

   for(uint i=0; i < getNbCoFaces();i++)
      getCoFace(i)->check();

    if (isStructured()){
        // on vérifie que les côtés opposées ont le même nombre de bras

        // recherche des arêtes communes pour chacune des directions
        std::vector<CoEdge* > coedges_dir[2];

        getOrientedCoEdges(coedges_dir[0], coedges_dir[1]);

        // on marque les arêtes et les faces communes de la face à 1
        std::map<Edge*, uint> filtre_edges;
        std::map<CoFace*, uint> filtre_cofaces;
        for(uint i=0; i < getNbCoFaces(); i++){
            CoFace* coface = getCoFace(i);
            filtre_cofaces[coface] = 1;
            for(uint j=0; j < coface->getNbEdges(); j++){
                Edge* edge = coface->getEdge(j);
                filtre_edges[edge] = 1;
            } // for(uint j=0; j < coface->getNbEdges(); j++)
        } // for(uint i=0; i < getNbCoFaces(); i++)

        // comparaison entre les côtés 0 et 2
        {
            uint nb1, nb2;
            std::vector<CoEdge* > coedges_cote;
            if (TopoHelper::getCoEdgesBetweenVertices(getVertex(1), getVertex(0), coedges_dir[1], coedges_cote))
                // à partir des ces arêtes commune on recherche les arêtes de la face
                // (ce qui permet d'avoir un nombre de bras tenant compte de la semi-conformité)
                nb1 = _getNbMeshingEdges(filtre_edges, filtre_cofaces, coedges_cote,
                        getVertex(1), getVertex(0));
            else {
                std::cerr<<"Face::check() pour "<<*this<<std::endl;
                throw TkUtil::Exception (TkUtil::UTF8String ("Face::getNbMeshingEdges ne trouve pas les arêtes communes pour le côté 0", TkUtil::Charset::UTF_8));
            }

            if (TopoHelper::getCoEdgesBetweenVertices(getVertex(2), getVertex(getNbVertices()==4?3:0), coedges_dir[1], coedges_cote))
                nb2 = _getNbMeshingEdges(filtre_edges, filtre_cofaces, coedges_cote,
                        getVertex(2), getVertex(getNbVertices()==4?3:0));
            else {
                std::cerr<<"Face::check() pour "<<*this<<std::endl;
                throw TkUtil::Exception (TkUtil::UTF8String ("Face::getNbMeshingEdges ne trouve pas les arêtes communes pour le côté 2", TkUtil::Charset::UTF_8));
            }

            if (nb1 != nb2){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Erreur avec la face structurée "<<getName()
                       <<", les côtés 0 et 2 n'ont pas le même nombre de bras: "
                       << (short)nb1
                       <<" et "<< (short)nb2;
                throw TkUtil::Exception(messErr);
            }
        }

        // comparaison entre les côtés 1 et 3
        if (getNbVertices() == 4){
            uint nb1, nb2;
            std::vector<CoEdge* > coedges_cote;
            if (TopoHelper::getCoEdgesBetweenVertices(getVertex(1), getVertex(2), coedges_dir[0], coedges_cote))
                // à partir des ces arêtes commune on recherche les arêtes de la face
                // (ce qui permet d'avoir un nombre de bras tenant compte de la semi-conformité)
                nb1 = _getNbMeshingEdges(filtre_edges, filtre_cofaces, coedges_cote,
                        getVertex(1), getVertex(2));
            else {
                std::cerr<<"Face::check() pour "<<*this<<std::endl;
                throw TkUtil::Exception (TkUtil::UTF8String ("Face::getNbMeshingEdges ne trouve pas les arêtes communes pour le côté 1", TkUtil::Charset::UTF_8));
            }

            if (TopoHelper::getCoEdgesBetweenVertices(getVertex(0), getVertex(3), coedges_dir[0], coedges_cote))
                nb2 = _getNbMeshingEdges(filtre_edges, filtre_cofaces, coedges_cote,
                        getVertex(0), getVertex(3));
            else {
                std::cerr<<"Face::check() pour "<<*this<<std::endl;
                throw TkUtil::Exception (TkUtil::UTF8String ("Face::getNbMeshingEdges ne trouve pas les arêtes communes pour le côté 3", TkUtil::Charset::UTF_8));
            }

            if (nb1 != nb2){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Erreur avec la face structurée "<<getName()
                       <<", les côtés 1 et 3 n'ont pas le même nombre de bras: "
                       << (short)nb1
                       <<" et "<< (short)nb2;
                throw TkUtil::Exception(messErr);
            }
        }

    } // end if (isStructured())

     for (uint i=0; i<getNbBlocks(); i++)
         if (getBlock(i)->isDestroyed()){
             std::cerr<<"Face::check() pour la face "<<getName()<<std::endl;
             throw TkUtil::Exception (TkUtil::UTF8String ("Une face pointe sur un bloc détruit", TkUtil::Charset::UTF_8));
         }

}
/*----------------------------------------------------------------------------*/
bool Face::isA(const std::string& name)
{
    return (name.compare(0,getTinyName().size(),getTinyName()) == 0);
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String & operator << (TkUtil::UTF8String & o, const Face & f)
{
    o << " " << f.getName() << " (uniqueId "<<f.getUniqueId()<<")";

    if (f.isStructured()){
        uint nbI, nbJ;
        f.getNbMeshingEdges(nbI, nbJ, true);
        o << " (structurée "<<(short)nbI<<"x"<<(short)nbJ<<") ";
    } else {
        o << " (non structurée)";
    }

    o << (f.isDestroyed()?" (DETRUITE)":"")
      << (f.isEdited()?" (EN COURS D'EDITION)":"");

    o << " avec "<<(short)f.getNbCoFaces()<<" faces communes:\n";

    for (uint i=0; i<f.getNbCoFaces();i++)
        o <<" "<<f.getCoFace(i)->getName()<<"\n";
//      o <<*f.getCoFace(i)<<"\n";
    o << "\n";

    o << " " << f.getName()<< " face avec "<<(short)f.getNbVertices()<<" sommets:\n";
    const std::vector<Topo::Vertex* > & vertices = f.getVertices();
    for (uint i=0; i<vertices.size();i++)
        o <<"   "<<*vertices[i]<<"\n";

    const std::vector<Topo::Block* > & blocs = f.getBlocks();
    if (blocs.empty()){
        o << "    cette face ne pointe sur aucun bloc.";
    }
    else {
        o << "   " << (blocs.size()==1?"cette face pointe sur le bloc :":"cette face pointe sur les blocs :");
        for (uint i=0; i<blocs.size();i++){
            o << " " << blocs[i]->getName();
            if (blocs[i]->isDestroyed())
                o << "  [DETRUIT] ";
        }
    }
    o << "\n";
    return o;
}
/*----------------------------------------------------------------------------*/
std::ostream & operator << (std::ostream & o, const Face & f)
{
	TkUtil::UTF8String	us (TkUtil::Charset::UTF_8);
    us << f;
    o << us;
    return o;
}
/*----------------------------------------------------------------------------*/
Topo::TopoInfo Face::getInfos() const
{
	Topo::TopoInfo infos;
	infos.name = getName();
	infos.dimension = getDim();
	getVertices(infos.incident_vertices);
	getCoEdges(infos.incident_coedges);
	getCoFaces(infos.incident_cofaces);
	getBlocks(infos.incident_blocks);
	infos.geom_entity = getGeomAssociation();
	return infos;
}
/*----------------------------------------------------------------------------*/
void Face::
getNodes(Vertex* sommet1, Vertex* sommet2, Vertex* sommet3, Vertex* sommet4,
        std::vector<gmds::Node> &vectNd)
{
	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 << "Face::getNodes("
            <<sommet1->getName()<<","
            <<sommet2->getName()<<","
            <<sommet3->getName()<<","
            <<sommet4->getName()<<") pour la face "
            <<getName()<<"\n";
    log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));
    //std::cout<<*this<<std::endl;

    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Face::getNodes n'est pas possible avec une face non structurée", TkUtil::Charset::UTF_8));

    // pour les autres cas, il faut retrouver la structuration de la face
    // elle-même composée de faces communes

    std::vector<CoEdge* > iCoedges;
    std::vector<CoEdge* > jCoedges;

    getOrientedCoEdges(iCoedges, jCoedges);

    uint iLocDir = TopoHelper::getInternalDir(sommet2, sommet3, iCoedges, jCoedges);
    uint jLocDir = TopoHelper::getInternalDir(sommet2, sommet1, iCoedges, jCoedges);

    if (iLocDir == jLocDir)
        throw TkUtil::Exception (TkUtil::UTF8String ("Face::getNodes n'arrive pas à trouver les directions / sommets", TkUtil::Charset::UTF_8));
    else if (iLocDir == 2)
        iLocDir = ((jLocDir == 0)? 1:0);
    else if (jLocDir == 2)
        jLocDir = ((iLocDir == 0)? 1:0);

    // on calcul les nombres de bras par direction
    // (avant de permuter, car pas compatible avec _getNbMeshingEdges)
    uint nbI, nbJ;
    _getNbMeshingEdges(nbI, nbJ, iCoedges, jCoedges);

    // on aura besoin de tenir compte de cette permutation pour les ratios / CoFaces
    bool permIJ_face = false;

    // on choisi iCoedges et jCoedges de tel manières que (sommet2, sommet3) soit sur iCoedges
    if (iLocDir == 1){
        TopoHelper::permuteVector(iCoedges, jCoedges);
        permIJ_face = true;
        uint nb_tmp = nbI;
        nbI = nbJ;
        nbJ = nb_tmp;
    }

    uint nbNoeudsI = nbI+1;
    uint nbNoeudsJ = nbJ+1;


    vectNd.resize(nbNoeudsI*nbNoeudsJ);
#ifdef _DEBUG2
    std::cout<<"nbI = "<<nbI<<std::endl;
    std::cout<<"nbJ = "<<nbJ<<std::endl;
#endif
#ifdef _DEBUG
    for (int i=0; i<nbNoeudsI*nbNoeudsJ; i++)
        vectNd[i] = gmds::Node();
#endif
    // filtre pour marquer/retrouver les CoFaces dans la Face
    std::map<CoFace*, uint> filtre_cofaces;
    for(uint i=0; i < getNbCoFaces(); i++)
        filtre_cofaces[getCoFace(i)] = 1;

    // lien entre les sommets et la position dans le tableau vectNd
    std::map<Vertex*, uint> vertex2indVectNd;
    vertex2indVectNd[sommet2] = 0;
    //        std::cout<<"vertex2indVectNd["<<sommet2->getName()<<"] = 0"<<std::endl;

    // on renseigne vertex2indVectNd pour tous les sommets sur les arêtes
    // entre sommet2 et sommet3, et entre sommet2 et sommet1
    for (uint dir=0; dir<2; dir++){
        std::vector<CoEdge* > coedges_between;
        if (dir==0)
            TopoHelper::getCoEdgesBetweenVertices(sommet2, sommet3, iCoedges, coedges_between);
        else
            TopoHelper::getCoEdgesBetweenVertices(sommet2, sommet1, jCoedges, coedges_between);

        for (std::vector<CoEdge* >::iterator iter = coedges_between.begin();
                iter != coedges_between.end(); ++iter){
            CoEdge* coedge = *iter;

            // on recherche la coface qui utilise cette CoEdge
            CoFace* coface_select = 0;
            std::vector<CoFace* > loc_cofaces;
            coedge->getCoFaces(loc_cofaces);
            for (std::vector<CoFace* >::iterator iter4 = loc_cofaces.begin();
                    iter4 != loc_cofaces.end(); ++iter4)
                if (filtre_cofaces[*iter4] == 1)
                    coface_select = (*iter4);
            if (0 == coface_select)
                throw TkUtil::Exception (TkUtil::UTF8String ("Face::getNodes n'arrive à retrouver la CoFace interne à la Face à partir d'une CoEdge", TkUtil::Charset::UTF_8));

            // recherche de l'Edge de cette CoFace qui utilise cette CoEdge
            Edge* edge = coface_select->getEdgeContaining(coedge);

            uint id0 = vertex2indVectNd[coedge->getVertex(0)];
            uint id1 = vertex2indVectNd[coedge->getVertex(1)];
            uint inc = coedge->getNbMeshingEdges() / edge->getRatio(coedge)
                                / m_mesh_property->getRatio(coface_select, (dir + permIJ_face)%2);

            if (dir==1)
                inc *= nbNoeudsI;

            if (id0>id1 || sommet2==coedge->getVertex(0)){
                vertex2indVectNd[coedge->getVertex(1)] = id0+inc;
                //                    std::cout<<"vertex2indVectNd["<<coedge->getVertex(1)->getName()<<"] = "<<id0+inc<<std::endl;

            }
            else {
                vertex2indVectNd[coedge->getVertex(0)] = id1+inc;
                //                    std::cout<<"vertex2indVectNd["<<coedge->getVertex(0)->getName()<<"] = "<<id1+inc<<std::endl;
            }
        } // for iter = coedges_between.begin()
    } // end for (uint dir=0; dir<2; dir++)

    // boucle autant de fois que de CoFace
    // filtre pour ne prendre les CoFaces qu'une fois
    filtre_cofaces.clear();
    for (uint i=0; i<getNbCoFaces(); i++){
        // on cherche une coface non vue dont 3 des sommets ont un vertex2indVectNd de positionné
        CoFace* coface_select = 0;
        for (uint j=0; j<getNbCoFaces(); j++){
            CoFace* coface = getCoFace(j);
            uint nbIdPos = 0;
            if (filtre_cofaces[coface] == 0){
                for (uint k=0; k<coface->getNbVertices(); k++){
                    Vertex* vk = coface->getVertex(k);
                    if (sommet2==vk || vertex2indVectNd[vk]>0)
                        nbIdPos++;
                } // end for k<coface->getNbVertices()
                if (nbIdPos>=3)
                    coface_select = coface;
            } // end if (filtre_cofaces[getFace(j)] == 0)
        } // end for (uint j=0; j<getNbCoFaces(); j++)

        if (0 == coface_select)
            throw TkUtil::Exception (TkUtil::UTF8String ("Face::getNodes ne trouve pas de face commune avec 3 sommets déjà vus", TkUtil::Charset::UTF_8));

        //            std::cout<<"coface_select : "<<coface_select->getName()<<std::endl;
        filtre_cofaces[coface_select] = 1;

        // recherche l'indice du sommet ayant le plus petit indice dans vertex2indVectNd
        // équivalent local de sommet2
        uint idSommet2 = 5;
        uint indMin = UINT_MAX;
        for (uint k=0; k<coface_select->getNbVertices(); k++){
            Vertex* vk = coface_select->getVertex(k);
            if (sommet2==vk || (vertex2indVectNd[vk] != 0 && vertex2indVectNd[vk]<indMin)){
                indMin = vertex2indVectNd[vk];
                idSommet2 = k;
            }
        }
        uint nbSom = coface_select->getNbVertices();
        Vertex* sommetLoc1 = coface_select->getVertex((idSommet2-1+nbSom)%nbSom);
        Vertex* sommetLoc2 = coface_select->getVertex(idSommet2);
        Vertex* sommetLoc3 = coface_select->getVertex((idSommet2+1)%nbSom);
        Vertex* sommetLoc4 = (nbSom==4?coface_select->getVertex((idSommet2+2)%nbSom):0);
        //            std::cout<<"recherche du sens\n";
        // recherche le sens / sens dans la Face
        iLocDir = TopoHelper::getInternalDir(sommetLoc2, sommetLoc3, iCoedges, jCoedges);
        jLocDir = TopoHelper::getInternalDir(sommetLoc2, sommetLoc1, iCoedges, jCoedges);

        if (iLocDir == jLocDir)
            throw TkUtil::Exception (TkUtil::UTF8String ("Face::getNodes n'arrive pas à trouver les directions / sommets locaux", TkUtil::Charset::UTF_8));
        else if (iLocDir == 2)
            iLocDir = ((jLocDir == 0)? 1:0);
        else if (jLocDir == 2)
            jLocDir = ((iLocDir == 0)? 1:0);

        // on mérorise le fait d'avoir une permutation de la CoFace
        bool permIJ_coface = false;

        // on se met dans le même repère que ce qui est demandé à getNodes
        if (iLocDir == 1){
            Vertex* sommetTmp = sommetLoc1;
            sommetLoc1 = sommetLoc3;
            sommetLoc3 = sommetTmp;
            permIJ_coface = true;
        }
#ifdef _DEBUG2
        std::cout<<"sommetLoc1 : "<<sommetLoc1->getName()<<std::endl;
        std::cout<<"sommetLoc2 : "<<sommetLoc2->getName()<<std::endl;
        std::cout<<"sommetLoc3 : "<<sommetLoc3->getName()<<std::endl;
        std::cout<<"sommetLoc4 : "<<(sommetLoc4?sommetLoc4->getName():"nul")<<std::endl;
#endif

        // recherche des noeuds de la face communes que l'on copie dans le conteneur pour cette Face
        std::vector<gmds::Node> nodes;

        // on renseigne vertex2indVectNd pour tous les sommets locaux sur les arêtes de la face commune
        // entre sommetLoc1 et sommetLoc4, et entre sommetLoc3 et sommetLoc4
        // on ne fait rien pour les cofaces dégénérées
        if (sommetLoc4){
            for (uint dir=0; dir<2; dir++){
                std::vector<CoEdge* > coedges_between;
                // Edge pour le côté auquel on s'intéresse
                Edge* edge;

                if (dir==0){
                    TopoHelper::getCoEdgesBetweenVertices(sommetLoc1, sommetLoc4, iCoedges, coedges_between);
                    edge = coface_select->getEdge(sommetLoc1, sommetLoc4);
                }
                else {
                    TopoHelper::getCoEdgesBetweenVertices(sommetLoc3, sommetLoc4, jCoedges, coedges_between);
                    edge = coface_select->getEdge(sommetLoc3, sommetLoc4);
                }

                for (std::vector<CoEdge* >::iterator iter = coedges_between.begin();
                        iter != coedges_between.end(); ++iter){
                    CoEdge* coedge = *iter;

                    uint id0 = vertex2indVectNd[coedge->getVertex(0)];
                    uint id1 = vertex2indVectNd[coedge->getVertex(1)];
                    uint inc = coedge->getNbMeshingEdges() / edge->getRatio(coedge)
                                        / m_mesh_property->getRatio(coface_select, (dir + permIJ_face)%2);

                    if (dir==1)
                        inc *= nbNoeudsI;

                    if (id0>id1 && id1 == 0){
                        vertex2indVectNd[coedge->getVertex(1)] = id0+inc;
                        //                            std::cout<<"vertex2indVectNd["<<coedge->getVertex(1)->getName()<<"] = "<<id0+inc<<std::endl;
                    }
                    else if (id0 == 0){
                        vertex2indVectNd[coedge->getVertex(0)] = id1+inc;
                        //                            std::cout<<"vertex2indVectNd["<<coedge->getVertex(0)->getName()<<"] = "<<id1+inc<<std::endl;
                    }
                } // for iter = coedges_between.begin()
            } // end for (uint dir=0; dir<2; dir++)

            coface_select->getNodes(sommetLoc1, sommetLoc2, sommetLoc3, sommetLoc4, nodes);
        } // end if (sommetLoc4)
        else {
            // cas avec coface dégénérée
            coface_select->getNodes(sommetLoc1, sommetLoc2, sommetLoc3, sommetLoc1, nodes);

        }

        // incrément de 1 et plus si le ratio est plus grand
        uint incI = m_mesh_property->getRatio(coface_select, (0 + permIJ_face)%2);
        uint incJ = m_mesh_property->getRatio(coface_select, (1 + permIJ_face)%2);

        uint nbNoeudsILoc = vertex2indVectNd[sommetLoc3] - vertex2indVectNd[sommetLoc2] + 1;
        uint nbNoeudsJLoc = (vertex2indVectNd[sommetLoc1] - vertex2indVectNd[sommetLoc2] + nbNoeudsI-1)/nbNoeudsI + 1;

#ifdef _DEBUG2
        std::cout<<"vertex2indVectNd[sommetLoc3] = "<<vertex2indVectNd[sommetLoc3]<<std::endl;
        std::cout<<"vertex2indVectNd[sommetLoc2] = "<<vertex2indVectNd[sommetLoc2]<<std::endl;
        std::cout<<"vertex2indVectNd[sommetLoc1] = "<<vertex2indVectNd[sommetLoc1]<<std::endl;
        std::cout<<"nbNoeudsI = "<<nbNoeudsI<<std::endl;
        std::cout<<"nbNoeudsILoc = "<<nbNoeudsILoc<<std::endl;
        std::cout<<"nbNoeudsJLoc = "<<nbNoeudsJLoc<<std::endl;
        std::cout<<"incI = "<<incI<<std::endl;
        std::cout<<"incJ = "<<incJ<<std::endl;
#endif

        uint nbNoeudsCoFaceI = (nbNoeudsILoc-1)*incI+1;

        uint idDep = vertex2indVectNd[sommetLoc2];
        for (uint j=0; j<nbNoeudsJLoc; j++)
            for (uint i=0; i<nbNoeudsILoc; i++){
                vectNd[idDep+i+j*nbNoeudsI] = nodes[i*incI+j*incJ*nbNoeudsCoFaceI];
#ifdef _DEBUG2
                std::cout<<"vectNd["<<idDep+i+j*nbNoeudsI
                		 <<"] = nodes["<<i*incI+j*incJ*nbNoeudsCoFaceI<<"] : "
                		 << vectNd[idDep+i+j*nbNoeudsI]->getX()<<","
                		 << vectNd[idDep+i+j*nbNoeudsI]->getY()<<","
                		 << vectNd[idDep+i+j*nbNoeudsI]->getZ()<<","
                		 << std::endl;
#endif
            }

    } // end for (uint i=0; i<getNbCoFaces(); i++)

#ifdef _DEBUG
        uint nbError = 0;
        for (int i=0; i<nbNoeudsI*nbNoeudsJ; i++)
            if (vectNd[i].getID() == gmds::NullID) {
                std::cout<<"vectNd["<<i<<"] non renseigné !\n";
                nbError++;
            }
        if (nbError){
            std::cout<<message1<<std::endl;
            std::cout<<"nbNoeudsI = "<<nbNoeudsI<<std::endl;
            std::cout<<"nbNoeudsJ = "<<nbNoeudsJ<<std::endl;
            std::cout<<"nbError = "<<nbError<<std::endl;
            std::cout<<*this;
        }
#endif

}
/*----------------------------------------------------------------------------*/
void Face::
getNodes(uint decalage,
        bool sens_directe,
        std::vector<gmds::Node> &vectNd)
{
	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 << "Face::getNodes(decalage: "
            <<(short)decalage<<","
            <<(sens_directe?"sens normal":"sens inverse")
            <<", vectNd) pour la face "
            <<getName()<<"\n";
    log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));

    // il n'est prévu de passer par cette fonction que pour le cas dégénéré en 3 sommets
    // avec une seule coface

    if (!(sens_directe && decalage == 0) && !((!sens_directe) && decalage == 3))
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, il n'est pas prévu de faire appel à Face::getNodes pour ce decalage et ce sens", TkUtil::Charset::UTF_8));

    if (getNbCoFaces() != 1){
        // pour le cas avec plusieurs CoFaces, on traite cela comme avec le cas général
        // et on ne traite pour le moment que 2 cas
        if (sens_directe) // && decalage == 0
            getNodes(getVertex(0), getVertex(1), getVertex(2), getVertex(0), vectNd);
        else
            getNodes(getVertex(0), getVertex(2), getVertex(1), getVertex(0), vectNd);
    }
    else {
        if (getNbVertices() != 3)
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, il n'est pas prévu de faire appel à Face::getNodes(decalage, ...) pour une face non dégénérée en un triangle", TkUtil::Charset::UTF_8));

        // il faut vérifier que les sommets sont dans le même ordre
        CoFace* coface = getCoFace(0);
        if (coface->getNbVertices() != 3)
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, la face commune n'est pas dégénérée", TkUtil::Charset::UTF_8));

        // test sur le sommet à la dégénérescence
        if (getVertex(0) != coface->getVertex(0))
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, la face commune n'a pas le même premier sommet", TkUtil::Charset::UTF_8));

        if (getVertex(1) == coface->getVertex(1) && getVertex(2) == coface->getVertex(2))
            coface->getNodes(decalage, sens_directe, vectNd);
        else if (getVertex(2) == coface->getVertex(1) && getVertex(1) == coface->getVertex(2))
            coface->getNodes((sens_directe?3:0), !sens_directe, vectNd);
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, la face commune n'a pas les mêmes sommets que la face", TkUtil::Charset::UTF_8));

    } // end else / if (getNbCoFaces() != 1)
}
/*----------------------------------------------------------------------------*/
void Face::
fuse(Face* face_B,
        Internal::InfoCommand* icmd)
{
    // recherche des couples de sommets confondus
    const std::vector<Vertex* > & vertices_A = getVertices();
    const std::vector<Vertex* > & vertices_B = face_B->getVertices();
    std::map<Topo::Vertex*, Topo::Vertex*> corr_vertex_A_B;

    Vertex::findNearlyVertices(vertices_A, vertices_B, corr_vertex_A_B);

    std::vector<Vertex* > vertices_B_new;
    for (std::vector<Vertex* >::const_iterator iter = vertices_A.begin();
            iter != vertices_A.end(); ++iter){
        vertices_B_new.push_back(corr_vertex_A_B[*iter]);
    }

    // appel à la fusion des faces
    fuse(face_B, vertices_A, vertices_B_new, icmd);
}
/*----------------------------------------------------------------------------*/
void Face::
fuse(Face* face_B,
        const std::vector<Vertex* >& l_sommets_A,
        const std::vector<Vertex* >& l_sommets_B,
        Internal::InfoCommand* icmd)
{
    if (getNbCoFaces() != 1 || face_B->getNbCoFaces() != 1)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, il n'est pas prévu de faire appel à Face::fuse pour une face composée", TkUtil::Charset::UTF_8));

    getCoFace(0)->fuse(face_B->getCoFace(0), l_sommets_A, l_sommets_B, icmd);

    // TODO [EB] traiter le cas avec de multiples faces communes
}
/*----------------------------------------------------------------------------*/
void Face::
free(Internal::InfoCommand* icmd)
{
#ifdef _DEBUG2
    std::cout<<"Face::free() pour "<<getName()<<std::endl;
#endif

    saveFaceTopoProperty(icmd);

    // on retire la relation avec la géométrie
    if (icmd && getGeomAssociation()
             && icmd->getTopoInfoEntity()[this] != Internal::InfoCommand::NONE){
        saveTopoProperty();
        setGeomAssociation(0);
    }

    // on met au courant la commande de la destruction
    if (icmd)
        icmd->addTopoInfoEntity(this,Internal::InfoCommand::DELETED);

    setDestroyed(true);

    // on laisse les relation du bloc vers cette face

    // suppression des relation remontante des faces communes vers cette face
    for (uint i=0; i<getNbCoFaces(); i++) {
        getCoFace(i)->saveCoFaceTopoProperty(icmd);
        getCoFace(i)->removeFace(this, true);
    }

    clearDependancy();
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_SPLIT
void Face::
split(std::vector<Edge* > & splitingEdges,
        Face* & face1,
        Face* & face2,
        Internal::InfoCommand* icmd)
{
#ifdef _DEBUG_SPLIT
    std::cout<<"Face::split() pour la face : "<<*this;
    std::cout<<"On dispose des arêtes suivantes pour la séparation:";
    for (std::vector<Edge* >::iterator iter = splitingEdges.begin();
            iter != splitingEdges.end(); ++iter)
        std::cout<<" "<<(*iter)->getName();
    std::cout<<std::endl;
#endif

    // on marque les CoFaces et les Edges (communes) dans la face
    std::map<CoEdge*, uint> filtre_coedges;
    std::map<CoFace*, uint> filtre_cofaces;
    std::map<Vertex*, uint> filtre_vertex;
    for(uint i=0; i < getNbCoFaces(); i++){
        CoFace* coface = getCoFace(i);
        filtre_cofaces[coface] = 1;
        std::vector<CoEdge* > coedges;
        coface->getCoEdges(coedges);
        for(uint j=0; j < coedges.size(); j++)
            filtre_coedges[coedges[j]] = 1;

    } // for(uint i=0; i < getNbCoFaces(); i++)

    // on marque les CoEdges qui délimitent la coupe (comme étant en dehors de la face)
    // on marque égallement ces sommets
    for(std::vector<Edge* >::iterator iter = splitingEdges.begin();
            iter != splitingEdges.end(); ++iter){
        Edge* edge = *iter;

        std::vector<CoEdge* > coedges;
        edge->getCoEdges(coedges);
        for(uint j=0; j < coedges.size(); j++)
        	filtre_coedges[coedges[j]] = 2;

        filtre_vertex[edge->getVertex(0)] = 2;
        filtre_vertex[edge->getVertex(1)] = 2;
    }

    // on part d'une CoFace reliée au sommet 1 de la face
    CoFace* coface_dep = 0;
    for (uint i=0; i<getVertex(1)->getNbCoEdges(); i++){
    	CoEdge* coedge = getVertex(1)->getCoEdge(i);
    	if (1 == filtre_coedges[coedge]){
    		std::vector<Topo::CoFace* > cofaces;
    		coedge->getCoFaces(cofaces);
    		for (uint k=0; k<cofaces.size(); k++){
    			CoFace* coface = cofaces[k];
    			if (1 == filtre_cofaces[coface])
    				coface_dep = coface;
    		}
    	}
    }

    if (0 == coface_dep)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Face::split ne trouve pas de face commune pour démarer", TkUtil::Charset::UTF_8));

    // on fait un parcours CoFace vers CoFace en utilisant seulement les CoEdges marquées dans la face
    // Ce premier groupe constituera la première face, le reste la seconde
    std::vector<CoFace* > cofaces_dep, cofaces_suiv;
    cofaces_dep.push_back(coface_dep);
    filtre_cofaces[coface_dep] = 2;

    while (!cofaces_dep.empty()) {

        for (std::vector<CoFace* >::iterator iter1 = cofaces_dep.begin();
                iter1 != cofaces_dep.end(); ++iter1){
            CoFace* coface = *iter1;

            std::vector<CoEdge* > coedges;
            coface->getCoEdges(coedges);

            for (std::vector<CoEdge* >::iterator iter2 = coedges.begin();
            		iter2 != coedges.end(); ++iter2){
            	if (filtre_coedges[*iter2] == 1){
            		std::vector<CoFace* > loc_cofaces;
            		(*iter2)->getCoFaces(loc_cofaces);
            		for (std::vector<CoFace* >::iterator iter4 = loc_cofaces.begin();
            				iter4 != loc_cofaces.end(); ++iter4){
            			if (filtre_cofaces[*iter4] == 1){
            				cofaces_suiv.push_back(*iter4);
            				filtre_cofaces[*iter4] = 2;
            			}
            		}
            	}
            } // for iter2 = coedges.begin()
        } // for iter1 = cofaces_dep.begin()

        cofaces_dep = cofaces_suiv;
        cofaces_suiv.clear();

    } // while (!cofaces_dep.empty())

    // les 2 groupes de CoFaces
    std::list<CoFace* > cofaces_1, cofaces_2;
    for (std::map<CoFace*, uint>::iterator iter = filtre_cofaces.begin();
            iter != filtre_cofaces.end(); ++iter){
        if ((*iter).second == 2)
            cofaces_1.push_back((*iter).first);
        else if ((*iter).second == 1)
            cofaces_2.push_back((*iter).first);
    }


#ifdef _DEBUG_SPLIT
    std::cout<<"cofaces_1.size() = "<<cofaces_1.size()<<std::endl;
    std::cout<<"cofaces_2.size() = "<<cofaces_2.size()<<std::endl;
#endif

    // la coupe passe sur un bord, on ne renseigne pas les Faces
    if (cofaces_1.empty())
        return;

    if (cofaces_2.empty())
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Face::split ne retrouve pas la coupure", TkUtil::Charset::UTF_8));


    // tri des cofaces / UniqueId
    std::vector<CoFace* > cofaces_1_sorted, cofaces_2_sorted;
    cofaces_1.sort(Utils::Entity::compareEntity);
    cofaces_2.sort(Utils::Entity::compareEntity);
    cofaces_1_sorted.insert(cofaces_1_sorted.end(),cofaces_1.begin(),cofaces_1.end());
    cofaces_2_sorted.insert(cofaces_2_sorted.end(),cofaces_2.begin(),cofaces_2.end());

#ifdef _DEBUG_SPLIT
    std::cout<<"cofaces_1_sorted : ";
    for (uint i=0; i<cofaces_1_sorted.size(); i++)
    	std::cout<<" "<<cofaces_1_sorted[i]->getName();
    std::cout<<std::endl;
    std::cout<<"cofaces_2_sorted : ";
    for (uint i=0; i<cofaces_2_sorted.size(); i++)
    	std::cout<<" "<<cofaces_2_sorted[i]->getName();
    std::cout<<std::endl;
#endif

    // recherche des nouveaux sommets à partir des arêtes sur les bords
    // on recherche la direction des arêtes coupées
    std::vector<CoEdge* > iCoedges;
    std::vector<CoEdge* > jCoedges;
    eDirOnFace dirFaceSplit = undef_dir;
    getOrientedCoEdges(iCoedges, jCoedges);

    // on sépare les coedges en 4 groupes (un par côté de la face)
    std::vector<CoEdge* > iCoedges_1, iCoedges_2, jCoedges_1, jCoedges_2;
    if (!TopoHelper::getCoEdgesBetweenVertices(getVertex(1), getVertex(2), iCoedges, iCoedges_1))
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Face::split echoue avec getCoEdgesBetweenVertices / iCoedges_1", TkUtil::Charset::UTF_8));
    if (getNbVertices() == 4){
        if (!TopoHelper::getCoEdgesBetweenVertices(getVertex(0), getVertex(3), iCoedges, iCoedges_2))
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Face::split echoue avec getCoEdgesBetweenVertices / iCoedges_2", TkUtil::Charset::UTF_8));
    }
    else {
        // iCoedges_2 reste vide
    }
    if (!TopoHelper::getCoEdgesBetweenVertices(getVertex(1), getVertex(0), jCoedges, jCoedges_1))
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Face::split echoue avec getCoEdgesBetweenVertices / jCoedges_1", TkUtil::Charset::UTF_8));
    if (getNbVertices() == 4){
        if (!TopoHelper::getCoEdgesBetweenVertices(getVertex(2), getVertex(3), jCoedges, jCoedges_2))
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Face::split echoue avec getCoEdgesBetweenVertices / jCoedges_2", TkUtil::Charset::UTF_8));
    }
    else {
        if (!TopoHelper::getCoEdgesBetweenVertices(getVertex(2), getVertex(0), jCoedges, jCoedges_2))
            throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Face::split echoue avec getCoEdgesBetweenVertices / jCoedges_2", TkUtil::Charset::UTF_8));
    }

    if (getNbVertices() == 4){
        for (std::vector<CoEdge* >::iterator iter = iCoedges_1.begin();
                iter != iCoedges_1.end(); ++iter)
            if (filtre_vertex[(*iter)->getVertex(0)] == 2 || filtre_vertex[(*iter)->getVertex(1)] == 2)
                dirFaceSplit = i_dir;
        for (std::vector<CoEdge* >::iterator iter = iCoedges_2.begin();
                iter != iCoedges_2.end(); ++iter)
            if (filtre_vertex[(*iter)->getVertex(0)] == 2 || filtre_vertex[(*iter)->getVertex(1)] == 2)
                dirFaceSplit = i_dir;

        for (std::vector<CoEdge* >::iterator iter = jCoedges_1.begin();
                iter != jCoedges_1.end(); ++iter)
            if (filtre_vertex[(*iter)->getVertex(0)] == 2 || filtre_vertex[(*iter)->getVertex(1)] == 2)
                dirFaceSplit = j_dir;
        for (std::vector<CoEdge* >::iterator iter = jCoedges_2.begin();
                iter != jCoedges_2.end(); ++iter)
            if (filtre_vertex[(*iter)->getVertex(0)] == 2 || filtre_vertex[(*iter)->getVertex(1)] == 2)
                dirFaceSplit = j_dir;
    }
    else {
        // cas avec dégénérecence, on détermine le sens en fonction du fait
        // que la coupe passe par un des sommets de la face ou non
        bool coupeParSommetFace = false;
        for (uint i=0; i<getNbVertices(); i++)
            if (filtre_vertex[getVertex(i)] == 2)
                coupeParSommetFace = true;

        if (coupeParSommetFace)
            dirFaceSplit = i_dir;
        else
            dirFaceSplit = j_dir;
    }

    if (dirFaceSplit == undef_dir)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Face::split ne trouve pas de direction pour la coupe", TkUtil::Charset::UTF_8));


    // recherche des sommets nouveaux de la coupe sur le bord de la face
    Vertex* vtx1 = 0;
    Vertex* vtx2 = 0;
    if (dirFaceSplit == i_dir){
        for (std::vector<CoEdge* >::iterator iter = iCoedges_1.begin();
                iter != iCoedges_1.end(); ++iter)
            if (filtre_vertex[(*iter)->getVertex(0)] == 2)
                vtx1 = (*iter)->getVertex(0);
            else if (filtre_vertex[(*iter)->getVertex(1)] == 2)
                vtx1 = (*iter)->getVertex(1);
        if (iCoedges_2.empty())
            vtx2 = getVertex(0);
        else
            for (std::vector<CoEdge* >::iterator iter = iCoedges_2.begin();
                    iter != iCoedges_2.end(); ++iter)
                if (filtre_vertex[(*iter)->getVertex(0)] == 2)
                    vtx2 = (*iter)->getVertex(0);
                else if (filtre_vertex[(*iter)->getVertex(1)] == 2)
                    vtx2 = (*iter)->getVertex(1);
    } else {
        for (std::vector<CoEdge* >::iterator iter = jCoedges_1.begin();
                iter != jCoedges_1.end(); ++iter)
            if (filtre_vertex[(*iter)->getVertex(0)] == 2)
                vtx1 = (*iter)->getVertex(0);
            else if (filtre_vertex[(*iter)->getVertex(1)] == 2)
                vtx1 = (*iter)->getVertex(1);

        for (std::vector<CoEdge* >::iterator iter = jCoedges_2.begin();
                iter != jCoedges_2.end(); ++iter)
            if (filtre_vertex[(*iter)->getVertex(0)] == 2)
                vtx2 = (*iter)->getVertex(0);
            else if (filtre_vertex[(*iter)->getVertex(1)] == 2)
                vtx2 = (*iter)->getVertex(1);
    }

    if (0 == vtx1 || 0 == vtx2)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, Face::split ne trouve pas l'un des sommets sur la coupe", TkUtil::Charset::UTF_8));

#ifdef _DEBUG_SPLIT
    std::cout<<"vtx1 = "<<vtx1->getName()<<std::endl;
    std::cout<<"vtx2 = "<<vtx2->getName()<<std::endl;
#endif

    // construction des 2 groupes de 4 sommets pour chacunes des faces
    std::vector<Vertex* > vertices_1, vertices_2;
    if (dirFaceSplit == i_dir){
        vertices_1.push_back(getVertex(0));
        vertices_1.push_back(getVertex(1));
        vertices_1.push_back(vtx1);
        if (getNbVertices() == 4)
            vertices_1.push_back(vtx2);

        vertices_2.push_back(vtx2);
        vertices_2.push_back(vtx1);
        vertices_2.push_back(getVertex(2));
        if (getNbVertices() == 4)
            vertices_2.push_back(getVertex(3));

    } else {
        vertices_1.push_back(vtx1);
        vertices_1.push_back(getVertex(1));
        vertices_1.push_back(getVertex(2));
        vertices_1.push_back(vtx2);

        vertices_2.push_back(getVertex(0));
        vertices_2.push_back(vtx1);
        vertices_2.push_back(vtx2);
        if (getNbVertices() == 4)
            vertices_2.push_back(getVertex(3));

    }

    if (icmd){
        for (uint j=0; j<cofaces_1_sorted.size(); j++)
            cofaces_1_sorted[j]->saveCoFaceTopoProperty(icmd);
        for (uint j=0; j<cofaces_2_sorted.size(); j++)
            cofaces_2_sorted[j]->saveCoFaceTopoProperty(icmd);
    }

    // construction des 2 faces
    face1 = new Topo::Face(getContext(), cofaces_1_sorted, vertices_1, true);
    face2 = new Topo::Face(getContext(), cofaces_2_sorted, vertices_2, true);

#ifdef _DEBUG_SPLIT
    std::cout<<"Création de la face1 : "<<*face1;
    std::cout<<"Création de la face2 : "<<*face2;
#endif

    if (icmd){
        for (uint j=0; j<cofaces_1_sorted.size(); j++)
            icmd->addTopoInfoEntity(cofaces_1_sorted[j], Internal::InfoCommand::OTHERMODIFIED);
        for (uint j=0; j<cofaces_2_sorted.size(); j++)
            icmd->addTopoInfoEntity(cofaces_2_sorted[j], Internal::InfoCommand::OTHERMODIFIED);
        icmd->addTopoInfoEntity(face1, Internal::InfoCommand::CREATED);
        icmd->addTopoInfoEntity(face2, Internal::InfoCommand::CREATED);
        free(icmd);
    }


}
/*----------------------------------------------------------------------------*/
void Face::
saveFaceMeshingProperty(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::OTHERMODIFIED);
        if (change && m_save_mesh_property == 0)
            m_save_mesh_property = m_mesh_property->clone();
    }
}
/*----------------------------------------------------------------------------*/
void Face::
saveFaceTopoProperty(Internal::InfoCommand* icmd)
{
    if (icmd) {
        bool change = icmd->addTopoInfoEntity(this,Internal::InfoCommand::DISPMODIFIED);
        if (change && m_save_topo_property == 0)
            m_save_topo_property = m_topo_property->clone();
    }
}
/*----------------------------------------------------------------------------*/
FaceMeshingProperty* Face::
setProperty(FaceMeshingProperty* prop)
{
    FaceMeshingProperty* tmp = m_mesh_property;
    m_mesh_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
FaceTopoProperty* Face::
setProperty(FaceTopoProperty* prop)
{
    FaceTopoProperty* tmp = m_topo_property;
    m_topo_property = prop;
    return tmp;
}
/*----------------------------------------------------------------------------*/
void Face::
saveInternals(CommandEditTopo* cet)
{
    TopoEntity::saveInternals (cet);

    if (m_save_mesh_property) {
        cet->addFaceInfoMeshingProperty(this, m_save_mesh_property);
        m_save_mesh_property = 0;
    }
    if (m_save_topo_property) {
        cet->addFaceInfoTopoProperty(this, m_save_topo_property);
        m_save_topo_property = 0;
    }
}
/*----------------------------------------------------------------------------*/
void Face::
getEdges(std::vector<Edge* >& edges) const
{
    edges.clear();
    std::list<Topo::Edge*> l_e;
    for(uint i=0; i < getNbCoFaces();i++) {
        const std::vector<Edge* >& local_edges = getCoFace(i)->getEdges();
        l_e.insert(l_e.end(), local_edges.begin(), local_edges.end());
    }

    l_e.sort(Utils::Entity::compareEntity);
    l_e.unique();

    edges.insert(edges.end(),l_e.begin(),l_e.end());
}
/*----------------------------------------------------------------------------*/
Edge* Face::
getEdge(Vertex* v1, Vertex* v2) const
{
    std::vector<Edge* > edges;
    getEdges(edges);

    for (std::vector<Edge* >::iterator iter = edges.begin();
            iter != edges.end(); ++iter)
        if ((*iter)->getNbVertices() == 2){
            Vertex* s1 = (*iter)->getVertex(0);
            Vertex* s2 = (*iter)->getVertex(1);
            if ((s1 == v1 && s2 == v2) || (s1 == v2 && s2 == v1))
                return (*iter);
        }

    // on a échoué
	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 << "Face::getEdge("
            <<v1->getName()<<","
            <<v2->getName()<<")\n";
    message1 << "this :"<<*this;
    //log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));
    std::cerr<<message1<<std::endl;
    throw TkUtil::Exception (TkUtil::UTF8String ("Face::getEdge(v1,v2) ne trouve l'arête pour ces 2 sommets", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
Edge* Face::
getEdge(uint ind) const
{
    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Face::getEdge(ind) n'est pas possible avec une surface non structurée", TkUtil::Charset::UTF_8));
    if (getNbCoFaces() != 1)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, il n'est pas prévu de faire appel à Face::getEdge pour une face composée", TkUtil::Charset::UTF_8));

    return getCoFace(0)->getEdge(ind);
}
/*----------------------------------------------------------------------------*/
void Face::
getCoEdges(std::vector<CoEdge* >& coedges) const
{
    coedges.clear();
    std::list<Topo::CoEdge*> l_e;
    for(uint i=0; i < getNbCoFaces();i++) {
        const std::vector<Edge* >& local_edges = getCoFace(i)->getEdges();
        for(uint j=0; j < local_edges.size();j++) {
            const std::vector<CoEdge* >& local_coedges = local_edges[j]->getCoEdges();

            l_e.insert(l_e.end(), local_coedges.begin(), local_coedges.end());
        }
    }

    l_e.sort(Utils::Entity::compareEntity);
    l_e.unique();

    coedges.insert(coedges.end(),l_e.begin(),l_e.end());
}
/*----------------------------------------------------------------------------*/
void Face::
getOrientedCoEdges(std::vector<CoEdge* > & iCoedges,
                   std::vector<CoEdge* > & jCoedges) const
{
//#define _DEBUG_getOrientedCoEdges
#ifdef _DEBUG_getOrientedCoEdges
	std::cout<<"Face::getOrientedCoEdges() pour la face: "<<getName()<<std::endl;
	std::cout<<"avec "<<getNbVertices()<<" sommets:";
	for (uint i=0; i<getNbVertices(); i++)
		std::cout<<" "<<getVertex(i)->getName();
	std::cout<<std::endl;
#endif
    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Face::getOrientedCoEdges(,) n'est pas possible avec une surface non structurée dans ", TkUtil::Charset::UTF_8)+getName());

    // on marque tous les cofaces et coedges de la face
    std::map<CoFace*, uint> filtre_coface;
    std::map<CoEdge*, uint> filtre_coedge;
    for(uint i=0; i < getNbCoFaces();i++){
        CoFace* coface = getCoFace(i);
        filtre_coface[coface] = 1;

        for(uint j=0; j < coface->getNbEdges();j++){
            Edge* edge = coface->getEdge(j);

            for(uint k=0; k < edge->getNbCoEdges();k++){
                CoEdge* coedge = edge->getCoEdge(k);
                filtre_coedge[coedge] = 1;
#ifdef _DEBUG_getOrientedCoEdges
                std::cout<<" on marque l'arête "<<coedge->getName()<<" vue dans "<<edge->getName()<<" "<<coface->getName()<<std::endl;
#endif

            } // for(uint k=0; k < edge->getNbCoEdges();k++)
        } // for(uint j=0; j < coface->getNbEdges();j++)
    } // for(uint i=0; i < getNbCoFaces();i++)


    // on part d'une première CoFace et de proche en proche on parcours toute la face
    std::vector<CoFace* > cofaces_dep, cofaces_suiv;

    if (getNbCoFaces()==0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Face::getOrientedCoEdges(,) ne peut se faire sur une face vide dans ", TkUtil::Charset::UTF_8)+getName());

    // on prend arbitrairement le même sens que pour la première CoFace
    // une permutation à la fin peut être nécessaire
    cofaces_dep.push_back(getCoFace(0));
    getCoFace(0)->getOrientedCoEdges(iCoedges, jCoedges);
    filtre_coface[getCoFace(0)] = 2;
    // on marque les CoEdge
    for (std::vector<CoEdge* >::iterator iter = iCoedges.begin();
            iter != iCoedges.end(); ++iter)
        filtre_coedge[*iter] = 2;
    for (std::vector<CoEdge* >::iterator iter = jCoedges.begin();
            iter != jCoedges.end(); ++iter)
        filtre_coedge[*iter] = 3;

    while (!cofaces_dep.empty()) {

        for (std::vector<CoFace* >::iterator iter1 = cofaces_dep.begin();
                iter1 != cofaces_dep.end(); ++iter1){
            CoFace* coface = *iter1;

            // recherche parmis les CoFaces voisines celles qui sont dans la Face et non-vues
            std::vector<CoFace* > cofaces_vois;
            coface->getNeighbour(cofaces_vois);

            for (std::vector<CoFace* >::iterator iter2 = cofaces_vois.begin();
                    iter2 != cofaces_vois.end(); ++iter2){
                CoFace* coface_v = *iter2;
                if (filtre_coface[coface_v] == 1){
                    filtre_coface[coface_v] = 2;
                    cofaces_suiv.push_back(coface_v);

                    // les CoEdges par direction de cette face voisine
                    std::vector<CoEdge* > iCoedges_v, jCoedges_v;

                    coface_v->getOrientedCoEdges(iCoedges_v, jCoedges_v);

                    // on cherche parmis les CoEdges de cette face une de marquée
                    // pour en déduire si le sens est le même ou non
                    bool sens = true;
                    bool trouve = false;
                    for (std::vector<CoEdge* >::iterator iter3 = iCoedges_v.begin();
                            iter3 != iCoedges_v.end(); ++iter3){
                        if (filtre_coedge[*iter3] == 2){
                            sens = true;
                            trouve = true;
                        } else if (filtre_coedge[*iter3] == 3){
                            sens = false;
                            trouve = true;
                        }
                    } // for iter3

                    for (std::vector<CoEdge* >::iterator iter3 = jCoedges_v.begin();
                            iter3 != jCoedges_v.end(); ++iter3){
                        if (filtre_coedge[*iter3] == 2){
                            sens = false;
                            trouve = true;
                        } else if (filtre_coedge[*iter3] == 3){
                            sens = true;
                            trouve = true;
                        }
                    } // for iter3

                    if (!trouve)
                        throw TkUtil::Exception (TkUtil::UTF8String ("Face::getOrientedCoEdges(,) ne trouve pas d'arête en commun avec la face voisine dans ", TkUtil::Charset::UTF_8)+getName());

                    // sens différent de la face commune voisine
                    if (!sens){
                        std::vector<CoEdge* > v_tmp;
                        v_tmp = iCoedges_v;
                        iCoedges_v = jCoedges_v;
                        jCoedges_v = v_tmp;
                    }

                    // on ajoute les nouvelles CoEdges
                    for (std::vector<CoEdge* >::iterator iter3 = iCoedges_v.begin();
                            iter3 != iCoedges_v.end(); ++iter3)
                        if (filtre_coedge[*iter3] == 1){
                            filtre_coedge[*iter3] = 2;
                            iCoedges.push_back(*iter3);
                        }
                    for (std::vector<CoEdge* >::iterator iter3 = jCoedges_v.begin();
                            iter3 != jCoedges_v.end(); ++iter3)
                        if (filtre_coedge[*iter3] == 1){
                            filtre_coedge[*iter3] = 3;
                            jCoedges.push_back(*iter3);
                        }

                } // if (filtre_coface[coface_v] == 1)
            } // for iter2
        } // for iter1

        cofaces_dep = cofaces_suiv;
        cofaces_suiv.clear();

    } // while (!cofaces_dep.empty())

    // vérification du sens, permutation si nécessaire
    bool sens12_i_ok, sens10_i_ok, sens12_j_ok, sens10_j_ok;
    std::vector<CoEdge* > coedges_between;

    // pour le cas avec une dégénérescence, on compte le nombre d'arêtes entre 2 sommets
    uint nbCoedges12_i = 0;
    uint nbCoedges10_i = 0;
    uint nbCoedges12_j = 0;
    uint nbCoedges10_j = 0;


    sens12_i_ok = TopoHelper::getCoEdgesBetweenVertices(getVertex(1), getVertex(2), iCoedges, coedges_between);
    nbCoedges12_i = coedges_between.size();

    sens10_i_ok = TopoHelper::getCoEdgesBetweenVertices(getVertex(1), getVertex(0), iCoedges, coedges_between);
    nbCoedges10_i = coedges_between.size();

    sens12_j_ok = TopoHelper::getCoEdgesBetweenVertices(getVertex(1), getVertex(2), jCoedges, coedges_between);
    nbCoedges12_j = coedges_between.size();

    sens10_j_ok = TopoHelper::getCoEdgesBetweenVertices(getVertex(1), getVertex(0), jCoedges, coedges_between);
    nbCoedges10_j = coedges_between.size();

#ifdef _DEBUG_getOrientedCoEdges
    std::cout<<"nbCoedges12_i = "<<nbCoedges12_i<<std::endl;
    std::cout<<"nbCoedges10_i = "<<nbCoedges10_i<<std::endl;
    std::cout<<"nbCoedges12_j = "<<nbCoedges12_j<<std::endl;
    std::cout<<"nbCoedges10_j = "<<nbCoedges10_j<<std::endl;
#endif

    bool sensI_ok, sensJ_ok;
    if (sens12_i_ok && sens10_i_ok){
        // on prend le plus court chemin
        if (nbCoedges12_i < nbCoedges10_i)
            sensI_ok = true;
        else if (nbCoedges12_i > nbCoedges10_i)
            sensI_ok = false;
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Face::getOrientedCoEdges(,) ne trouve pas de plus court chemin pour I dans ", TkUtil::Charset::UTF_8)+getName());
    } else if (sens12_i_ok)
        sensI_ok = true;
    else if (sens10_i_ok)
        sensI_ok = false;
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Face::getOrientedCoEdges(,) ne retrouve pas le bord de la face pour déterminer le sens de I dans ", TkUtil::Charset::UTF_8)+getName());

    if (sens12_j_ok && sens10_j_ok){
        // on prend le plus court chemin
        if (nbCoedges12_j < nbCoedges10_j)
            sensJ_ok = false;
        else if (nbCoedges12_j > nbCoedges10_j)
            sensJ_ok = true;
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Face::getOrientedCoEdges(,) ne trouve pas de plus court chemin pour J dans ", TkUtil::Charset::UTF_8)+getName());
    } else if (sens12_j_ok)
        sensJ_ok = false;
    else if (sens10_j_ok)
        sensJ_ok = true;
    else
        throw TkUtil::Exception (TkUtil::UTF8String ("Face::getOrientedCoEdges(,) ne retrouve pas le bord de la face pour déterminer le sens de J dans ", TkUtil::Charset::UTF_8)+getName());


    if (sensJ_ok != sensI_ok)
        throw TkUtil::Exception (TkUtil::UTF8String ("Face::getOrientedCoEdges(,) n'est pas cohérent entre les 2 côtés dans ", TkUtil::Charset::UTF_8)+getName());

    if (!sensI_ok){
        std::vector<CoEdge* > v_tmp;
        v_tmp = iCoedges;
        iCoedges = jCoedges;
        jCoedges = v_tmp;
    }
#ifdef _DEBUG_getOrientedCoEdges
    std::cout<<" getOrientedCoEdges sort correctement"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void Face::
getNbMeshingEdges(uint& nbI, uint& nbJ, bool accept_error) const
{
    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Face::getNbMeshingEdges(,) n'est pas possible avec une surface non structurée", TkUtil::Charset::UTF_8));

    try {
        // recherche des arêtes communes pour chacune des directions
        std::vector<CoEdge* > coedges_dir[2];

        getOrientedCoEdges(coedges_dir[0], coedges_dir[1]);

        _getNbMeshingEdges(nbI, nbJ, coedges_dir[0], coedges_dir[1]);

    } catch (TkUtil::Exception& exc) {
        if (accept_error){
            nbI = 0;
            nbJ = 0;
        }
        else
            throw exc;
    }
}
/*----------------------------------------------------------------------------*/
void Face::
_getNbMeshingEdges(uint& nbI, uint& nbJ,
        std::vector<CoEdge* > coedgesI,
        std::vector<CoEdge* > coedgesJ) const
{
    //std::cout<<"Face::_getNbMeshingEdges nbI et nbJ pour Face "<<getName()<<std::endl;
    // on marque les arêtes et les faces communes de la face à 1
    std::map<Edge*, uint> filtre_edges;
    std::map<CoFace*, uint> filtre_cofaces;
    for(uint i=0; i < getNbCoFaces(); i++){
        CoFace* coface = getCoFace(i);
        filtre_cofaces[coface] = 1;
        for(uint j=0; j < coface->getNbEdges(); j++){
            Edge* edge = coface->getEdge(j);
            filtre_edges[edge] = 1;
        } // for(uint j=0; j < coface->getNbEdges(); j++)
    } // for(uint i=0; i < getNbCoFaces(); i++)

    // on fait la même chose pour les deux directions:
    // on cumule le nombre de bras pour les arêtes en tenant compte des ratios au niveau des arêtes et des faces
    uint nb_dir[2];
    for (uint dir = 0; dir<2; dir++){
        // on fait un parcours depuis un sommet jusqu'à un autre sommet pour en extraire le sous-ensemble
        std::vector<CoEdge* > coedges_cote;
        if (TopoHelper::getCoEdgesBetweenVertices(getVertex(1), getVertex(dir==0?2:0), dir==0?coedgesI:coedgesJ, coedges_cote)){
            // à partir des ces arêtes commune on recherche les arêtes de la face
            // (ce qui permet d'avoir un nombre de bras tenant compte de la semi-conformité)
            nb_dir[dir] = _getNbMeshingEdges(filtre_edges, filtre_cofaces, coedges_cote,
                    getVertex(1), getVertex(dir==0?2:0));

        } else
            throw TkUtil::Exception (TkUtil::UTF8String ("Face::_getNbMeshingEdges ne trouve pas les arêtes communes pour la direction i", TkUtil::Charset::UTF_8));
    } // end for dir

    nbI = nb_dir[0];
    nbJ = nb_dir[1];
    //std::cout<<"Face::_getNbMeshingEdges() donne nbI = "<<nbI<<", nbJ = "<<nbJ<<std::endl;
}
/*----------------------------------------------------------------------------*/
uint Face::
_getNbMeshingEdges(std::map<Edge*, uint> &filtre_edges,
        std::map<CoFace*, uint> &filtre_cofaces,
        std::vector<CoEdge* > &coedges_cote,
        Vertex* v1, Vertex* v2) const
{
//    std::cout<<"Face::_getNbMeshingEdges avec filtre pour Face "<<getName()<<std::endl;
//    std::cout<<"v1 = "<<v1->getName()<<std::endl;
//    std::cout<<"v2 = "<<v2->getName()<<std::endl;

    uint nb_dir = 0;
    uint dir = getDir(v1, v2);

    for (std::vector<CoEdge* >::const_iterator iter1 = coedges_cote.begin();
            iter1 != coedges_cote.end(); ++iter1){
        const std::vector<Edge* > & edges = (*iter1)->getEdges();

//        std::cout<<"CoEdge observée : "<<(*iter1)->getName()<<std::endl;

        // recherche d'une arête non vue dans cette face
        for (std::vector<Edge* >::const_iterator iter2 = edges.begin();
                iter2 != edges.end(); ++iter2){

//            std::cout<<"Edge observée : "<<(*iter2)->getName()<<std::endl;


            if (filtre_edges[*iter2] == 1){
//                std::cout<<"Edge ok avec filtre"<<std::endl;

                // pour ne pas réutiliser cette arête
                filtre_edges[*iter2] = 2;

                // on recherche la face commune à laquelle elle appartient
                const std::vector<CoFace* > & cofaces = (*iter2)->getCoFaces();

                std::vector<CoFace* >::const_iterator iter3;
                for (iter3 = cofaces.begin(); iter3 != cofaces.end() && filtre_cofaces[*iter3] != 1; ++iter3){
//                    std::cout<<"CoFace observée : "<<(*iter3)->getName()<<std::endl;
                }
                if (iter3 != cofaces.end()){
                    // face commune trouvée, on ne change pas sa marque,
                    // car elle reservira pour la deuxième direction,
                    nb_dir += (*iter2)->getNbMeshingEdges() / m_mesh_property->getRatio(*iter3, dir);
//                    std::cout<<"CoFace ok ("<<(*iter3)->getName()<<") avec filtre"<<std::endl;
                }
                else {
                    std::cerr<<"Pb avec "<<*this;

                    throw TkUtil::Exception (TkUtil::UTF8String ("Face::_getNbMeshingEdges ne trouve la face commune associée à une arête", TkUtil::Charset::UTF_8));
                }
            } // end else / if (filtre_edges[*iter2] == 1)
        } // for iter2
    } // for iter1
    return nb_dir;
}
/*----------------------------------------------------------------------------*/
Edge* Face::
getOppositeEdge(Edge* e) const
{
    if (getNbCoFaces() != 1)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, il n'est pas prévu de faire appel à Face::getOppositeEdge pour une face composée", TkUtil::Charset::UTF_8));

    return getCoFace(0)->getOppositeEdge(e);
}
/*----------------------------------------------------------------------------*/
Vertex* Face::
getOppositeVertex(Vertex* v, eDirOnFace& dir) const
{
//    std::cout<<"Face::getOppositeVertex("<<v->getName()<<", "<<dir<<")\n";
    // recherche de l'indice du sommet
    uint ind = getIndex(v);
    if (dir == i_dir){
        if (ind == 1)
            return getVertex(2);
        else if (ind == 2)
            return getVertex(1);
        else if (ind == 3)
                    return getVertex(0);
        else if (ind == 0 && getNbVertices() == 4)
                    return getVertex(3);
        else if (ind == 0 && getNbVertices() == 3)
                    return getVertex(0); // cas dégénéré ...
    } else {
        if (ind == 0)
            return getVertex(1);
        else if (ind == 1)
            return getVertex(0);
        else if (ind == 3)
            return getVertex(2);
        else if (ind == 2 && getNbVertices() == 4)
            return getVertex(3);
        else if (ind == 2 && getNbVertices() == 3)
            return getVertex(0); // cas dégénéré ...
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
Face::eDirOnFace Face::
getDir(Vertex* v1, Vertex* v2) const
{
    if (!isStructured())
        throw TkUtil::Exception (TkUtil::UTF8String ("Face::getDir(V1, v2) n'est pas possible avec une surface non structurée", TkUtil::Charset::UTF_8));

    uint nbVtx = getNbVertices();
    // on se base uniquement sur l'ordre des sommets dans la face
    if (  (v1 == getVertex(0) && v2 == getVertex(1))
            || (v1 == getVertex(1) && v2 == getVertex(0))
            || (nbVtx == 4 && v1 == getVertex(2) && v2 == getVertex(3))
            || (nbVtx == 4 && v1 == getVertex(3) && v2 == getVertex(2))
            || (nbVtx == 3 && v1 == getVertex(2) && v2 == getVertex(0))
            || (nbVtx == 3 && v1 == getVertex(0) && v2 == getVertex(2)) )
        return j_dir;
    else if (  (v1 == getVertex(1) && v2 == getVertex(2))
            || (v1 == getVertex(2) && v2 == getVertex(1))
            || (nbVtx == 4 && v1 == getVertex(0) && v2 == getVertex(3))
            || (nbVtx == 4 && v1 == getVertex(3) && v2 == getVertex(0)) )
        return i_dir;
    else {
        std::cerr<<"Face::getDir("<<v1->getName()<<", "<<v2->getName()<<") dans la face : "<<*this<<std::endl;
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, on ne trouve pas les sommets dans la face pour getDir()", TkUtil::Charset::UTF_8));
    }
}
/*----------------------------------------------------------------------------*/
void Face::
setGeomAssociation(Geom::GeomEntity* ge)
{
    // ce sont les faces communes qui portent les projections
    for (uint i=0; i<getNbCoFaces(); i++)
        getCoFace(i)->setGeomAssociation(ge);
}
/*----------------------------------------------------------------------------*/
void Face::
_permuteToFirstAndLastVertices(Vertex* v1, Vertex* v2, Internal::InfoCommand* icmd)
{

    uint nb = getNbVertices();

    // recherche des indices de v1 et v2
    uint ind1 = 0;
    uint ind2 = 0;
    bool trouve1 = false;
    bool trouve2 = false;
    for (uint ind = 0; ind<nb; ind++){
        Vertex* vi = getVertex(ind);
        if (v1 == vi){
            trouve1 = true;
            ind1 = ind;
        }
        if (v2 == vi){
            trouve2 = true;
            ind2 = ind;
        }
    }

    if (!trouve1 || !trouve2)
        throw TkUtil::Exception (TkUtil::UTF8String ("Face::_permuteToFirstAndLastVertices ne trouve pas l'un des sommets", TkUtil::Charset::UTF_8));

    std::vector<Vertex* > initial_vertices;
    m_topo_property->getVertexContainer().get(initial_vertices);

    std::vector<Vertex* > sorted_vertices;

    // le changement implique-t-il de permuter les ratios de FaceMeshProperty ?
    // util pour le cas structuré
    bool perm_ratio = false;

    if (isStructured()){
        if (getNbVertices() != 4){
			TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
            message1 << "Face::_permuteToFirstAndLastVertices(...) pour "<<*this;
            message1 << " avec v1 : "<<*v1<< " et v2 : "<<*v2;
            log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_4));
            throw TkUtil::Exception (TkUtil::UTF8String ("Face::_permuteToFirstAndLastVertices n'est possible qu'avec une face structurée à 4 sommets", TkUtil::Charset::UTF_8));
        }

        if ((ind1+1)%4 == ind2){
            // cas avec v1 juste avant v2
            // on met v1 en premier tout en inversent le sens
            for (uint ind = 0; ind<4; ind++)
                sorted_vertices.push_back(initial_vertices[(ind1-ind+4)%4]);
            if ((ind1%2) == 1)
                perm_ratio = true;
        }
        else if ((ind2+1)%4 == ind1){
            // cas avec v1 juste après v2
            // on met v1 en premier sans rien permutter
            for (uint ind = 0; ind<4; ind++)
                sorted_vertices.push_back(initial_vertices[(ind1+ind)%4]);
            if ((ind1%2) == 0)
                perm_ratio = true;
        }
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Face::_permuteToFirstAndLastVertices n'arrive pas à ordonner les sommets", TkUtil::Charset::UTF_8));
    }
    else {
        if ((ind1+1)%nb == ind2){
            // cas avec v1 juste avant v2
            // on met v1 en premier tout en inversent le sens
            for (uint ind = 0; ind<nb; ind++)
                sorted_vertices.push_back(initial_vertices[(ind1-ind+nb)%nb]);
        }
        else if ((ind2+1)%nb == ind1){
            // cas avec v1 juste après v2
            // on met v1 en premier sans rien permutter
            for (uint ind = 0; ind<nb; ind++)
                sorted_vertices.push_back(initial_vertices[(ind1+ind)%nb]);
        }
        else
            throw TkUtil::Exception (TkUtil::UTF8String ("Face::_permuteToFirstAndLastVertices n'arrive pas à ordonner les sommets", TkUtil::Charset::UTF_8));
    }

    m_topo_property->getVertexContainer().clear();
    m_topo_property->getVertexContainer().add(sorted_vertices);

    if (perm_ratio)
        _permuteMeshingRatios(icmd);
}
/*----------------------------------------------------------------------------*/
void Face::
_permuteMeshingRatios(Internal::InfoCommand* icmd)
{
    for (uint i=0; i<getNbCoFaces(); i++){
        CoFace* coface = getCoFace(i);
        uint nbI = m_mesh_property->getRatio(coface, 0);
        uint nbJ = m_mesh_property->getRatio(coface, 1);
        if (nbI != nbJ){
            saveFaceMeshingProperty(icmd);
            m_mesh_property->setRatio(coface, nbI, 1);
            m_mesh_property->setRatio(coface, nbJ, 0);
        }
    }
}
/*----------------------------------------------------------------------------*/
bool Face::
isEdited() const
{
    return TopoEntity::isEdited()
    || m_save_topo_property != 0
    || m_save_mesh_property != 0;
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point Face::getBarycentre() const
{
	Utils::Math::Point barycentre;

	std::vector<Topo::Vertex* > vertices;
	getVertices(vertices);
	for (uint i=0; i<vertices.size(); i++)
		barycentre += vertices[i]->getCoord();
	barycentre /= (double)vertices.size();
	return barycentre;
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
