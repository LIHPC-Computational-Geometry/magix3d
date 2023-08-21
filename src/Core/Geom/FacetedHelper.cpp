/*----------------------------------------------------------------------------*/
/*
 * \file FacetedHelper.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/4/2018
 */
/*----------------------------------------------------------------------------*/
#include "Geom/FacetedHelper.h"
#include "Geom/FacetedSurface.h"
#include "Geom/FacetedCurve.h"
#include "Geom/FacetedVertex.h"
#include "Mesh/MeshImplementation.h"
#include "Utils/Vector.h"

#include "GMDS/IG/IGMesh.h"
#include "GMDS/IG/Face.h"
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
void FacetedHelper::computeBoundingBox(const std::vector<gmds::Node>& nodes,
		Utils::Math::Point& pmin,
		Utils::Math::Point& pmax)
{
	std::vector<gmds::Node>::const_iterator itn = nodes.begin ( );

	pmin = Utils::Math::Point((*itn).X(), (*itn).Y(), (*itn).Z());
	pmax = pmin;
	itn++;

	for (; itn != nodes.end ( ); itn++){
		if (pmin.getX() > (*itn).X())
			pmin.setX((*itn).X());
		if (pmax.getX() < (*itn).X())
			pmax.setX((*itn).X());
		if (pmin.getY() > (*itn).Y())
			pmin.setY((*itn).Y());
		if (pmax.getY() < (*itn).Y())
			pmax.setY((*itn).Y());
		if (pmin.getZ() > (*itn).Z())
			pmin.setZ((*itn).Z());
		if (pmax.getZ() < (*itn).Z())
			pmax.setZ((*itn).Z());
	}
}
/*----------------------------------------------------------------------------*/
void FacetedHelper::transform(std::vector<gmds::Node>& nodes,
		gp_Trsf* transf)
{
	for (std::vector<gmds::Node>::iterator itn = nodes.begin ( );
			itn != nodes.end ( ); itn++){
		double x = (*itn).X();
		double y = (*itn).Y();
		double z = (*itn).Z();

		transf->Transforms(x, y, z);

		(*itn).setX(x);
		(*itn).setY(y);
		(*itn).setZ(z);
	}
}
/*----------------------------------------------------------------------------*/
void FacetedHelper::transform(std::vector<gmds::Node>& nodes,
		gp_GTrsf* transf)
{
	for (std::vector<gmds::Node>::iterator itn = nodes.begin ( );
			itn != nodes.end ( ); itn++){
		double x = (*itn).X();
		double y = (*itn).Y();
		double z = (*itn).Z();

		transf->Transforms(x, y, z);

		(*itn).setX(x);
		(*itn).setY(y);
		(*itn).setZ(z);
	}
}
/*----------------------------------------------------------------------------*/
void FacetedHelper::getGMDSNodes(const std::vector<gmds::Face>& poly,
                                 std::vector<gmds::Node >& ANodes)
{
    ANodes.clear();

    // utilisation d'un filtre pour ne pas référencer plusieurs fois un même noeud
    std::map<gmds::TCellID, uint> filtre_nodes;

    for(unsigned int i=0; i<poly.size(); i++) {
        std::vector<gmds::Node> nodes;

        poly[i].get(nodes);
        for(unsigned int iNode=0; iNode<nodes.size(); iNode++) {
            if (filtre_nodes[nodes[iNode].getID()] == 0){
                ANodes.push_back(nodes[iNode]);

                filtre_nodes[nodes[iNode].getID()] = 1;
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void FacetedHelper::buildIsoCurve(const std::vector<gmds::Face>& poly,
                                      std::vector<Utils::Math::Point>& points,
                                      std::vector<size_t>& indices)
{
    //std::cout<<" FacetedHelper::buildIsoCurve avec "<<poly.size()<<" polygones"<<std::endl;
    if (poly.empty())
        return;

    // construit la liste des noeuds
    std::vector<gmds::Node>	nodes;
    getGMDSNodes(poly, nodes);

    // recherche de la boite englobante
    Utils::Math::Point pmin;
    Utils::Math::Point pmax;
    computeBoundingBox(nodes, pmin, pmax);

    // détermine le nombre de section par direction
    uint nbX = 0;
    uint nbY = 0;
    uint nbZ = 0;
    double precision = Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon;
    uint nbNodes = nodes.size();
    uint nbSections = 1+log10(nbNodes);

    double lgX = pmax.getX() - pmin.getX();
    double lgY = pmax.getY() - pmin.getY();
    double lgZ = pmax.getZ() - pmin.getZ();
//    double lgMin = lgX;
    double lgMax = lgX;
    if (lgY>lgMax)
        lgMax = lgY;
    if (lgZ>lgMax)
        lgMax = lgZ;
//    if (lgY<lgMin)
//        lgMin = lgY;
//    if (lgZ<lgMin)
//        lgMin = lgZ;

    if (lgX > lgMax/10)
        nbX = nbSections;
    else if (lgX > precision)
        nbX = 1;

    if (lgY > lgMax/10)
        nbY = nbSections;
    else if (lgY > precision)
        nbY = 1;

    if (lgZ > lgMax/10)
        nbZ = nbSections;
    else if (lgZ > precision)
        nbZ = 1;

//    std::cout<<"nbX = "<<nbX<<std::endl;
//    std::cout<<"nbY = "<<nbY<<std::endl;
//    std::cout<<"nbZ = "<<nbZ<<std::endl;

    // pour une direction donnée, un plan donné, détermine les noeuds d'un côté
    // ou de l'autre, ou à cheval à epsilon près
    std::map<gmds::TCellID, uint> filtre_nodes;
    if (nbX) {
        filtre_nodes.clear();
        std::vector<double> coordPlX;
        for (uint i=0; i<nbX; i++) {
            coordPlX.push_back(pmin.getX() + lgX / (double) (nbX + 1) * (i+1));
            //std::cout <<"coordPlX["<<i<<"] = "<<coordPlX[i]<<std::endl;
        }

        for (std::vector<gmds::Node>::const_iterator itn = nodes.begin ( );
             itn != nodes.end ( ); itn++) {
            Utils::Math::Point pt((*itn).X(), (*itn).Y(), (*itn).Z());
            // recherche de l'interval dans lequel est ce noeud
            uint masqueInterval = 1;
            uint masqueNd = 0;
            for (uint i = 0; i < nbX; i++) {
                if (masqueNd == 0 && (*itn).X() < coordPlX[i]-precision)
                    masqueNd = masqueInterval;
                else if (masqueNd == 0 && (*itn).X() < coordPlX[i]+precision)
                    masqueNd = masqueInterval+1;
                masqueInterval += 2;
            } // end for i
            if (masqueNd == 0)
                masqueNd = masqueInterval;
            filtre_nodes[(*itn).getID()] = masqueNd;
            //std::cout<<" nd "<<(*itn).getID()<<" masque à "<<masqueNd<<std::endl;
        } // end for itn

        // pour les triangles à cheval, déterminer les points d'intersection avec le plan pour un ou deux segments
        for(unsigned int i=0; i<poly.size(); i++) {
            std::vector<gmds::Node> pnodes;
            poly[i].get(pnodes);
            sortNodes(pnodes, filtre_nodes);
            addPointsIsoCurve(pnodes, filtre_nodes, coordPlX, 1, points, indices);
        }

    } // end for nbX

    if (nbY) {
        filtre_nodes.clear();
        std::vector<double> coordPlY;
        for (uint i=0; i<nbY; i++) {
            coordPlY.push_back(pmin.getY() + lgY / (double) (nbY + 1) * (i+1));
            //std::cout <<"coordPlY["<<i<<"] = "<<coordPlY[i]<<std::endl;
        }

        for (std::vector<gmds::Node>::const_iterator itn = nodes.begin ( );
            itn != nodes.end ( ); itn++) {
            Utils::Math::Point pt((*itn).X(), (*itn).Y(), (*itn).Z());
            // recherche de l'interval dans lequel est ce noeud
            uint masqueInterval = 1;
            uint masqueNd = 0;
            for (uint i = 0; i < nbY; i++) {
                if (masqueNd == 0 && (*itn).Y() < coordPlY[i]-precision)
                    masqueNd = masqueInterval;
                else if (masqueNd == 0 && (*itn).Y() < coordPlY[i]+precision)
                    masqueNd = masqueInterval+1;
                masqueInterval += 2;
            } // end for i
            if (masqueNd == 0)
                masqueNd = masqueInterval;
            filtre_nodes[(*itn).getID()] = masqueNd;
            //std::cout<<" nd "<<(*itn).getID()<<" masque à "<<masqueNd<<std::endl;
        } // end for itn

        // pour les triangles à cheval, déterminer les points d'intersection avec le plan pour un ou deux segments
        for(unsigned int i=0; i<poly.size(); i++) {
            std::vector<gmds::Node> pnodes;
            poly[i].get(pnodes);
            sortNodes(pnodes, filtre_nodes);
            addPointsIsoCurve(pnodes, filtre_nodes, coordPlY, 2, points, indices);
        }

    } // end for nbY

    if (nbZ) {
        filtre_nodes.clear();
        std::vector<double> coordPlZ;
        for (uint i=0; i<nbZ; i++) {
            coordPlZ.push_back(pmin.getZ() + lgZ / (double) (nbZ + 1) * (i+1));
            //std::cout <<"coordPlZ["<<i<<"] = "<<coordPlZ[i]<<std::endl;
        }

        for (std::vector<gmds::Node>::const_iterator itn = nodes.begin ( );
             itn != nodes.end ( ); itn++) {
            Utils::Math::Point pt((*itn).X(), (*itn).Y(), (*itn).Z());
            // recherche de l'interval dans lequel est ce noeud
            uint masqueInterval = 1;
            uint masqueNd = 0;
            for (uint i = 0; i < nbZ; i++) {
                if (masqueNd == 0 && (*itn).Z() < coordPlZ[i]-precision)
                    masqueNd = masqueInterval;
                else if (masqueNd == 0 && (*itn).Z() < coordPlZ[i]+precision)
                    masqueNd = masqueInterval+1;
                masqueInterval += 2;
            } // end for i
            if (masqueNd == 0)
                masqueNd = masqueInterval;
            filtre_nodes[(*itn).getID()] = masqueNd;
            //std::cout<<" nd "<<(*itn).getID()<<" masque à "<<masqueNd<<std::endl;
        } // end for itn

        // pour les triangles à cheval, déterminer les points d'intersection avec le plan pour un ou deux segments
        for(unsigned int i=0; i<poly.size(); i++) {
            std::vector<gmds::Node> pnodes;
            poly[i].get(pnodes);
            sortNodes(pnodes, filtre_nodes);
            addPointsIsoCurve(pnodes, filtre_nodes, coordPlZ, 3, points, indices);
        }

    } // end for nbZ

}
/*----------------------------------------------------------------------------*/
void FacetedHelper::sortNodes(std::vector<gmds::Node>& nodes, std::map<gmds::TCellID, uint>& filtre_nodes)
{
    if (nodes.size() != 3)
        throw TkUtil::Exception(TkUtil::UTF8String ("FacetedHelper::sortNodes n'est prévu que pour des triangles.", TkUtil::Charset::UTF_8));

    std::vector<gmds::Node> nodes_tmp;
    for (uint i=0; i<3; i++)
        if (filtre_nodes[nodes[i].getID()] % 2 == 0)
            nodes_tmp.push_back(nodes[i]);
     for (uint i=0; i<3; i++)
        if (filtre_nodes[nodes[i].getID()] % 2 != 0)
            nodes_tmp.push_back(nodes[i]);
    nodes = nodes_tmp;
}
/*----------------------------------------------------------------------------*/
void FacetedHelper::addPointsIsoCurve(std::vector<gmds::Node>& nodes,
        std::map<gmds::TCellID, uint>& filtre_nodes,
        std::vector<double>& coordPl,
        uint dirPlan,
        std::vector<Utils::Math::Point>& points,
        std::vector<size_t>& indices)
{

    // soit un plan passe par 2 sommets du triangles
    // soit par un sommet et 2 autres sommets de part et d'autre du plan
    // soit 1 sommet d'un côté et 2 autres de l'autre
    // soit avec 1 sommet sur un plan et les 2 autres du même côté (pas d'intersection à faire)
    // soit 3 sommets dans une même zone hors plan (pas d'intersection à faire)

    // cas avec noeud 0 sur le plan
    if (filtre_nodes[nodes[0].getID()] % 2 == 0) {
        if (filtre_nodes[nodes[1].getID()] % 2 == 0) {
            // ces 2 sommets sont sur le plan
            indices.push_back(points.size());
            points.push_back (Utils::Math::Point (nodes[0].X(), nodes[0].Y(), nodes[0].Z()));
            indices.push_back(points.size());
            points.push_back (Utils::Math::Point (nodes[1].X(), nodes[1].Y(), nodes[1].Z()));
        }
        else if (filtre_nodes[nodes[1].getID()] != filtre_nodes[nodes[2].getID()]) {
            // cas avec un sommet sur le plan et les 2 autres de part et d'autre
            indices.push_back(points.size());
            points.push_back (Utils::Math::Point (nodes[0].X(), nodes[0].Y(), nodes[0].Z()));
            indices.push_back(points.size());
            // recherche de l'intersection entre les 2 sommets et le plan
            points.push_back (getIntersectionPlan(nodes[1], nodes[2], coordPl[filtre_nodes[nodes[0].getID()]/2-1], dirPlan));
        }
        else {
            // cas d'un sommet sur le plan et les deux autres d'un même côté
            // On ne fait rien
        }
    }
    else {
        if (filtre_nodes[nodes[0].getID()] != filtre_nodes[nodes[1].getID()]){
            indices.push_back(points.size());
            // recherche de l'intersection entre les 2 sommets et le plan
            uint indPl = (filtre_nodes[nodes[0].getID()]+filtre_nodes[nodes[1].getID()])/4-1;
            points.push_back (getIntersectionPlan(nodes[0], nodes[1], coordPl[indPl], dirPlan));
        }
        if (filtre_nodes[nodes[1].getID()] != filtre_nodes[nodes[2].getID()]){
            indices.push_back(points.size());
            // recherche de l'intersection entre les 2 sommets et le plan
            uint indPl = (filtre_nodes[nodes[2].getID()]+filtre_nodes[nodes[1].getID()])/4-1;
            points.push_back (getIntersectionPlan(nodes[1], nodes[2], coordPl[indPl], dirPlan));
        }
        if (filtre_nodes[nodes[0].getID()] != filtre_nodes[nodes[2].getID()]){
            indices.push_back(points.size());
            // recherche de l'intersection entre les 2 sommets et le plan
            uint indPl = (filtre_nodes[nodes[0].getID()]+filtre_nodes[nodes[2].getID()])/4-1;
            points.push_back (getIntersectionPlan(nodes[0], nodes[2], coordPl[indPl], dirPlan));
        }
    }
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point FacetedHelper::getIntersectionPlan(gmds::Node& nd1, gmds::Node& nd2, double plVal, uint &dirPlan)
{
//    std::cout<<"getIntersectionPlan avec plY = "<<plVal<<" entre :"<<std::endl;
//    std::cout<<nd1<<std::endl;
//    std::cout<<nd2<<std::endl;

        if (dirPlan == 1) {
            double ratio = (nd1.X() - plVal) / (nd1.X() - nd2.X());

            return Utils::Math::Point(plVal,
                                      nd1.Y() + ratio * (nd2.Y() - nd1.Y()),
                                      nd1.Z() + ratio * (nd2.Z() - nd1.Z()));
        }
        else if (dirPlan == 2) {
            double ratio = (nd1.Y() - plVal) / (nd1.Y() - nd2.Y());

            return Utils::Math::Point(nd1.X() + ratio * (nd2.X() - nd1.X()),
                                      plVal,
                                      nd1.Z() + ratio * (nd2.Z() - nd1.Z()));
        }
        else if (dirPlan == 3) {
            double ratio = (nd1.Z() - plVal) / (nd1.Z() - nd2.Z());

            return Utils::Math::Point(nd1.X() + ratio * (nd2.X() - nd1.X()),
                                      nd1.Y() + ratio * (nd2.Y() - nd1.Y()),
                                      plVal);
        }
        else
           return Utils::Math::Point();
}
/*----------------------------------------------------------------------------*/
void FacetedHelper::duplicateMesh(Mesh::MeshImplementation* mesh, std::vector<FacetedSurface*>& fs, std::vector<FacetedCurve*>& fc, std::vector<FacetedVertex*>& fv)
{
    uint new_mesh_id = mesh->createNewGMDSMesh();
    uint old_mesh_id;

    // recherche de l'id initial
    if (!fs.empty())
        old_mesh_id = fs[0]->getGMDSID();
    else if (!fc.empty())
        old_mesh_id = fc[0]->getGMDSID();
    else if (!fv.empty())
        old_mesh_id = fv[0]->getGMDSID();
    else
        return; // cas sans rien à faire

    // on vérifie qu'ils ont tous le même maillage de référence
    for (uint i=0; i<fs.size(); i++)
        if (fs[i]->getGMDSID() != old_mesh_id)
            throw TkUtil::Exception(TkUtil::UTF8String ("FacetedHelper::duplicateMesh ne peut fonctionner avec 2 surfaces qui ne référencent pas le même maillage initial.", TkUtil::Charset::UTF_8));
     for (uint i=0; i<fc.size(); i++)
        if (fc[i]->getGMDSID() != old_mesh_id)
            throw TkUtil::Exception(TkUtil::UTF8String ("FacetedHelper::duplicateMesh ne peut fonctionner avec 2 courbes qui ne référencent pas le même maillage initial.", TkUtil::Charset::UTF_8));
      for (uint i=0; i<fv.size(); i++)
        if (fv[i]->getGMDSID() != old_mesh_id)
            throw TkUtil::Exception(TkUtil::UTF8String ("FacetedHelper::duplicateMesh ne peut fonctionner avec 2 sommets qui ne référencent pas le même maillage initial.", TkUtil::Charset::UTF_8));

    gmds::IGMesh& old_mesh = mesh->getGMDSMesh(old_mesh_id);
    gmds::IGMesh& new_mesh = mesh->getGMDSMesh(new_mesh_id);

    // utilisation de filtres sur les noeuds et les faces (polygones) à l'aide de GMDS
    int node_mark = old_mesh.getNewMark<gmds::Node>();
    int face_mark = old_mesh.getNewMark<gmds::Face>();

    // indirection (sur indices des noeuds et faces) pour le cas où tout le maillage ne serait pas dans la surface
    uint max_node_id = old_mesh.getNbNodes();
    uint max_face_id = old_mesh.getNbFaces();
    int reduce_node_id[max_node_id];
    int reduce_face_id[max_face_id];
    for (uint i=0; i<max_node_id; i++)
        reduce_node_id[i] = -1;
    for (uint i=0; i<max_face_id; i++)
        reduce_face_id[i] = -1;

    // le maillage initial
    std::vector<gmds::Face> mesh_faces;
    old_mesh.getAll<gmds::Face>(mesh_faces);
    std::vector<gmds::Node> mesh_nodes;
    old_mesh.getAll<gmds::Node>(mesh_nodes);

    // parcours les polygones et leurs noeuds pour marquer ce qui doit être dupliqué
    for (size_t k = 0; k < fs.size(); k++) {
        std::vector<gmds::Face>& old_faces = fs[k]->getGMDSFaces();
        for (size_t i = 0; i < old_faces.size(); i++) {
            gmds::Face poly = old_faces[i];
            std::vector <gmds::Node> nds = poly.getAll<gmds::Node>();
            old_mesh.mark(poly,face_mark);
            for (uint j=0; j<nds.size(); j++)
                old_mesh.mark(nds[j],node_mark);
        }
    }
    // parcours les noeuds des courbes pour marquer ce qui doit être dupliqué
    for (size_t k = 0; k < fc.size(); k++) {
        std::vector<gmds::Node>& old_nodes = fc[k]->getGMDSNodes();
        for (size_t i = 0; i < old_nodes.size(); i++) {
            gmds::Node nd = old_nodes[i];
            old_mesh.mark(nd,node_mark);
        }
    }
    // parcours les noeuds des sommets pour marquer ce qui doit être dupliqué
    for (size_t k = 0; k < fv.size(); k++) {
        gmds::Node& nd = fv[k]->getGMDSNode();
        old_mesh.mark(nd,node_mark);
    }

    // parcours tous les noeuds, duplique ceux qui sont marqués
     for (uint i=0; i<mesh_nodes.size(); i++) {
         gmds::Node old_nd = mesh_nodes[i];
         if (old_mesh.isMarked(old_nd, node_mark)) {
             reduce_node_id[old_nd.getID()] = new_mesh.getNbNodes();
             gmds::Node new_nd = new_mesh.newNode(old_nd.X(), old_nd.Y(), old_nd.Z());
             //std::cout<<"new_nd depuis old-id "<<old_nd.getID()<<" new-id "<<reduce_node_id[old_nd.getID()]<<" en position "<<new_nd<<std::endl;
         }
     }
    // parcours tous les polygones, duplique ceux qui sont marqués
     for (uint i=0; i<mesh_faces.size(); i++) {
         gmds::Face old_face = mesh_faces[i];
         if (old_mesh.isMarked(old_face, face_mark)) {
             reduce_face_id[old_face.getID()] = new_mesh.getNbFaces();
             std::vector <gmds::TCellID> nds = old_face.getAllIDs<gmds::Node>();
             if (nds.size() != 3)
                 throw TkUtil::Exception(TkUtil::UTF8String ("FacetedHelper::duplicateMesh n'est prévu que pour des triangles.", TkUtil::Charset::UTF_8));

             int nd1 = reduce_node_id[nds[0]];
             int nd2 = reduce_node_id[nds[1]];
             int nd3 = reduce_node_id[nds[2]];
             if (nd1 == -1 || nd2 == -1 || nd3 == -1)
                 throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, FacetedHelper::duplicateMesh a un problème d'indirection non définie sur un noeud.", TkUtil::Charset::UTF_8));

             gmds::Face new_face = new_mesh.newTriangle(nd1, nd2, nd3);
             //std::cout<<"new_face depuis old-id "<<old_face.getID()<<" new-id "<<reduce_face_id[old_face.getID()]<<" avec sommets "<<nd1<<", "<<nd2<<", "<<nd3<<std::endl;
         }
     }

    // MAJ de la liste des polygones dans la surface
    for (size_t k = 0; k < fs.size(); k++) {
        std::vector <gmds::Face> old_faces = fs[k]->getGMDSFaces(); // copie des polygones
        std::vector<gmds::Face>& new_faces = fs[k]->getGMDSFaces();
        new_faces.clear();

        for (uint i = 0; i < old_faces.size(); i++) {
            gmds::Face poly = old_faces[i];
            int fa = reduce_face_id[poly.getID()];
            if (fa == -1)
                throw TkUtil::Exception(TkUtil::UTF8String(
                        "Erreur interne, FacetedHelper::duplicateMesh a un problème d'indirection non définie sur un polygone.",
                        TkUtil::Charset::UTF_8));
            new_faces.push_back(new_mesh.get<gmds::Face>(fa));
        }
    }

    // mise à jour des listes de noeuds pour les courbes
    for (uint i=0; i<fc.size(); i++) {
        FacetedCurve* old_crv = fc[i];

        std::vector<gmds::Node> old_nodes = old_crv->getGMDSNodes();
        std::vector<gmds::Node>& new_nodes = old_crv->getGMDSNodes();
        new_nodes.clear();

        for (uint j=0; j<old_nodes.size(); j++) {
            int nd1 = reduce_node_id[old_nodes[j].getID()];
            if (nd1 == -1)
                throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, FacetedHelper::duplicateMesh a un problème d'indirection non définie sur un noeud d'une courbe.", TkUtil::Charset::UTF_8));
            new_nodes.push_back(new_mesh.get<gmds::Node>(nd1));
        }
    }

     // mise à jour des noeuds pour les sommets
    for (uint i=0; i<fv.size(); i++) {
        FacetedVertex* old_vtx = fv[i];

        gmds::Node old_node = old_vtx->getGMDSNode();
        gmds::Node& new_node = old_vtx->getGMDSNode();

        int nd1 = reduce_node_id[old_node.getID()];
        if (nd1 == -1)
            throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, FacetedHelper::duplicateMesh a un problème d'indirection non définie sur un noeud d'un sommet.", TkUtil::Charset::UTF_8));
        new_node = new_mesh.get<gmds::Node>(nd1);
    }

    // annule les marques
    for (uint i=0; i<mesh_faces.size(); i++)
        old_mesh.unmark(mesh_faces[i],face_mark);
    old_mesh.freeMark<gmds::Face>(face_mark);

    for (uint i=0; i<mesh_nodes.size(); i++)
        old_mesh.unmark(mesh_nodes[i],node_mark);
    old_mesh.freeMark<gmds::Node>(node_mark);

    // MAJ de la référence sur l'id du nouveau maillage
    for (size_t i = 0; i < fs.size(); i++)
        fs[i]->setGMDSID(new_mesh_id);
    for (uint i=0; i<fc.size(); i++)
        fc[i]->setGMDSID(new_mesh_id);
    for (uint i=0; i<fv.size(); i++)
        fv[i]->setGMDSID(new_mesh_id);
}
/*----------------------------------------------------------------------------*/
void FacetedHelper::computeOrdonatedNodes(gmds::Node first_node, std::vector<FacetedCurve*>& fc, std::vector<gmds::Node>& nodes)
{
    nodes.push_back(first_node);
    for (uint i=0; i<fc.size(); i++){
        std::vector<gmds::Node>& loc_nodes = fc[i]->getGMDSNodes();

        if (loc_nodes.front() == first_node){
            std::cout<<"même sens"<<std::endl;
            for (uint j=1; j<loc_nodes.size(); j++)
                nodes.push_back(loc_nodes[j]);
            first_node = nodes.back();
        }
        else if (loc_nodes.back() == first_node){
            std::cout<<"sens inverse"<<std::endl;
            for (uint j=1; j<loc_nodes.size(); j++)
                nodes.push_back(loc_nodes[loc_nodes.size()-j-1]);
            first_node = nodes.back();
        }
        else {
            throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne, FacetedHelper::computeOrdonatedNodes ne trouve pas de continuité entre 2 courbes", TkUtil::Charset::UTF_8));
        }
    }
}
/*----------------------------------------------------------------------------*/
void FacetedHelper::transformOffset(Mesh::MeshImplementation* mesh, FacetedSurface* fs, double& offset)
{
    // récupération du maillage GMDS et de quelques informations
    uint mesh_id = fs->getGMDSID();
    gmds::IGMesh& gmds_mesh = mesh->getGMDSMesh(mesh_id);
    uint max_face_id = gmds_mesh.getMaxLocalID(2);
    uint max_node_id = gmds_mesh.getMaxLocalID(0);

    // calcul de la normale pour chacun des polygones

    // on commence par allouer un tableau de vecteurs pour les normales, une par id de polygone
    Utils::Math::Vector* normales = new Utils::Math::Vector[max_face_id+1];
    // comme le modèle GMDS utilisé n'a pas activé la connectivité N2F, on construit la table de relations
    std::vector<uint>* node_to_face = new std::vector<uint>[max_node_id+1];

    std::vector<gmds::Face>& faces = fs->getGMDSFaces();
    for (size_t i = 0; i < faces.size(); i++) {
        gmds::Face poly = faces[i];
        std::vector <gmds::Node> nds = poly.getAll<gmds::Node>();
        if (nds.size() != 3)
            throw TkUtil::Exception(TkUtil::UTF8String ("On ne sait pas traiter les surfaces facétisées avec autre chose que des triangles", TkUtil::Charset::UTF_8));
        Utils::Math::Vector v1(nds[1].X()-nds[0].X(), nds[1].Y()-nds[0].Y(), nds[1].Z()-nds[0].Z());
        Utils::Math::Vector v2(nds[2].X()-nds[0].X(), nds[2].Y()-nds[0].Y(), nds[2].Z()-nds[0].Z());

        // le vecteur normal n'est pas normalisé pour tenir compte de la surface ce qui permettra de pondérer la normale en un noeud
        normales[poly.getID()] = v1*v2;
        //std::cout<<"normales pour "<<poly.getID()<<" : "<< normales[poly.getID()]<<std::endl;

        for (uint j=0; j<3; j++)
            node_to_face[nds[j].getID()].push_back(poly.getID());
    }

    // pour chacun des noeuds de la surface, déplacement en fonction de la normale des polygones adjacents
    std::vector<gmds::Node> nodes;
    getGMDSNodes(faces, nodes);

    for (uint i=0; i<nodes.size(); i++){
        gmds::Node node = nodes[i];
        std::vector<uint> l_faces_id = node_to_face[node.getID()];
        //std::cout<<"noeud "<<node.getID()<<" avec "<<l_faces_id.size()<<" polygones"<<std::endl;
        // la somme des normales des pomlygones adjacents
        Utils::Math::Vector vect;
        for (uint j=0; j<l_faces_id.size(); j++)
            vect+=normales[l_faces_id[j]];
        // on normalise le vecteur final pour un noeud
        vect/=vect.abs();
        // on déplace le noeud
        node.setX(node.X()+vect.getX()*offset);
        node.setY(node.Y()+vect.getY()*offset);
        node.setZ(node.Z()+vect.getZ()*offset);
   }

    delete [] normales;
    delete [] node_to_face;

    // buildAABBTree à refaire
    fs->update();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
