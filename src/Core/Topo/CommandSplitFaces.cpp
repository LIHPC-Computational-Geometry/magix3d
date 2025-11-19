/*----------------------------------------------------------------------------*/
#include "Topo/CommandSplitFaces.h"
#include "Topo/TopoHelper.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/CoEdge.h"
#include "Geom/Curve.h"
#include "Geom/GeomProjectImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
//#define _DEBUG_SPLIT
/*----------------------------------------------------------------------------*/
CommandSplitFaces::
CommandSplitFaces(Internal::Context& c, std::vector<Topo::CoFace* > &cofaces, CoEdge* arete, double ratio_dec, double ratio_ogrid, bool project_on_meshing_edges)
:CommandEditTopo(c, "Découpage des faces structurées")
, m_arete(arete)
, m_ratio_dec(ratio_dec)
, m_ratio_ogrid(ratio_ogrid)
, m_project_on_meshing_edges(project_on_meshing_edges)
, m_cas_2D(true)
{
#ifdef _DEBUG_SPLIT
	std::cout<<"CommandSplitFaces::CommandSplitFaces("<<cofaces.size()<<" cofaces, "<<arete->getName()<<", "<<ratio_dec<<", "<<ratio_ogrid<<", "<<project_on_meshing_edges<<")"<<std::endl;
#endif
	init(cofaces);

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Découpage des faces structurées";
	for (uint i=0; i<cofaces.size() && i<5; i++)
		comments << " " << cofaces[i]->getName();
	if (cofaces.size()>5)
		comments << " ... ";
	comments << " suivant l'arête "<<arete->getName();
	comments << " avec comme ratio "<<ratio_dec;

	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSplitFaces::
CommandSplitFaces(Internal::Context& c, std::vector<Topo::CoFace* > &cofaces, CoEdge* arete, const Utils::Math::Point& pt, double ratio_ogrid, bool project_on_meshing_edges)
:CommandEditTopo(c, "Découpage des faces structurées")
, m_arete(arete)
, m_ratio_dec(0)
, m_ratio_ogrid(ratio_ogrid)
, m_project_on_meshing_edges(project_on_meshing_edges)
, m_cas_2D(true)
{
	m_ratio_dec = arete->computeRatio(pt);

#ifdef _DEBUG_SPLIT
	std::cout<<"CommandSplitFaces::CommandSplitFaces("<<cofaces.size()<<" cofaces, "<<arete->getName()<<", "<<m_ratio_dec<<", "<<ratio_ogrid<<", "<<project_on_meshing_edges<<")"<<std::endl;
#endif
	init(cofaces);

	TkUtil::UTF8String	comments (TkUtil::Charset::UTF_8);
	comments << "Découpage des faces structurées";
	for (uint i=0; i<cofaces.size() && i<5; i++)
		comments << " " << cofaces[i]->getName();
	if (cofaces.size()>5)
		comments << " ... ";
	comments << " suivant l'arête "<<arete->getName();
	comments << " avec comme point de départ "<<pt.getScriptCommand();

	setScriptComments(comments);
	setName(comments);
}
/*----------------------------------------------------------------------------*/
CommandSplitFaces::
CommandSplitFaces(Internal::Context& c, CoEdge* arete, double ratio_dec, double ratio_ogrid, bool project_on_meshing_edges)
:CommandEditTopo(c, "Découpage de toutes les faces structurées")
, m_arete(arete)
, m_ratio_dec(ratio_dec)
, m_ratio_ogrid(ratio_ogrid)
, m_project_on_meshing_edges(project_on_meshing_edges)
, m_cas_2D(true)
{
#ifdef _DEBUG_SPLIT
	std::cout<<"CommandSplitFaces::CommandSplitFaces("<<arete->getName()<<", "<<ratio_dec<<", "<<ratio_ogrid<<", "<<project_on_meshing_edges<<")"<<std::endl;
#endif
	std::vector<Topo::CoFace* > cofaces = getContext().getTopoManager().getCoFacesObj();
	init(cofaces);
}
/*----------------------------------------------------------------------------*/
CommandSplitFaces::
CommandSplitFaces(Internal::Context& c, CoEdge* arete, const Point& pt, double ratio_ogrid, bool project_on_meshing_edges)
:CommandEditTopo(c, "Découpage de toutes les faces structurées")
, m_arete(arete)
, m_ratio_dec(0)
, m_ratio_ogrid(ratio_ogrid)
, m_project_on_meshing_edges(project_on_meshing_edges)
, m_cas_2D(true)
{
    m_ratio_dec = arete->computeRatio(pt);
#ifdef _DEBUG_SPLIT
	std::cout<<"CommandSplitFaces::CommandSplitFaces("<<arete->getName()<<", "<<m_ratio_dec<<", "<<ratio_ogrid<<", "<<project_on_meshing_edges<<")"<<std::endl;
#endif
    std::vector<Topo::CoFace* > cofaces = getContext().getTopoManager().getCoFacesObj();
    init(cofaces);
}
/*----------------------------------------------------------------------------*/
void CommandSplitFaces::init(std::vector<Topo::CoFace* > &cofaces)
{
#ifdef _DEBUG_SPLIT
	std::cout<<"init avec "<<cofaces.size() <<" cofaces"<<std::endl;
#endif
    // on ne conserve que les faces  structurées
    for (Topo::CoFace* hcf : cofaces){
		
        if (hcf->isStructured()){
			if (hcf->getNbFaces()) {
				// si hcf est 2D, elle n'a pas de face
				// et si au moins une coface est 3D, le cas est 3D
				m_cas_2D = false;
			}
			m_cofaces.push_back(hcf);
            verif(hcf);
#ifdef _DEBUG_SPLIT
            std::cout<< hcf->getName()<<" est retenue"<<std::endl;
#endif
        }
        else {
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "Une face (" << hcf->getName() << ") n'est pas structurée";
            log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
        }
    }
}
/*----------------------------------------------------------------------------*/
void CommandSplitFaces::verif(Topo::CoFace* coface)
{
#ifdef _DEBUG2
	std::cout<<"CommandSplitFaces::verif pour "<<coface->getName()<<std::endl;
#endif

	std::vector<CoEdge*> coedges = coface->getCoEdges();

	// recherche si une arête est projetée sur un demi cercle

	for (uint i=0; i<coedges.size(); i++){
		Geom::Curve* curve = 0;
		if (coedges[i]->getGeomAssociation())
			curve = dynamic_cast<Geom::Curve*>(coedges[i]->getGeomAssociation());

		if (curve && curve->isCircle()){
			double first, last;
			curve->getParameters(first, last);
#ifdef _DEBUG2
			std::cout<<"arête "<<coedges[i]->getName()<<" associée à un cercle"<<std::endl;
			std::cout<<"first  = "<<first<<std::endl;
			std::cout<<"last  = "<<last<<std::endl;
#endif
			if (Utils::Math::MgxNumeric::isNearlyZero(last - M_PI)){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
				message << "Une face (" << coface->getName()
						<< ") a 1 côté ("<<coedges[i]->getName()
						<<") associé à un demi-cercle ("<<curve->getName()<<")\n";
				message << "  Il est recommandé de couper ce cercle en 2 dans la modélisation 2D";
				log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
			}
		}


	} // end for i<coedges.size()


	// recherche si 2 côtés de part et d'autre d'un sommet logique de la coface sont colinéaires
	const std::vector<Vertex*>& vertices = coface->getVertices();
	if (coface->isStructured() && vertices.size() == 4){

		// on marque les arêtes de cette coface
		std::map<CoEdge*, uint> filtre_ce;
		for (uint i=0; i<coedges.size(); i++)
			filtre_ce[coedges[i]] = 1;


		// pour tous les sommets de la coface
		// recherche d'un couple d'arêtes de la face de part et d'autre d'un sommet
		for (uint i=0; i<vertices.size(); i++){
			Vertex* vtx = vertices[i];
#ifdef _DEBUG2
			std::cout<<"sommet "<<vtx->getName()<<std::endl;
#endif
			std::vector<CoEdge*> aretes;

			// on se limite aux arêtes reliés au sommet
			for (CoEdge* vtx_ce : vtx->getCoEdges())
				if (filtre_ce[vtx_ce] == 1){
					filtre_ce[vtx_ce] = 2;
					aretes.push_back(vtx_ce);
				}

			if (aretes.size() == 2){
#ifdef _DEBUG2
				std::cout<<"arêtes "<<aretes[0]->getName()<<" et "<<aretes[1]->getName()<<std::endl;
#endif
				// tester si ces arêtes sont des droites et si elles sont colinéaires
				std::vector<Geom::Curve*> courbes;
				for (uint j=0; j<2; j++)
					if (aretes[j]->getGeomAssociation())
						courbes.push_back(dynamic_cast<Geom::Curve*>(aretes[j]->getGeomAssociation()));
#ifdef _DEBUG2
				if (courbes.size() == 2 && courbes[0] && courbes[1]){
					std::cout<<"courbes "<<courbes[0]->getName()<<" et "<<courbes[1]->getName()<<std::endl;
					std::cout<<" la 1ère isLinear = "<<courbes[0]->isLinear()<<std::endl;
					std::cout<<" la 2ème isLinear = "<<courbes[1]->isLinear()<<std::endl;
				}
#endif

				if (courbes.size() == 2 && courbes[0] && courbes[1] && courbes[0]->isLinear() && courbes[1]->isLinear()){
					Utils::Math::Vector v1(vtx->getCoord(), aretes[0]->getOppositeVertex(vtx)->getCoord());
					Utils::Math::Vector v2(vtx->getCoord(), aretes[1]->getOppositeVertex(vtx)->getCoord());
					double dev = (v1*v2).norme();
#ifdef _DEBUG2
					std::cout<<"dev = "<<dev<<std::endl;
#endif

					if (Utils::Math::MgxNumeric::isNearlyZero(dev)){
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
						message << "Une face (" << coface->getName()
									<< ") a 2 côtés anormalement alignés (autour du sommet "<<vtx->getName()<<")";
						log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
					}
				}

				// reinit du filtre
				for (uint j=0; j<2; j++)
					filtre_ce[aretes[j]] = 1;
			}
			else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
				messErr<<"Le sommet "<<vtx->getName()<<" ne serait pas relié à 2 arêtes de la face "
						<<coface->getName();
				throw TkUtil::Exception(messErr);
			}
		}
	}
}
/*----------------------------------------------------------------------------*/
CommandSplitFaces::
~CommandSplitFaces()
{
#ifdef _DEBUG_SPLIT
	std::cout<<"CommandSplitFaces::~CommandSplitFaces()"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void CommandSplitFaces::
internalExecute()
{
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandSplitFaces::execute pour la commande " << getName ( )
            << " de nom unique " << getUniqueName ( );

    if (m_ratio_dec<0.0 || m_ratio_dec>1.0){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    	messErr<<"Le ratio pour le découpage doit être dans l'interval [0 1] et non "<<m_ratio_dec;
    	throw TkUtil::Exception(messErr);
    }


    if (m_ratio_ogrid<0.0 || m_ratio_ogrid>=1.0)
        throw TkUtil::Exception (TkUtil::UTF8String ("Le ratio pour l'ogrid doit être dans l'interval [0 1[", TkUtil::Charset::UTF_8));

    // vérification que l'arête appartient à l'une des cofaces
    bool arete_vue = false;
    for (std::vector<CoFace* >::iterator iter1 = m_cofaces.begin();
    		iter1 != m_cofaces.end(); ++iter1){
    	CoFace* coface = *iter1;
    	for (CoEdge* coedge : coface->getCoEdges())
    		if (coedge == m_arete)
    			arete_vue = true;
    }
    if (!arete_vue)
    	throw TkUtil::Exception (TkUtil::UTF8String ("L'arête sélectionnée doit appartenir à l'une des faces topologiques", TkUtil::Charset::UTF_8));


	uint nb_faces_dep = m_cofaces.size();
	uint nb_faces_split = 0;

	std::set<Vertex*> filtre_vertices;
	std::set<CoFace*> filtre_faces;

	std::vector<Edge*> splitingEdges;

	if (m_cas_2D) {
		TopoHelper::splitFaces2D(m_cofaces, m_arete, m_ratio_dec, m_ratio_ogrid, false, true, splitingEdges, &getInfoCommand());
	} else {
		do {
			Vertex* sommet = 0;
			CoFace* coface = 0;

			std::vector<Edge* > current_splitingEdge;
			std::vector<CoFace*> coface_vec;

			if (nb_faces_split){

				// recherche d'un bloc suivant avec une arête
				if(findFaceUnmarkedWithVertexMarked(filtre_faces, filtre_vertices, coface, sommet)) {
					coface_vec = {coface};

					CoEdge *coedge = 0;
					for (CoEdge* cf_ce : coface->getCoEdges())
						if (Utils::contains(sommet, cf_ce->getVertices()))
							coedge = cf_ce;

					if (coedge->getVertices()[0] == sommet)
						TopoHelper::splitFaces3D(coface_vec, coedge, 0, m_ratio_ogrid, false, false, current_splitingEdge, &getInfoCommand());
					else if (coedge->getVertices()[1] == sommet)
						TopoHelper::splitFaces3D(coface_vec, coedge, 1, m_ratio_ogrid, false, false, current_splitingEdge, &getInfoCommand());

					filtre_faces.insert(coface);
				}
			}
			else{
				std::vector<CoFace*> arete_cofaces = m_arete->getCoFaces();
				for (auto iter = arete_cofaces.begin(); iter != arete_cofaces.end(); ++iter)
					if (std::find(m_cofaces.begin(), m_cofaces.end(),*iter) != m_cofaces.end())
						coface = *iter;

				coface_vec = {coface};

				TopoHelper::splitFaces3D(coface_vec, m_arete, m_ratio_dec, m_ratio_ogrid, false, true, current_splitingEdge, &getInfoCommand());

				filtre_faces.insert(coface);
			}

			nb_faces_split+=1;

			for(auto e : current_splitingEdge){
				splitingEdges.push_back(e);
				filtre_vertices.insert(e->getVertices()[0]);
				filtre_vertices.insert(e->getVertices()[1]);
			}

		} while (nb_faces_dep != nb_faces_split);
	}

	// on replace les sommets en fonction de m_ratio_dec
    if (!m_project_on_meshing_edges)
    	for (uint i=0; i<splitingEdges.size(); i++){
    		Edge* edge = splitingEdges[i];
    		// recherche les cofaces de part et d'autre de cette edge
    		std::vector<CoFace*> cofaces;

    		for (CoFace* cf : edge->getCoFaces())
    			if (getInfoCommand().getTopoInfoEntity()[cf] == Internal::InfoCommand::CREATED)
    				cofaces.push_back(cf);

    		// cas avec découpage au bord (sans coupe en fait)
    		if (cofaces.size() == 0)
    			continue;

    		if (cofaces.size() != 2){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
    			messErr<<"On ne peut replacer le sommet suivant le ratio, on devrait avoir 2 faces topologiques et non "<<cofaces.size();
    			throw TkUtil::Exception(messErr);
    		}

    		// on utilise un filtre pour marquer les Edges que l'on accepte pour la suite
    		std::map<Edge*, uint> filtre_edge;
    		for (CoFace* cf : cofaces)
    			for (Edge* cf_e : cf->getEdges())
    				filtre_edge[cf_e] = 1;
    		filtre_edge[edge] = 0;

    		for (uint j=0; j<edge->getVertices().size(); j++){
    			Vertex* vtx_c = edge->getVertices()[j];
    			// les 2 arêtes de part et d'autre d'un sommet
    			std::vector<Edge*> edges;
				for (Edge* e : vtx_c->getEdges())
    				if (filtre_edge[e] == 1)
    					edges.push_back(e);

    			if (edges.size() != 2)
    				throw TkUtil::Exception (TkUtil::UTF8String ("On ne peut replacer le sommet suivant le ratio, on ne trouve pas exactement 2 edges autour d'un sommet", TkUtil::Charset::UTF_8));


    			double nb1 = edges[0]->getNbMeshingEdges();
    			double nb2 = edges[1]->getNbMeshingEdges();
    			double ratio_edges = nb1/(nb1+nb2);

    			bool sens = std::abs(ratio_edges-m_ratio_dec) < std::abs(1-ratio_edges-m_ratio_dec);
    			Vertex* vtx0 = edges[sens?0:1]->getOppositeVertex(vtx_c);
    			Vertex* vtx1 = edges[sens?1:0]->getOppositeVertex(vtx_c);

    			// l'association pour placer le sommet
    			Geom::GeomEntity* ge = vtx_c->getGeomAssociation();
    			std::vector<Utils::Math::Point> points;
    			if (ge){
    				Geom::Curve* curve = dynamic_cast<Geom::Curve*> (ge);
    				if (curve){
    					curve->getParametricsPoints(vtx0->getCoord(), vtx1->getCoord(), 1, &m_ratio_dec, points);
    				}
    				else {
    					points.push_back(vtx0->getCoord() + (vtx1->getCoord() - vtx0->getCoord()) * m_ratio_dec);
						Geom::GeomProjectVisitor gpv(points[0]);
						ge->accept(gpv);
						points[0] = gpv.getProjectedPoint();
    				}
    			}
    			else {
    				points.push_back(vtx0->getCoord() + (vtx1->getCoord() - vtx0->getCoord()) * m_ratio_dec);
    			}
    			vtx_c->setCoord(points[0]);

    		} // end forj<edge->getNbVertices()
    	} // end for i<splitingEdges.size()

    // suppression des entités temporaires
    cleanTemporaryEntities();

    // enregistrement des nouvelles entités dans le TopoManager
    registerToManagerCreatedEntities();

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
bool CommandSplitFaces::
findFaceUnmarkedWithVertexMarked(std::set<CoFace*>& filtre_faces,
										  std::set<Vertex*>& filtre_vertices, CoFace* &face, Vertex* &noeud)
{
	//std::cout<<"findBlockUnmarkedWithCoEdgeMarked ..."<<std::endl;
	for (CoFace* cf : m_cofaces){
		// le bloc est-il marqué ?
		if (filtre_faces.find(cf) == filtre_faces.end()){
			// possède-t-il une arête marquée ?
			for (Vertex* vtx : cf->getAllVertices()){
				if (filtre_vertices.find(vtx) != filtre_vertices.end()){
					noeud = vtx;
					face = cf;
					return true;
				}
			}
		}
	}
	return false;
}
/*----------------------------------------------------------------------------*/
void CommandSplitFaces::
countNbCoEdgesByVertices(std::map<Topo::Vertex*, uint> &nb_coedges_by_vertex)
{
	// stocke pour chacun des sommets le nombre d'arêtes auxquelles il est relié
	for (CoFace* coface : m_cofaces){
		for (Vertex* vtx : coface->getAllVertices())
			nb_coedges_by_vertex[vtx] = vtx->getCoEdges().size();
	}
}
/*----------------------------------------------------------------------------*/
void CommandSplitFaces::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	return getPreviewRepresentationTopoModif(dr);
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
