/*----------------------------------------------------------------------------*/
/*
 * \file MeshHelper.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7 juin 2019
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Utils/MgxException.h"
#include "Mesh/MeshHelper.h"

#include "GMDS/IG/IGMesh.h"
#include "GMDS/IG/Edge.h"

#include <map>

#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
MeshHelper::MeshHelper( )
{
    MGX_FORBIDDEN("MeshHelper::MeshHelper is not allowed.");
}   // MeshHelper::MeshHelper

/*----------------------------------------------------------------------------*/
MeshHelper::MeshHelper(const MeshHelper&)
{
    MGX_FORBIDDEN("MeshHelper::MeshHelper is not allowed.");
}   // MeshHelper::MeshHelper(const MeshHelper&)

/*----------------------------------------------------------------------------*/
MeshHelper& MeshHelper::operator = (const MeshHelper&)
{
    MGX_FORBIDDEN("MeshHelper::operator = is not allowed.");
    return *this;
}   // MeshHelper::operator =

/*----------------------------------------------------------------------------*/
MeshHelper::~MeshHelper ( )
{
    MGX_FORBIDDEN("MeshHelper::~MeshHelper is not allowed.");
}   // MeshHelper::~MeshHelper
/*----------------------------------------------------------------------------*/
std::vector<uint> MeshHelper::getOrderedNodesId(std::vector<gmds::Edge> bras)
{
	// construction de la table de connectivité nd -> bras

	std::map<uint, std::vector<gmds::Edge*> > nd2br;

	for(std::vector<gmds::Edge>::iterator iter1 = bras.begin();
			iter1 != bras.end(); ++iter1) {
		gmds::Edge& bras = *iter1;
//std::cout<<" bras : "<<bras.getID()<<std::endl;
		std::vector<gmds::Node> br_nodes = bras.get<gmds::Node>();

		for (std::vector<gmds::Node>::iterator iter2 = br_nodes.begin();
				iter2 != br_nodes.end(); ++iter2) {
			gmds::Node& nd = (*iter2);
//std::cout<<"   nd : "<<nd.getID()<<std::endl;
			std::vector<gmds::Edge*>& v_bras = nd2br[nd.getID()];

			v_bras.push_back(&bras);

		} // end for iter2

	} // end for iter1


	// liste des ids des noeuds aux extrémités (ceux avec un seul bras)
	std::vector<uint> dep_nodes;

	for (std::map<uint, std::vector<gmds::Edge*> >::iterator iter3 = nd2br.begin();
			iter3 != nd2br.end(); ++iter3) {
		std::vector<gmds::Edge*>& v_bras = iter3->second;
		if (v_bras.size() == 1)
			dep_nodes.push_back(iter3->first);

		//std::cout<<" pour nd "<<iter3->first<<" avec "<<v_bras.size()<<" bras"<<std::endl;

	} // end for iter3

	//std::cout<<"MeshHelper::getOrderedNodes dep_nodes.size() = "<< dep_nodes.size()<<std::endl;

	if (dep_nodes.size() != 2 && dep_nodes.size() != 0) {
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
		messErr <<"La recherche des sommets extrémité pour une courbe facétisée a échouée, on trouve "<<dep_nodes.size()<<" noeuds reliés à un seul bras";
		throw TkUtil::Exception(messErr);
	}

	// la liste des id des noeuds recherché
	std::vector<uint> nodes_id;

	std::map<uint, uint> filtre_bras;
	uint id_dep = 0;
	uint id_end = 0;
	// pour accepter le cas avec liste vide (cas courbe qui boucle)
	if (dep_nodes.size() == 0){
		// recherche du noeud commun au premier et au dernier bras
		gmds::Edge br1 = bras.front();
		gmds::Edge br2 = bras.back();
		std::vector<gmds::Node> br1_nodes = br1.get<gmds::Node>();
		std::vector<gmds::Node> br2_nodes = br2.get<gmds::Node>();

		if (br1_nodes.size() != 2 || br2_nodes.size() != 2)
			throw TkUtil::Exception("Erreur interne avec bras ayant autre chose que 2 noeuds");

		if (br1_nodes[0] == br2_nodes[0] || br1_nodes[0] == br2_nodes[1])
			id_dep = br1_nodes[0].getID();
		else if (br1_nodes[1] == br2_nodes[0] || br1_nodes[1] == br2_nodes[1])
			id_dep = br1_nodes[1].getID();
		else {
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr <<"La recherche des sommets extrémité pour une courbe facétisée a échouée, les bras extrémités n'ont pas de noeud commun";
			throw TkUtil::Exception(messErr);
		}
		id_end = id_dep;
	}
	else {
		id_dep = dep_nodes[0];
		id_end = dep_nodes[1];
	}

	nodes_id.push_back(id_dep);

	do {
		gmds::Edge bras;
		bool bras_found = false;

		std::vector<gmds::Edge*>& v_bras = nd2br[id_dep];
		for (uint i=0; i<v_bras.size() && !bras_found; i++)
			if (filtre_bras[v_bras[i]->getID()] == 0){
				bras = *v_bras[i];
				filtre_bras[bras.getID()] = 1;
				bras_found = true;
			}
		if (!bras_found){
			TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
			messErr <<"Erreur interne, on ne trouve pas de bras suivant pour constituer la liste discrète pour le noeud "<<(short)id_dep;
			throw TkUtil::Exception(messErr);
		}

		std::vector<gmds::Node> br_nodes = bras.get<gmds::Node>();
		uint id_next = (br_nodes[0].getID() == id_dep?br_nodes[1].getID():br_nodes[0].getID());

		id_dep = id_next;
		nodes_id.push_back(id_dep);

	} while (id_dep != id_end);

	return nodes_id;
}
/*----------------------------------------------------------------------------*/
std::vector<gmds::Node> MeshHelper::getNodes(std::vector<gmds::Edge> bras)
{
	std::vector<gmds::Node> nodes;

	for(std::vector<gmds::Edge>::iterator iter1 = bras.begin();
			iter1 != bras.end(); ++iter1) {
		gmds::Edge& bras = *iter1;
		std::vector<gmds::Node> br_nodes = bras.get<gmds::Node>();

		for (std::vector<gmds::Node>::iterator iter2 = br_nodes.begin();
				iter2 != br_nodes.end(); ++iter2)
			nodes.push_back(*iter2);
	}
	return nodes;
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
