/*----------------------------------------------------------------------------*/
/*
 * MeshImplementationMeshGems.cpp
 *
 *  Created on: 26 juin 2015
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifdef USE_MESHGEMS
#include "Internal/ContextIfc.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/CommandCreateMesh.h"
/*----------------------------------------------------------------------------*/
#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/Edge.h"
#include "Topo/TopoHelper.h"
#include "Topo/FaceMeshingPropertyMeshGems.h"
/*----------------------------------------------------------------------------*/
#include "Utils/Common.h"
#include "Internal/Context.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/OCCGeomRepresentation.h"
/*----------------------------------------------------------------------------*/
/// TkUtil
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
#include "MeshGemsPlugin/SurfMesh.h"
#include "MeshGemsPlugin/SurfMeshParam.h"
#include "MeshGemsPlugin/MGOCCManager.h"
#include "MeshGemsPlugin/MGOCCCurve.h"
#include<Geom_Curve.hxx>
#include<BRep_Tool.hxx>
#include<GeomAPI_ProjectPointOnCurve.hxx>
#include<TopoDS.hxx>
#include<TopoDS_Edge.hxx>
#include<TopExp.hxx>
#include<gp_Pnt.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
void MeshImplementation::
meshTriangularMeshGems(Mesh::CommandCreateMesh* command, Topo::CoFace* fa)
{
#ifdef _DEBUG_MESH
	std::cout<<"Maillage de la face commune "<<fa->getName()
    						<<" avec MeshGems"<<std::endl;
#endif

	double bounds[6];
	fa->getBounds(bounds);
	Utils::Math::Point pt_min(bounds[0], bounds[2], bounds[4]);
	Utils::Math::Point pt_max(bounds[1], bounds[3], bounds[5]);
	double epsilonLocal = std::sqrt(pt_min.length2(pt_max)) * Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon;

	//Récupération des paramètres de maillage MeshGems
    Topo::FaceMeshingPropertyMeshGems* prop =
            dynamic_cast<Topo::FaceMeshingPropertyMeshGems*>(fa->getCoFaceMeshingProperty());
    CHECK_NULL_PTR_ERROR(prop);

	//Geometrie de la coface
	Geom::GeomEntity*  geo_entity = fa->getGeomAssociation();
	if(geo_entity==0){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message << "La face  "<< fa->getName()
                    		<< " ne peut pas être maillée en triangles : absence de géométrie associée";
		throw TkUtil::Exception (message);
	}

	// limitations actuelles du branchement MeshGems dans MGX3D
	{
		// les sommets topo doivent être associés à des sommets geom
		std::vector<Topo::Vertex*> vertices = fa->getVertices();

		for(auto v: vertices) {
			Geom::GeomEntity*  gv = v->getGeomAssociation();

			if (gv == 0){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
				message << "Le sommet "<< v->getName()
			                    		<< " n'est pas projeté, ce cas n'est pas prévu pour le cas MeshGems ";
				throw TkUtil::Exception (message);
			}

			Geom::Vertex* vertex = dynamic_cast<Geom::Vertex*>(gv);
			if (vertex == 0){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
				message << "Le sommet topo "<< v->getName()
			                    		<< " n'est pas projeté sur un sommet geom, ce qui n'est pas prévu pour le cas MeshGems ";
				throw TkUtil::Exception (message);
			}
		}

	}

#ifdef _DEBUG_MESH
	std::cout<<"========= FACE "<<geo_entity->getUniqueName()<<" ==========="<<std::endl;
#endif
	//surface geometrique M3D
	Geom::Surface* geo_surf=0;
	if(geo_entity->getDim()==2)
		geo_surf = dynamic_cast<Geom::Surface*>(geo_entity);

	CHECK_NULL_PTR_ERROR(geo_surf);

	//surface geometrique OCC
	Geom::OCCGeomRepresentation* f_rep =
			dynamic_cast<Geom::OCCGeomRepresentation*>(geo_surf->getComputationalProperty());
	CHECK_NULL_PTR_ERROR(f_rep);
	TopoDS_Shape f_shape = f_rep->getShape();

	MeshGems::MGOCCManager meshgems_occ_mng;
	meshgems_occ_mng.importFace(TopoDS::Face(f_shape));


	//===================================================
	// On linke les entites topologiques M3D et les
	// shapes du manager pour MeshGems
	std::map<int,int > points_M3_to_MG;
	std::map<int,int > points_MG_to_M3;
	std::map<int,int > curves_M3_to_MG;
	std::map<int,int > curves_MG_to_M3;


	///////////////////////////////////////////////////////////////
//	meshgems_occ_mng.newSurface(TopoDS::Face(f_shape));
//
//	std::vector<Topo::Vertex* > vertices;
//	fa->getVertices(vertices);
//	for(unsigned int i=0;i<vertices.size();i++){
//		Topo::Vertex* vi = vertices[i];
//
//		Geom::GeomEntity* geom_vi = vi->getGeomAssociation();
//		Geom::Vertex* curve = 0;
//	}
//
//	std::vector<Topo::CoEdge* > edges;
//	fa->getCoEdges(edges);//,false);
//
//	for(unsigned int i=0;i<edges.size();i++) {
//		//===================================================
//		//Traitement de l'edge i
//		//===================================================
//		Topo::CoEdge* edge = edges[i];
//		Geom::GeomEntity* geom_edge = edge->getGeomAssociation();
//		Geom::Curve* curve = 0;
//		if(geom_edge->getDim()!=1)
//			throw TkUtil::Exception (TkUtil::UTF8String ("MeshGems requiert des arêtes topologiques classifiées sur des courbes géométriques", TkUtil::Charset::UTF_8));
//		else
//			curve = dynamic_cast<Geom::Curve*>(geom_edge);
//
//		CHECK_NULL_PTR_ERROR(curve);
//		// we need the underlying occ shape for computing the parameterizatioin
//		Geom::OCCGeomRepresentation* e_rep =
//				dynamic_cast<Geom::OCCGeomRepresentation*>(curve->getComputationalProperty());
//		CHECK_NULL_PTR_ERROR(e_rep);
//		TopoDS_Shape e_shape = e_rep->getShape();
//		TopoDS_Edge occ_edge = TopoDS::Edge(e_shape);
//
//		meshgems_occ_mng.newCurve(occ_edge);
//	}
	///////////////////////////////////////////////////////////////

	// on recupere les entites de MeshGems
	std::vector<MeshGems::MGCurve*> mg_curves;
	std::vector<MeshGems::MGPoint*> mg_points;
	meshgems_occ_mng.getPoints(mg_points);
	meshgems_occ_mng.getCurves(mg_curves);

	// mise en relation des sommets
	std::vector<Topo::Vertex* > vertices;
	fa->getVertices(vertices);
	for(unsigned int i=0;i<vertices.size();i++){
		Topo::Vertex* vi = vertices[i];

		bool vertexFound = false;

		for(unsigned int j=0; j<mg_points.size();j++){
			MeshGems::MGPoint* pj = mg_points[j];

			std::cout<<"mg_points.size() "<<mg_points.size()<<std::endl;
			std::cout<<pj->X()<<" "<<pj->Y()<<" "<<pj->Z()<<std::endl;

			if(std::fabs(vi->getX()-pj->X())<epsilonLocal && std::fabs(vi->getY()-pj->Y())<epsilonLocal && std::fabs(vi->getZ()-pj->Z())<epsilonLocal) {
				points_M3_to_MG[i] = pj->getId();
				points_MG_to_M3[pj->getId()] = i;
#ifdef _DEBUG_MESH
				std::cout<<"ASSOC POINTS "<<i<<" -> "<<j<<std::endl;
				std::cout<<vi->getCoord()<<" "<<pj->getPoint()<<std::endl;
#endif
				vertexFound = true;
			}
		}
		if(!vertexFound) {
			std::cerr<<"MeshGems: Point association not found "<<vi->getName()<<" "<<vi->getCoord()<<std::endl;
			throw TkUtil::Exception (TkUtil::UTF8String ("MeshGems : l'Appariemment d'un sommet a échoué.", TkUtil::Charset::UTF_8));
		}
	}

	// mise en relation des courbes
	std::vector<Topo::CoEdge* > edges;
	fa->getCoEdges(edges);//,false);

	for(unsigned int i=0;i<edges.size();i++) {
		//===================================================
		//Traitement de l'edge i
		//===================================================
		Topo::CoEdge* edge = edges[i];
		Geom::GeomEntity* geom_edge = edge->getGeomAssociation();
		Geom::Curve* curve = 0;
		if(geom_edge->getDim()!=1)
			throw TkUtil::Exception (TkUtil::UTF8String ("MeshGems requiert des arêtes topologiques classifiées sur des courbes géométriques", TkUtil::Charset::UTF_8));
		else
			curve = dynamic_cast<Geom::Curve*>(geom_edge);

		CHECK_NULL_PTR_ERROR(curve);
		// we need the underlying occ shape for computing the parameterizatioin
		Geom::OCCGeomRepresentation* e_rep =
				dynamic_cast<Geom::OCCGeomRepresentation*>(curve->getComputationalProperty());
		CHECK_NULL_PTR_ERROR(e_rep);
		TopoDS_Shape e_shape = e_rep->getShape();
		TopoDS_Edge occ_edge = TopoDS::Edge(e_shape);

		bool curveFound = false;

		for(unsigned int j=0; j<mg_curves.size();j++) {
			MeshGems::MGOCCCurve* cj = dynamic_cast<MeshGems::MGOCCCurve*>(mg_curves[j]);
			if(cj==0)
				throw TkUtil::Exception (TkUtil::UTF8String ("Problème de courbes MG", TkUtil::Charset::UTF_8));

			TopoDS_Edge occ_cj = cj->getEdge();
//			if(occ_cj.IsSame(occ_edge)){ //IsSame
//			if(occ_cj.isEqual(occ_edge)){ //IsSame
			if(Geom::OCCGeomRepresentation::areEquals(occ_cj,occ_edge)){ //IsSame
				curves_M3_to_MG[i] = cj->getId();
				curves_MG_to_M3[cj->getId()] = i;
#ifdef _DEBUG_MESH
				std::cout<<"ASSOC "<<i<<" -> "<<j<<std::endl;
				std::cout<<"curve found "<<curve->getName()<<std::endl;
#endif
				curveFound = true;
			}
		}

		if(!curveFound) {
			std::cerr<<"MeshGems : curve not found "<<curve->getName()<<std::endl;
			throw TkUtil::Exception (TkUtil::UTF8String ("MeshGems : l'Appariemment d'une courbe a échoué.", TkUtil::Charset::UTF_8));
		}
	}


	//===================================================
	// Recuperation des entites de maillage liées aux
	// sommmets et aux aretes M3D
	//===================================================
	// on recupere le maillage des sommets
	std::vector<gmds::Node > v_nodes;
	for(unsigned int i=0;i<vertices.size();i++){
		v_nodes.push_back(getGMDSMesh().get<gmds::Node>(vertices[i]->getNode()));
	}
	//===================================================
	// on recupere le maillage des aretes topologiques
	//
	// Pour chaque arête, il nous faut:
	// 1) les coordonnées des sommets la discrétisant
	// 2) la courbe géométrique correspondant à l'arête. Si jamais celle-ci n'existe pas
	//    on lève une exception
	std::vector<Geom::Curve*> e_curves;
	std::vector<std::vector<gmds::Node > > e_nodes;
	std::vector<std::vector<double > > e_param_nodes;



	for(unsigned int i=0;i<edges.size();i++) {
		//===================================================
		//Traitement de l'edge i
		//===================================================
		Topo::CoEdge* edge = edges[i];
		Geom::GeomEntity* geom_edge = edge->getGeomAssociation();
		Geom::Curve* curve = 0;
		if(geom_edge->getDim()!=1)
			throw TkUtil::Exception (TkUtil::UTF8String ("MeshGems requiert des arêtes topologiques classifiées sur des courbes géométriques", TkUtil::Charset::UTF_8));
		else
			curve = dynamic_cast<Geom::Curve*>(geom_edge);

		//we store the curve for a next treatment to apply
		e_curves.push_back(curve);

#ifdef _DEBUG_MESH
		std::cout<<"Edge "<<edge->getUniqueId()<<" - curve "<<curve->getUniqueId()<<std::endl;
#endif
		// if the curve was previously traversed, it means we have
		// a periodic surface
		bool is_periodic=false;
		for(unsigned int j=0; j<i && !is_periodic; j++) {
			Topo::CoEdge* prev_edge = edges[j];
			if(edge->getUniqueId()==prev_edge->getUniqueId())
				is_periodic=true;
		}

		// we need the underlying occ shape for computing the parameterizatioin
		Geom::OCCGeomRepresentation* e_rep =
				dynamic_cast<Geom::OCCGeomRepresentation*>(curve->getComputationalProperty());
		TopoDS_Shape e_shape = e_rep->getShape();
		TopoDS_Edge occ_edge = TopoDS::Edge(e_shape);

		std::vector<Topo::Vertex* > v_edge = edge->getVertices();

		//		if(v_edge.size()!=2 || v_edge[0] == v_edge[1]){
//				    TkUtil::UTF_8   message (Charset::UTF_8);
//			message << "La face  "
//					<< fa->getName()
//					<< " ne peut pas être maillée en triangles :\n"
//					<<" une arete est incidente à "
//					<<v_edge.size()<<" sommets (ou 2 d'identiques)";
//			throw TkUtil::Exception (message);
//		}

		//===================================================
		//In order to provide edge discretization to meshGems, we must be in adequation
		// with the geometric curve orientation
		//===================================================

		Geom::Vertex* curve_pnt1 = curve->firstPoint();
		Geom::Vertex* curve_pnt2 = curve->secondPoint();
		if(is_periodic){
#ifdef _DEBUG_MESH
			std::cout<<"periodic !!!!!!!!"<<std::endl;
#endif
//			curve_pnt1 = curve->secondPoint();
//			curve_pnt2 = curve->firstPoint();
		}
		Topo::Vertex* edge_pnt1 = v_edge[0];
		Topo::Vertex* edge_pnt2 = v_edge[1];
		std::vector<gmds::Node> edge_nodes;
		std::vector<double > edge_param_nodes;

		//matching between geom and topo point based on their geometric location
		Utils::Math::Point geom_p1 = curve_pnt1->getPoint();
		Utils::Math::Point topo_p1 = edge_pnt1->getCoord();
		Utils::Math::Point topo_p2 = edge_pnt2->getCoord();

		if(geom_p1.isEpsilonEqual(topo_p1,0.0001))
			edge->getNodes(v_edge[0], v_edge[1],edge_nodes);
		else if(geom_p1.isEpsilonEqual(topo_p2,0.0001))
			edge->getNodes(v_edge[1], v_edge[0],edge_nodes);
		else {
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
			message << "Problème d'orientation entre l'arête topologique  "
					<< edge->getName()
					<< " et la courbe géométrique"
					<< curve->getName();
			throw TkUtil::Exception (message);
		}


		edge_param_nodes.resize(edge_nodes.size());

		Standard_Real first, last;
		Handle_Geom_Curve occ_curve = BRep_Tool::Curve(occ_edge,first, last);
		int nb_nodes = edge_nodes.size();
#ifdef _DEBUG_MESH
		std::cout<<"("<<first<<", "<<last<<")"<<std::endl;
#endif
		if(first==last)
			nb_nodes = nb_nodes-1;
		for(unsigned int i_nodes=0; i_nodes<nb_nodes; i_nodes++){

			if(i_nodes==0) {
				edge_param_nodes[i_nodes] = first;
				continue;
			}

			if(i_nodes==nb_nodes-1) {
				edge_param_nodes[i_nodes] = last;
				continue;
			}

			gmds::Node ni = edge_nodes[i_nodes];
			gmds::math::Point pi = ni.getPoint();

			gp_Pnt pnt(pi.X(),pi.Y(),pi.Z());
			GeomAPI_ProjectPointOnCurve proj(pnt,occ_curve, first, last);
			proj.Perform(pnt);

			double param_i = proj.LowerDistanceParameter();

//			std::cerr<<"proj.NbPoints() "<<proj.NbPoints()<<" "<<i_nodes<<" "<<param_i<<std::endl;

			gp_Pnt nearest_pt = proj.NearestPoint();
			if(i_nodes==nb_nodes-1)
				edge_param_nodes[i_nodes] = last;
			else
				edge_param_nodes[i_nodes] = param_i;
		}

		e_nodes.push_back(edge_nodes);
		e_param_nodes.push_back(edge_param_nodes);
		//===================================================
		//===================================================
	}

	// on a le maillage de toutes les entites au bord. Maintenant on maille la face
	// On remplit les paramètres de l'algorithme de maillage
	MeshGems::SurfMeshParam smp;
	int face_index = getContext().getLocalGeomManager().getIndexOf(geo_surf);

	for(unsigned int i=0; i<edges.size(); i++){

		Topo::CoEdge*  ei = edges[i];
		Geom::Curve* ci = e_curves[i];

		std::vector<gmds::Node > nodes_i =  e_nodes[i];
		std::vector<double > param_i = e_param_nodes[i];

		const int nb_nodes = nodes_i.size();
		double t  [  nb_nodes];
		double xyz[3*nb_nodes];

		for(unsigned i_n=0; i_n<nb_nodes; i_n++){
			gmds::math::Point pi = nodes_i[i_n].getPoint();
			xyz[3*i_n  ]=pi.X();
			xyz[3*i_n+1]=pi.Y();
			xyz[3*i_n+2]=pi.Z();
			t[i_n]= param_i[i_n];
		}

#ifdef _DEBUG_MESH
		std::cout<<"Curve "<<i<<" -> "<<curves_M3_to_MG[i]<<std::endl;
#endif
		int ret = smp.setEdgeRequiredDiscretization(curves_M3_to_MG[i], nb_nodes, t, xyz);

		if(ret!=0)
			throw TkUtil::Exception (TkUtil::UTF8String ("Impossible de renseigner les paramètres d'une arête pour MeshGems", TkUtil::Charset::UTF_8));
	}

	smp.setFaceToMesh(1,true);
	smp.setFaceSize(1, prop->getSize());

	smp.setMeshGradation(prop->getGradation());

	MeshGems::SurfMesh sm(&meshgems_occ_mng, &smp);
	int ret = sm.generateSurfMesh();
	if(ret!=0)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur durant le maillage surfacique par MeshGems", TkUtil::Charset::UTF_8));

	//===================================================
	// MAILLAGE A TRANSFERER DE MESHGEMS VERS M3D
	//===================================================
	int nb_vertices=0, nb_triangles=0;
	MeshGems::SurfMesh::MeshVertex *v;
	MeshGems::SurfMesh::MeshFace *f;
	ret = sm.getMeshVertices(nb_vertices, &v);
	if(ret!=0)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur MeshGems: impossible de récupérer les sommets", TkUtil::Charset::UTF_8));

	ret = sm.getMeshFaces(nb_triangles, &f);

	if(ret!=0)
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur MeshGems: impossible de récupérer les triangles", TkUtil::Charset::UTF_8));

#ifdef _DEBUG_MESH
	std::cout<<"Nb surface nodes = "<<nb_vertices<<std::endl;
	std::cout<<"Nb surface faces = "<<nb_triangles<<std::endl;
#endif


	// on doit maintenant remplir le maillage
	// maintenant les noeuds sont tous crees, il faut ajouter les
	// polygones aux groupes suivant ce qui a été demandé
	// Tous les sommets créés sont classifiés. Ceux classifiés sur un sommet ou une courbe sont
	// anciens

	std::vector<gmds::TCellID>& nodes = fa->nodes();
	std::vector<gmds::TCellID>& elem = fa->faces();
	//On stocke dans node_ids la correspondance des noeuds nouvellement créés vers les noeuds déjà existant
	std::vector<gmds::TCellID> node_ids;
	node_ids.resize(nb_vertices);

	double epsilon = 1e-5;
	for (unsigned int i=0; i< nb_vertices;i++) {

		double x = v[i].xyz[0];
		double y = v[i].xyz[1];
		double z = v[i].xyz[2];
		Utils::Math::Point p(x,y,z);
		double geom_entity_id = v[i].geom_id;

		gmds::Node nd; // noeud du maillage M3D

		if(v[i].geom_dim ==0) {
			// Ancien noeud de maillage classifié sur un sommet
			// Recherche parmi les sommets topologiques, celui qui a pour index geom_id-1
			std::vector<Topo::Vertex* > vertices = fa->getVertices();
			bool found_node = false;
			for(unsigned int i_v=0; i_v< vertices.size() &&!found_node;i_v++) {
				Topo::Vertex* vi = vertices[i_v];
				Geom::GeomEntity* geom_ei = vi->getGeomAssociation();
				Geom::Vertex* geom_vi = 0;
				if(geom_ei->getDim()!=0)
					throw TkUtil::Exception (TkUtil::UTF8String ("MeshGems requiert des sommets topologiques classifiés sur des points géométriques", TkUtil::Charset::UTF_8));
				else
					geom_vi = dynamic_cast<Geom::Vertex*>(geom_ei);

				int index_i = i_v;//getContext().getLocalGeomManager().getIndexOf(geom_vi)+1;

				// TODO : this should work, instead of the 
				//        coordinate-based mapping
//				if(points_M3_to_MG[index_i] == v[i].geom_id){
//					found_node = true;
//					node_ids[i] = vi->getNode();
//					std::cerr<<"found node "<<i<<" "<<p<<std::endl;
//					std::cerr<<geom_vi->getCoord()<<std::endl;
//				}
//				if((geom_vi->getX() == v[i].xyz[0]) && (geom_vi->getY() == v[i].xyz[1]) && (geom_vi->getZ() == v[i].xyz[2])) {
//				if(std::fabs(geom_vi->getX()-v[i].xyz[0])<0.001 && std::fabs(geom_vi->getY()-v[i].xyz[1])<0.001 && std::fabs(geom_vi->getZ()-v[i].xyz[2])<0.001) {
				if(std::fabs(geom_vi->getX()-v[i].xyz[0])<epsilonLocal && std::fabs(geom_vi->getY()-v[i].xyz[1])<epsilonLocal && std::fabs(geom_vi->getZ()-v[i].xyz[2])<epsilonLocal) {
					found_node = true;
					node_ids[i] = vi->getNode();
				}
			}

			if(!found_node) {
				std::cerr<<"MeshGems: vertex node association to M3D not found "<<v[i].geom_id<<std::endl;
				throw TkUtil::Exception (TkUtil::UTF8String ("MeshGems : l'Appariemment d'un noeud sur sommet a échoué.", TkUtil::Charset::UTF_8));
			}

		}
		else if (v[i].geom_dim ==1){
			// Ancien noeud de maillage classifié sur une courbe
			// Recherche parmi les courbes topologiques, celle qui a pour index geom_id-1
			// puis parcours des noeuds associés et comparaison géométrique
			std::vector<Topo::CoEdge* > co_edges;
			fa->getCoEdges(co_edges);
			bool found_edge = false;
			for(unsigned int i_e=0; i_e<co_edges.size() && !found_edge; i_e++) {
				//===================================================
				//Traitement de l'edge i_e
				//===================================================
				Topo::CoEdge* edge = edges[i_e];

				//on recupere la courbe géométrique
				Geom::GeomEntity* geom_edge = edge->getGeomAssociation();
				Geom::Curve* curve = 0;

				if(geom_edge->getDim()!=1)
					throw TkUtil::Exception (TkUtil::UTF8String ("MeshGems requiert des arêtes topologiques classifiées sur des courbes géométriques", TkUtil::Charset::UTF_8));
				else
					curve = dynamic_cast<Geom::Curve*>(geom_edge);

				int index = i_e;// getContext().getLocalGeomManager().getIndexOf(curve)+1;

				if(curves_M3_to_MG[index] == v[i].geom_id){
					//on a trouvé la bonne courbe
					found_edge = true;

					//on récupère les noeuds de maillage associés
					std::vector<Topo::Vertex* > v_edge = edge->getVertices();
					std::vector<gmds::Node> edge_nodes;
					edge->getNodes(v_edge[0], v_edge[1],edge_nodes);

					//parmi ces noeuds, on cherche celui qui correspond à p
					bool found_node = false;
					for(unsigned int i_v=0; i_v< edge_nodes.size() && !found_node; i_v++) {
						gmds::Node ni = edge_nodes[i_v];
						Utils::Math::Point pi(ni.X(),ni.Y(),ni.Z());
//						if(p.isEpsilonEqual(pi,epsilon)){
						if(std::fabs(p.getX()-pi.getX())<epsilonLocal && std::fabs(p.getY()-pi.getY())<epsilonLocal && std::fabs(p.getZ()-pi.getZ())<epsilonLocal) {
							found_node = true;
							node_ids[i] = ni.getID();
						}
					}

					if(!found_node) {
						std::cerr<<"MeshGems: curve node to M3D association not found "<<v[i].geom_id<<std::endl;
						throw TkUtil::Exception (TkUtil::UTF8String ("MeshGems : l'Appariemment d'un noeud sur courbe a échoué.", TkUtil::Charset::UTF_8));
					}
				} //if(index == v[i].geom_id)

			}//for(unsigned int i_e=0; i_e<co_edges.size() && !found_edge; i_e++)

		} //else if (v[i].geom_dim ==1)
		else if (v[i].geom_dim ==2){
			// Nouveau noeud de maillage

			nd = getGMDSMesh().newNode(x, y, z);
			command->addCreatedNode(nd.getID());
			node_ids[i] =nd.getID();
		}

	}

	for (unsigned int i=0; i< nb_triangles;i++) {
		MeshGems::SurfMesh::MeshFace current_face = f[i];
		int current_nodes[3];
		current_nodes[0] =current_face.v[0]-1;
		current_nodes[1] =current_face.v[1]-1;
		current_nodes[2] =current_face.v[2]-1;
//		std::cout<<"Face "<<i<<" ("
//				<<node_map[current_nodes[0]]<<", "
//				<<node_map[current_nodes[1]]<<", "
//				<<node_map[current_nodes[2]]<<") "<<std::endl;

		gmds::Node fd_nodes[3];
		fd_nodes[0] = getGMDSMesh().get<gmds::Node>(node_ids[current_nodes[0]]) ;
		fd_nodes[1] = getGMDSMesh().get<gmds::Node>(node_ids[current_nodes[1]]) ;
		fd_nodes[2] = getGMDSMesh().get<gmds::Node>(node_ids[current_nodes[2]]) ;
		gmds::Face fd = getGMDSMesh().newTriangle(fd_nodes[0], fd_nodes[1], fd_nodes[2]);
		elem.push_back(fd.getID());
		command->addCreatedFace(fd.getID());
	}

	std::vector<std::string> groupsName;
	fa->getGroupsName(groupsName);

#ifdef _DEBUG_GROUP_BY_TOPO_ENTITY
// on ajoute un groupe pour distinguer les faces en mode debug
	groupsName.push_back(fa->getName());
#endif


	// ajout des polygones aux surfaces
	for (size_t i=0; i<groupsName.size(); i++){
		std::string& nom = groupsName[i];

		try {
			getContext().getLocalMeshManager().getSurface(nom);
			command->addModifiedSurface(nom);
		} catch (...) {
			command->addNewSurface(nom);
		}
		// la surface de maillage que l'on vient de créer/modifier
		Mesh::Surface* sf = getContext().getLocalMeshManager().getSurface(nom);
		sf->saveMeshSurfaceTopoProperty(&command->getInfoCommand());
		sf->addCoFace(fa);
	} // end for i<groupsName.size()


}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif	// USE_MESHGEMS
