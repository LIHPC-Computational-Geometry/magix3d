/*----------------------------------------------------------------------------*/
/*
 * \file MeshImplementationStructured.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18 oct. 2013
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/MeshImplementation.h"
#include "Mesh/CommandCreateMesh.h"

#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/Edge.h"
#include "Topo/TopoHelper.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyTabulated.h"
#include "Topo/FaceMeshingPropertyDirectional.h"
#include "Topo/FaceMeshingPropertyOrthogonal.h"
#include "Topo/FaceMeshingPropertyRotational.h"
#include "Topo/BlockMeshingData.h"
#include "Topo/BlockMeshingPropertyRotational.h"
#include "Topo/BlockMeshingPropertyDirectional.h"
#include "Topo/BlockMeshingPropertyOrthogonal.h"
#include "Topo/TopoHelper.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Internal/Context.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/Surface.h"
#include "Geom/OCCGeomRepresentation.h"
/*----------------------------------------------------------------------------*/
/// TkUtil
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
/// OCC
#include <Standard_Failure.hxx>
#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Quaternion.hxx>
/*----------------------------------------------------------------------------*/
// qualif
#include <GQualif/QualifHelper.h>
#include "Vecteur.h"
#include "Triangle.h"
#include "Quadrangle.h"
#include "Hexaedre.h"
#include "Prisme.h"
#include "Tetraedre.h"
#include "Pyramide.h"
#include <TkUtil/Timer.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
//#define _DEBUG_MESH_FUNCTION
//#define _DEBUG_MESH
//#define _DEBUG_GROUP_BY_TOPO_ENTITY
//#define _DEBUG_TIMER
/*----------------------------------------------------------------------------*/
void MeshImplementation::preMeshStrutured(Topo::Block* bl)
{
#ifdef _DEBUG_MESH_FUNCTION
	{
    std::cout <<"Maillage du bloc structuré "<<bl->getName()<<" avec la méthode "
            << bl->getMeshLawName();
    if (bl->getMeshLaw() == Topo::BlockMeshingProperty::rotational
            || bl->getMeshLaw() == Topo::BlockMeshingProperty::directional
			|| bl->getMeshLaw() == Topo::BlockMeshingProperty::orthogonal)
        std::cout << " et direction "<<(short)bl->getBlockMeshingProperty()->getDir();
    std::cout << std::endl;

    std::vector<std::string> groupsName;
    bl->getGroupsName(groupsName);
    std::cout << " GroupsName :";
    for (uint i=0; i<groupsName.size(); i++)
    	std::cout << " "<<groupsName[i];
    std::cout << std::endl;
	}
#endif
    if (!bl->isStructured()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "Maillage structuré impossible, pour le bloc \""
                << bl->getName() << "\", il n'est pas dit structuré";
        throw TkUtil::Exception (message);
    }

#ifdef _DEBUG_TIMER
    TkUtil::Timer timer(true);
#endif

	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 <<"Maillage du bloc structuré "<<bl->getName()<<" avec la méthode "
            << bl->getMeshLawName();
    if (bl->getMeshLaw() == Topo::BlockMeshingProperty::rotational
            || bl->getMeshLaw() == Topo::BlockMeshingProperty::directional
			|| bl->getMeshLaw() == Topo::BlockMeshingProperty::orthogonal)
        message1 << " et direction "<<(short)bl->getBlockMeshingProperty()->getDir();
    std::vector<std::string> groupsName;
    bl->getGroupsName(groupsName);
    message1 << "\n GroupsName :";
    for (uint i=0; i<groupsName.size(); i++)
    	message1 << " "<<groupsName[i];
    message1 << "\n";
    getContext().getLogStream()->log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_5, __FILE__, __LINE__));

    // Cas structuré, Transfini

    uint nbBrasI, nbBrasJ, nbBrasK;
    bl->getNbMeshingEdges(nbBrasI, nbBrasJ, nbBrasK);
#ifdef _DEBUG_MESH
    std::cout<<" getNbMeshingEdges => nbBrasI,J,K: "<<nbBrasI<<", "<<nbBrasJ<<", "<<nbBrasK<<std::endl;
#endif

    const uint nbNoeudsI = nbBrasI + 1;
    const uint nbNoeudsJ = nbBrasJ + 1;
    const uint nbNoeudsK = nbBrasK + 1;

    // allocation du tableau pour les points
    Utils::Math::Point* l_points = new Utils::Math::Point[nbNoeudsI*nbNoeudsJ*nbNoeudsK];
    bl->points() = l_points;

    // on va mettre en premier dans le tableau des id, ceux du bord
    bl->nodes().resize(nbNoeudsI*nbNoeudsJ*nbNoeudsK);

#ifdef _DEBUG_MESH
    for (uint i=0; i<nbNoeudsI*nbNoeudsJ*nbNoeudsK; i++)
        bl->nodes()[i] = 0;
#endif

    // boucle sur les 5 ou 6 faces
    for (uint cote=0; cote<6; cote++) {
        uint iblocdep, iblocpas, jblocdep, jblocpas;
        uint ifacesize, jfacesize;

        // décalage pour passage des indices de la face à ceux du bloc
        switch (cote) {
        case 0: // i_min
            iblocdep = 0;                                       iblocpas = (nbNoeudsI);
            jblocdep = 0;                                       jblocpas = (nbNoeudsI)*(nbNoeudsJ);
            ifacesize = nbNoeudsJ; jfacesize = nbNoeudsK;
            break;
        case 1: // i_max
            iblocdep = nbBrasI;                                 iblocpas = (nbNoeudsI);
            jblocdep = 0;                                       jblocpas = (nbNoeudsI)*(nbNoeudsJ);
            ifacesize = nbNoeudsJ; jfacesize = nbNoeudsK;
            break;
        case 2: // j_min
            iblocdep = 0;                                       iblocpas = 1;
            jblocdep = 0;                                       jblocpas = (nbNoeudsI)*(nbNoeudsJ);
            ifacesize = nbNoeudsI; jfacesize = nbNoeudsK;
            break;
        case 3: // j_max
            iblocdep = (nbNoeudsI)*(nbBrasJ);                   iblocpas = 1;
            jblocdep = 0;                                       jblocpas = (nbNoeudsI)*(nbNoeudsJ);
            ifacesize = nbNoeudsI; jfacesize = nbNoeudsK;
            break;
        case 4: // k_min
            iblocdep = 0;                                       iblocpas = 1;
            jblocdep = 0;                                       jblocpas = (nbNoeudsI);
            ifacesize = nbNoeudsI; jfacesize = nbNoeudsJ;
            break;
        case 5: // k_max
            iblocdep = (nbNoeudsI)*(nbNoeudsJ)*(nbBrasK);       iblocpas = 1;
            jblocdep = 0;                                       jblocpas = (nbNoeudsI);
            ifacesize = nbNoeudsI; jfacesize = nbNoeudsJ;
            break;
        }

        // cas pour les faces non-dégénérées (celles qui existent)
        if (cote<bl->getNbFaces()){
            Topo::Face* face = bl->getFace(cote);


            // nombre de points dans les 2 directions locales de la face
            uint nbPtI = 0;
            uint nbPtJ = 0;
            face->getNbMeshingEdges(nbPtI, nbPtJ);
            nbPtI += 1;
            nbPtJ += 1;

            if (bl->getNbVertices() == 8){
                Topo::Face::eDirOnFace iDir = face->getDir(bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnBlock[cote][1]),
                        bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnBlock[cote][2]));
                Topo::Face::eDirOnFace jDir = face->getDir(bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnBlock[cote][1]),
                        bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnBlock[cote][0]));
                if (iDir == Topo::Face::j_dir){
                    // on permute les nombres
                    int tmp = nbPtI;
                    nbPtI = nbPtJ;
                    nbPtJ = tmp;
                }
            }
            else if (face->getNbVertices() == 4) {
            	assert(cote<5);
                Topo::Face::eDirOnFace iDir = face->getDir(bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnDegBlock[cote][1]),
                        bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnDegBlock[cote][2]));
                Topo::Face::eDirOnFace jDir = face->getDir(bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnDegBlock[cote][1]),
                        bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnDegBlock[cote][0]));
                if (iDir == Topo::Face::j_dir){
                    // on permute les nombres
                    int tmp = nbPtI;
                    nbPtI = nbPtJ;
                    nbPtJ = tmp;
                }
            }
            else if (face->getNbVertices() == 3) {
                // l'ordre est bon ...
            }
            else
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, MeshImplementation::meshStrutured pour nbPtI et nbPtJ", TkUtil::Charset::UTF_8));


            std::vector<gmds::Node> nodes;

            // cas sans/avec dégénéréscence, on tient compte du fait que la face si elle est dégénérée
            // elle l'est sur sa dernière arête
            if (face->getNbVertices() == 4)
                // demander les points de la face avec respect de l'ordre des sommets internes du bloc
                if (bl->getNbVertices() == 8)
                    face->getNodes(bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnBlock[cote][0]),
                            bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnBlock[cote][1]),
                            bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnBlock[cote][2]),
                            bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnBlock[cote][3]),
                            nodes);
                else {
                	if (cote >= 5)
                		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, MeshImplementation::meshStrutured, cote trop grand", TkUtil::Charset::UTF_8));
                	face->getNodes(bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnDegBlock[cote][0]),
                            bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnDegBlock[cote][1]),
                            bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnDegBlock[cote][2]),
                            bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnDegBlock[cote][3]),
                            nodes);
                }
            else {
            	if (cote == 5)
            		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne avec cote == 5 alors qu'il y a une face dégénérée", TkUtil::Charset::UTF_8));

                // recherche du sens
                if (bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnDegBlock[cote][1]) == face->getVertex(1)
                        && bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnDegBlock[cote][2]) == face->getVertex(2))
                    face->getNodes(0, true, nodes);
                else if (bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnDegBlock[cote][1]) == face->getVertex(2)
                        && bl->getVertex(Topo::TopoHelper::tabIndVtxByFaceOnDegBlock[cote][2]) == face->getVertex(1))
                    face->getNodes(3, false, nodes);
                else {
					TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                    message << "Erreur interne dans MeshImplementation::meshStrutured pour le bloc"
                            << bl->getName() << ", avec la face "
                            << face->getName() << ", pb avec recherche du sens";
                    throw TkUtil::Exception (message);
                }

            }

            // remplissage des coordonnées aux extrémités
            for(uint jface=0, jbloc=jblocdep; jface<jfacesize; jface++, jbloc+=jblocpas)
                for(uint iface=0, ibloc=iblocdep; iface<ifacesize; iface++, ibloc+=iblocpas){
#ifdef _DEBUG2
                	if (iface+jface*ifacesize >= nodes.size())
                	{
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                		message << "Erreur interne dans MeshImplementation::meshStrutured pour le bloc"
                				<< bl->getName() <<" de taille "<< (short)nbNoeudsI<<" X "<< (short)nbNoeudsJ<<" X "<< (short)nbNoeudsK
                				<<", avec la face "
                				<< face->getName() <<" de taille "<<(short)nbPtI<<" X "<<(short)nbPtJ
                				<< ", en dehors des limites pour nodes de taille "<<nodes.size()
                				<<" alors que l'on cherche à obtenir l'indice "<<(long)iface+jface*ifacesize;
                		throw TkUtil::Exception (message);
                	}
#endif
                	bl->nodes()[ibloc+jbloc] = nodes[iface+jface*ifacesize].getID();
                    l_points[ibloc+jbloc] = getCoordNode(nodes[iface+jface*ifacesize]);
                    //                        std::cout<<"bl->nodes["<<ibloc+jbloc<<"] = "
                    //                                << getInfo(nodes[iface+jface*ifacesize])
                    //                                << std::endl;
                }
        }
        else {
            // cas de la dernière face dégénérées
            // 3 cas possibles: en un point ou suivant l'un des 2 côtés

            if (bl->getNbVertices() == 5){
                // copie le noeud au sommet
                gmds::Node node = getGMDSMesh().get<gmds::Node>(bl->getVertex(4)->getNode());
                Utils::Math::Point pt = getCoordNode(node);
                for(uint jface=0, jbloc=jblocdep; jface<jfacesize; jface++, jbloc+=jblocpas)
                    for(uint iface=0, ibloc=iblocdep; iface<ifacesize; iface++, ibloc+=iblocpas){
                        bl->nodes()[ibloc+jbloc] = node.getID();
                        l_points[ibloc+jbloc] = pt;
                    }
            }
            else if (bl->getFace(0)->getNbVertices() == 3
                    && bl->getFace(1)->getNbVertices() == 3
                    && bl->getFace(2)->getNbVertices() == 4
                    && bl->getFace(3)->getNbVertices() == 4) {

                // La face dégénérée étant la dernière, on utilise ce qui a été
                // remplit par les autres faces (les arêtes de cette face) pour remplir l'intérieur

                for(uint jface=0, jbloc=jblocdep; jface<jfacesize; jface++, jbloc+=jblocpas)
                    for(uint iface=0, ibloc=iblocdep; iface<ifacesize; iface++, ibloc+=iblocpas){
                        bl->nodes()[ibloc+jbloc] = bl->nodes()[ibloc+jblocdep];
                        l_points[ibloc+jbloc] = l_points[ibloc+jblocdep];
                    }

            }
            else if (bl->getFace(0)->getNbVertices() == 4
                    && bl->getFace(1)->getNbVertices() == 4
                    && bl->getFace(2)->getNbVertices() == 3
                    && bl->getFace(3)->getNbVertices() == 3) {

                for(uint jface=0, jbloc=jblocdep; jface<jfacesize; jface++, jbloc+=jblocpas)
                    for(uint iface=0, ibloc=iblocdep; iface<ifacesize; iface++, ibloc+=iblocpas){
                        bl->nodes()[ibloc+jbloc] = bl->nodes()[iblocdep+jbloc];
                        l_points[ibloc+jbloc] = l_points[iblocdep+jbloc];
                    }

            }
            else {
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message << "Erreur interne dans MeshImplementation::meshStrutured pour le bloc"
                        << bl->getName() << ", les faces dégénérées ne rentre pas dans l'une des 3 possibilitées";
                throw TkUtil::Exception (message);
            } // else ... (bl->getNbVertices() == 5)

        } // end else (cote<bl->getNbFaces())
    } // end for cote<6

    if (bl->getMeshLaw() == Topo::BlockMeshingProperty::transfinite){
        discretiseTransfinie(nbBrasI, nbBrasJ, nbBrasK, l_points);
    }
    else if (bl->getMeshLaw() == Topo::BlockMeshingProperty::rotational){

        Topo::BlockMeshingPropertyRotational* bmp = dynamic_cast<Topo::BlockMeshingPropertyRotational*>(bl->getBlockMeshingProperty());
        CHECK_NULL_PTR_ERROR(bmp);
        Utils::Math::Point axis1;
        Utils::Math::Point axis2;
        bmp->getAxis(axis1, axis2);
        uint dir = bmp->getDir();

        discretiseRotation(nbBrasI, nbBrasJ, nbBrasK, l_points, axis1, axis2, dir);

    }
    else if (bl->getMeshLaw() == Topo::BlockMeshingProperty::directional
			|| bl->getMeshLaw() == Topo::BlockMeshingProperty::orthogonal){
        Topo::BlockMeshingPropertyDirectional* bmp = dynamic_cast<Topo::BlockMeshingPropertyDirectional*>(bl->getBlockMeshingProperty());
        CHECK_NULL_PTR_ERROR(bmp);
        uint dir = bmp->getDir();
        Topo::CoEdgeMeshingProperty *empDir[3];
        uint nbBrasDir[3];
        nbBrasDir[0] = nbBrasI;
        nbBrasDir[1] = nbBrasJ;
        nbBrasDir[2] = nbBrasK;

        std::vector<Topo::CoEdge* > iCoedges[3];
        bl->getOrientedCoEdges(iCoedges[0], iCoedges[1], iCoedges[2]);

        // recherche des ratios par arête dans le bloc
        std::map<Topo::CoEdge*,uint> ratios;
        bl->getRatios(ratios);

        // construction d'un tableau avec les 8 sommets en duplicant les sommets en cas de dégénérescence
        std::vector<Topo::Vertex* > sommets;
        bl->getHexaVertices(sommets);

        uint dir_bl = bl->getBlockMeshingProperty()->getDir();
        for (uint i=0; i<3; i++)
            if (dir_bl != i)
                empDir[i] = new Topo::EdgeMeshingPropertyUniform(nbBrasDir[i]);
            else
                empDir[i] = 0;

        // recherche parmis les arêtes suivant la direction, si l'une d'elle ne serait composée que d'1 CoEdge
        for (uint j=0; j<4; j++){
            Topo::Vertex* vtx1 = sommets[Topo::TopoHelper::tabIndVtxByEdgeAndDirOnBlock[bl->getBlockMeshingProperty()->getDir()][j][0]];
            Topo::Vertex* vtx2 = sommets[Topo::TopoHelper::tabIndVtxByEdgeAndDirOnBlock[bl->getBlockMeshingProperty()->getDir()][j][1]];

            if (vtx1 != vtx2 && empDir[dir_bl] == 0){
                std::vector<Topo::CoEdge* > coedges_between;
                Topo::TopoHelper::getCoEdgesBetweenVertices(vtx1, vtx2, iCoedges[dir_bl], coedges_between);

                if (coedges_between.size() == 1){

                    // recherche s'il n'y aurait pas un déraffinement de cette arête
                    // si c'est le cas, on ne réutilise pas MeshingProperty de cette arête.
                    Topo::CoEdge* coedge = coedges_between[0];

                    if (ratios[coedge] == 1){
                        empDir[dir_bl] = coedge->getMeshingProperty()->clone();

                        if (coedge->getVertex(0) != vtx1)
                            empDir[dir_bl]->setDirect(!empDir[dir_bl]->getDirect());
#ifdef _DEBUG_MESH
                        std::cout<<" réutilisation de la discrétisation de l'arête "<<coedge->getName()<<std::endl;
                        std::cout<<" vtx1 = "<<vtx1->getName()<<", vtx2 = "<<vtx2->getName()<<std::endl;
                        std::cout<<" arête dont on prend la discrétisation: "<<*coedge;
#endif
                    }
                }
            }
        }
        if (empDir[dir_bl] == 0){
        	// recherche des points qui correspondent à la discrétisation de l'"arête" du bloc
        	std::vector<Utils::Math::Point> edge_points;
            for (uint j=0; j<4; j++){
                Topo::Vertex* vtx1 = sommets[Topo::TopoHelper::tabIndVtxByEdgeAndDirOnBlock[bl->getBlockMeshingProperty()->getDir()][j][0]];
                Topo::Vertex* vtx2 = sommets[Topo::TopoHelper::tabIndVtxByEdgeAndDirOnBlock[bl->getBlockMeshingProperty()->getDir()][j][1]];

                if (vtx1 != vtx2 && edge_points.empty()){
                    std::vector<Topo::CoEdge* > coedges_between;
                    Topo::TopoHelper::getCoEdgesBetweenVertices(vtx1, vtx2, iCoedges[dir_bl], coedges_between);
                    Topo::TopoHelper::getPoints(vtx1, vtx2, coedges_between, ratios, edge_points);
                }
            }

            if (edge_points.empty()){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            	message << "Le bloc "<<bl->getName() <<" ne respecte certainement pas le type de discrétisation\n";
            	message << "du fait d'un découpage de toutes ses arêtes communes ou d'un déraffinement";
            	getContext().getLogStream()->log(TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));

            	empDir[dir_bl] = new Topo::EdgeMeshingPropertyUniform(nbBrasDir[dir_bl]);
            }
            else {
            	// calcul des longueurs des bras
            	std::vector<double> tabulation;
            	for (uint i=1; i<edge_points.size(); i++)
            		tabulation.push_back(edge_points[i].length(edge_points[i-1]));

                // création d'une discrétisation basée sur les points des discrétisations des arêtes
            	empDir[dir_bl] = new Topo::EdgeMeshingPropertyTabulated(tabulation);
            }
        }

        if (bl->getMeshLaw() == Topo::BlockMeshingProperty::orthogonal){
        	Topo::BlockMeshingPropertyOrthogonal* bmp2 = dynamic_cast<Topo::BlockMeshingPropertyOrthogonal*>(bmp);
        	CHECK_NULL_PTR_ERROR(bmp2);
        	uint side = bmp2->getSide();
        	uint nbLayers = bmp2->getNbLayers();
        	// recherche de la surface / laquelle il faut être orthogonal
        	Geom::Surface* surface = 0;
        	Topo::Face* face = bl->getFace(dir*2+side);
        	std::vector<Topo::CoFace*> cofaces;
        	face->getCoFaces(cofaces);
        	for (uint i=0; i<cofaces.size(); i++)
        		if (cofaces[i]->getGeomAssociation() != 0)
        			surface = dynamic_cast<Geom::Surface*>(cofaces[i]->getGeomAssociation());
        	discretiseOrthogonalPuisCourbe(empDir[0], empDir[1], empDir[2], l_points, dir, side, nbLayers, surface);
        }
        else
        	discretiseDirection(empDir[0], empDir[1], empDir[2], l_points, dir);

        for (uint i=0; i<3; i++)
            delete empDir[i];
    } else {
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans MeshImplementation::meshStrutured pour block, type de maillage invalide", TkUtil::Charset::UTF_8));
    }
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"création des points d'un bloc "<<timer.cpuDuration()/TkUtil::Timer::cpuPerSecond()<<" secondes"<<std::endl;
    timer.reset();
    timer.start();
#endif

} // preMeshStrutured(Block*)
/*----------------------------------------------------------------------------*/
void MeshImplementation::meshStrutured(Mesh::CommandCreateMesh* command, Topo::Block* bl)
{
#ifdef _DEBUG_TIMER
    TkUtil::Timer timer(true);
#endif

    if (!bl->isPreMeshed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message << "Erreur interne, les points du maillage structuré non précalculés, pour le bloc \""
    			<< bl->getName()<<"\"";
    	throw TkUtil::Exception (message);
    }

    uint nbBrasI, nbBrasJ, nbBrasK;
    bl->getNbMeshingEdges(nbBrasI, nbBrasJ, nbBrasK);
#ifdef _DEBUG_MESH
    std::cout<<" getNbMeshingEdges => nbBrasI,J,K: "<<nbBrasI<<", "<<nbBrasJ<<", "<<nbBrasK<<std::endl;
#endif

    const uint nbNoeudsI = nbBrasI + 1;
    const uint nbNoeudsJ = nbBrasJ + 1;
    const uint nbNoeudsK = nbBrasK + 1;

    for (uint i=1; i<nbBrasI; i++)
        for (uint j=1; j<nbBrasJ; j++)
            for (uint k=1; k<nbBrasK; k++) {
                Utils::Math::Point &pt = bl->points()[i+nbNoeudsI*j+k*nbNoeudsI*nbNoeudsJ];
                gmds::Node nd = getGMDSMesh().newNode(pt.getX(), pt.getY(), pt.getZ());
                bl->nodes()[i+nbNoeudsI*j+k*nbNoeudsI*nbNoeudsJ] = nd.getID();
                command->addCreatedNode(nd.getID());
            }

    delete [] bl->points();
    bl->points() = 0;
    bl->getMeshingData()->setPreMeshed(false);

#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"création des noeuds d'un bloc "<<timer.cpuDuration()/TkUtil::Timer::cpuPerSecond()<<" secondes"<<std::endl;
    timer.reset();
    timer.start();
#endif

//#ifdef _DEBUG_MESH
//    std::cout<<"MeshImplementation::meshStrutured () avec comme nbNoeuds en I, J et K: "
//            <<nbNoeudsI<<", "<<nbNoeudsJ<<", "<<nbNoeudsK<<std::endl;
//    for (uint kk=0; kk<nbNoeudsK; kk++)
//        for (uint jj=0; jj<nbNoeudsJ; jj++)
//            for (uint ii=0; ii<nbNoeudsI; ii++){
//                Node* nd = bl->nodes()[(ii)+nbNoeudsI*(jj)+nbNoeudsI*nbNoeudsJ*(kk)];
//                std::cout<<"Node("<<ii<<","<<jj<<","<<kk<<") = ";
//                if (nd)
//                    std::cout<<"["<<nd->getX()<<", "<<nd->getY()<<", "<<nd->getZ()<<"]";
//                else
//                    std::cout<<"Pointeur NULL !";
//                std::cout<<std::endl;
//            }
//#endif

    // ajoute les polyedres aux groupes suivant ce qui a été demandé
    _addRegionsInVolumes(command, bl, nbBrasI, nbBrasJ, nbBrasK);

#ifdef _DEBUG_TIMER
   timer.stop();
    std::cout<<"création des polyèdres et volumes d'un bloc "<<timer.cpuDuration()/TkUtil::Timer::cpuPerSecond()<<" secondes"<<std::endl;
#endif

} // meshStrutured (Block*)
/*----------------------------------------------------------------------------*/
void MeshImplementation::preMeshStrutured(Topo::CoFace* coface)
{
    if (!coface->isStructured()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
        message << "Maillage structuré impossible, pour la face commune \""
                << coface->getName() << "\", elle n'est pas dite structurée";
        throw TkUtil::Exception (message);
    }

#ifdef _DEBUG_MESH
    std::cout <<"Maillage de la face commune structurée "<<coface->getName()<<" avec la méthode "
            << coface->getMeshLawName()<<std::endl;
#endif

#ifdef _DEBUG_TIMER
    TkUtil::Timer timer(true);
#endif

	TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
    message1 <<"Maillage de la face structurée "<<coface->getName()<<" avec la méthode "
            << coface->getMeshLawName();
    getContext().getLogStream()->log (TkUtil::TraceLog (message1, TkUtil::Log::TRACE_5, __FILE__, __LINE__));


    // Cas structuré

    uint idep[4];
    uint ipas[4];

    uint nbBrasI, nbBrasJ;
    coface->getNbMeshingEdges(nbBrasI, nbBrasJ);

    const uint nbNoeudsI = nbBrasI + 1;
    const uint nbNoeudsJ = nbBrasJ + 1;

    // on va mettre en premier dans le tableau des id, ceux du bord
    coface->nodes().resize(nbNoeudsI*nbNoeudsJ);

    // allocation du tableau pour les points
    Utils::Math::Point* l_points = new Utils::Math::Point[nbNoeudsI*nbNoeudsJ];
    coface->points() = l_points;

    // initialisation sens et ipas ..., boucle sur les 4 arêtes
    for (uint cote=0; cote<4; cote++){
        // indice pour boucle sur les points de la face (l_points), restreinte au cote en cours
        switch (cote) {
        case 0: // i_min
            idep[cote] = 0; ipas[cote] = nbNoeudsI;
            break;
        case 1: // j_min
            idep[cote] = 0; ipas[cote] = 1;
            break;
        case 2: // i_max
            idep[cote] = nbBrasI; ipas[cote] = nbNoeudsI;
            break;
        case 3: // j_max
            idep[cote] = nbNoeudsI*nbBrasJ; ipas[cote] = 1;
            break;
        }

        // cas pour les arêtes non dégénérées
        if (cote<coface->getNbEdges()){
            Topo::Edge* arete = coface->getEdge(cote);

            // demander les points de l'arête avec respect de l'ordre des sommets internes de la face
            std::vector<gmds::Node> nodes;
            if (coface->getNbVertices() == 4)
                arete->getNodes(coface->getVertex(Topo::TopoHelper::tabIndVtxByEdgeOnFace[cote][0]),
                		coface->getVertex(Topo::TopoHelper::tabIndVtxByEdgeOnFace[cote][1]),
                        nodes);
            else {
            	assert(cote<5);
                arete->getNodes(coface->getVertex(Topo::TopoHelper::tabIndVtxByEdgeOnDegFace[cote][0]),
                		coface->getVertex(Topo::TopoHelper::tabIndVtxByEdgeOnDegFace[cote][1]),
                        nodes);
            }

            uint jface = 0;
            uint nbPtI = arete->getNbNodes();
            // remplissage des coordonnées aux extrémités
            for(uint iarete=0, jface=idep[cote]; iarete<nbPtI; iarete++, jface+=ipas[cote]){
            	coface->nodes()[jface] = nodes[iarete].getID();
                l_points[jface] = getCoordNode(nodes[iarete]);
            }
        }
        else {
            // cas d'une arête dégénérées (la dernière)
            uint jface = 0;
            uint nbPtI = coface->getEdge(cote-2)->getNbNodes();
            gmds::Node node = getGMDSMesh().get<gmds::Node>(coface->getVertex(0)->getNode());
            for(uint iarete=0, jface=idep[cote]; iarete<nbPtI; iarete++, jface+=ipas[cote]){
            	coface->nodes()[jface] = node.getID();
                l_points[jface] = getCoordNode(node);
            }
        }

    }// end for cote<4

    if (coface->getMeshLaw() == Topo::CoFaceMeshingProperty::transfinite)
        discretiseTransfinie(nbBrasI, nbBrasJ, l_points);
    else if (coface->getMeshLaw() == Topo::CoFaceMeshingProperty::rotational){
        Utils::Math::Point axis1;
        Utils::Math::Point axis2;
        Topo::FaceMeshingPropertyRotational* prop =
                dynamic_cast<Topo::FaceMeshingPropertyRotational*>(coface->getCoFaceMeshingProperty());
        CHECK_NULL_PTR_ERROR(prop);
        prop->getAxis(axis1, axis2);
        uint dir = prop->getDir();

        discretiseRotation(nbBrasI, nbBrasJ, 0, l_points, axis1, axis2, dir);

    }
    else if (coface->getMeshLaw() == Topo::CoFaceMeshingProperty::directional
    		|| coface->getMeshLaw() == Topo::CoFaceMeshingProperty::orthogonal){

        Topo::CoEdgeMeshingProperty *empI, *empJ;
        Topo::FaceMeshingPropertyDirectional* prop =
                        dynamic_cast<Topo::FaceMeshingPropertyDirectional*>(coface->getCoFaceMeshingProperty());
        CHECK_NULL_PTR_ERROR(prop);
        uint dir = prop->getDir();
        Topo::CoEdgeMeshingProperty *empDir[2];

        // il faut retrouver si possible la discrétisation depuis une arête
        // on peut se permettre de reconstruire une loi uniforme pour les directions
        // qui ne sont pas celle de la discrétisation
        if (dir == 0){
        	bool is_inverted;
        	empDir[1] = new Topo::EdgeMeshingPropertyUniform(nbBrasJ);
        	Topo::CoEdgeMeshingProperty * empTmp = coface->getMeshingProperty(Topo::CoFace::i_dir, is_inverted);
        	// si elle est interpolée, on s'en passe
        	if (empTmp && empTmp->getMeshLaw() != Topo::CoEdgeMeshingProperty::interpolate){
        		empDir[0] = empTmp->clone();
        		if (is_inverted)
        			empDir[0]->setDirect(!empDir[0]->getDirect());
        	}
        	else
        		empDir[0] = 0;

        } else {
        	bool is_inverted;
        	empDir[0] = new Topo::EdgeMeshingPropertyUniform(nbBrasI);
        	Topo::CoEdgeMeshingProperty * empTmp = coface->getMeshingProperty(Topo::CoFace::j_dir, is_inverted);
        	// si elle est interpolée, on s'en passe
        	if (empTmp && empTmp->getMeshLaw() != Topo::CoEdgeMeshingProperty::interpolate){
        		empDir[1] = empTmp->clone();
        		if (is_inverted)
        			empDir[1]->setDirect(!empDir[1]->getDirect());
        	}
        	else
        		empDir[1] = 0;
        }

        // si la discrétisation est nulle (cas interpolée ou composée), on la remplace par une version tabulée
        if (empDir[dir] == 0){
        	std::vector<Topo::CoEdge* > coedges;
        	Topo::Vertex* vtx1;
        	Topo::Vertex* vtx2;
        	std::map<Topo::CoEdge*,uint> ratios;
        	if (dir == 0){
        		Topo::Edge* edge = coface->getEdge(1);
        		edge->getCoEdges(coedges);
        		vtx1 = coface->getVertex(1);
        		vtx2 = coface->getVertex(2);
        		for (uint i=0; i<coedges.size(); i++)
        			ratios[coedges[i]] = edge->getRatio(coedges[i]);

        	}
        	else {
        		Topo::Edge* edge = coface->getEdge(0);
        		edge->getCoEdges(coedges);
        		vtx1 = coface->getVertex(1);
        		vtx2 = coface->getVertex(0);
           		for (uint i=0; i<coedges.size(); i++)
           			ratios[coedges[i]] = edge->getRatio(coedges[i]);
        	}

        	// ordonne les coedges
        	std::vector<Topo::CoEdge* > coedges_between;
        	Topo::TopoHelper::getCoEdgesBetweenVertices(vtx1, vtx2, coedges, coedges_between);

        	std::vector<Utils::Math::Point> edge_points;
        	Topo::TopoHelper::getPoints(vtx1, vtx2, coedges_between, ratios, edge_points);

        	// calcul des longueurs des bras
        	std::vector<double> tabulation;
        	for (uint i=1; i<edge_points.size(); i++)
        		tabulation.push_back(edge_points[i].length(edge_points[i-1]));

        	// création d'une discrétisation basée sur les points des discrétisations des arêtes
        	empDir[dir] = new Topo::EdgeMeshingPropertyTabulated(tabulation);


        } // end if ((dir == 0 && empDir[0] == 0) || (dir == 1 && empDir[1] == 0))

        if (coface->getMeshLaw() == Topo::CoFaceMeshingProperty::orthogonal){
        	Topo::FaceMeshingPropertyOrthogonal* fmp = dynamic_cast<Topo::FaceMeshingPropertyOrthogonal*>(prop);
        	CHECK_NULL_PTR_ERROR(fmp);
        	uint side = fmp->getSide();
        	uint nbLayers = fmp->getNbLayers();
        	// recherche de la surface / laquelle il faut être orthogonal
        	// on ne connait pas la normale à une courbe ...
        	Geom::Surface* surface = 0;
        	Topo::Edge* edge = coface->getEdge(dir+side*2);
        	std::vector<Topo::CoEdge*> coedges;
        	edge->getCoEdges(coedges);
        	//std::cout<<"orthogonal pour "<<coface->getName()<<" dir="<<dir<<", side="<<side<<" edge="<<edge->getName()<<std::endl;
        	for (uint i=0; i<coedges.size(); i++)
        		if (coedges[i]->getGeomAssociation() != 0){
        			surface = dynamic_cast<Geom::Surface*>(coedges[i]->getGeomAssociation());
        			//std::cout<<" surface trouvée à partir de "<<coedges[i]->getName()<<std::endl;
        		}

        	discretiseOrthogonalPuisCourbe(empDir[0], empDir[1], l_points, dir, side, nbLayers, surface);
        }
        else
        	discretiseDirection(empDir[0], empDir[1], l_points, dir);

        for (uint i=0; i<2; i++)
        	delete empDir[i];

    }
    else {
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans MeshImplementation::meshStrutured pour face, type de maillage invalide", TkUtil::Charset::UTF_8));
    }


    // on ne fait pas de projection sur la surface pour les discrétisations
    // directionnelles et rotationnelles
    if (coface->getGeomAssociation() && coface->getMeshLaw() > Topo::CoFaceMeshingProperty::rotational){
#ifdef _DEBUG_MESH
        std::cout <<" Projection des noeuds de la coface "<<coface->getName()<<" sur la surface "<<coface->getGeomAssociation()->getName()<<std::endl;
#endif

        if (coface->getGeomAssociation()->getType() == Utils::Entity::GeomSurface){
            Geom::GeomEntity* ge = coface->getGeomAssociation();
            Geom::Surface* surf = dynamic_cast<Geom::Surface*>(ge);
            if (!surf)
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur pour retrouver une surface", TkUtil::Charset::UTF_8));

//            // on n'a besoin de projeter que pour le cas transfini sur surface non-plane
//            // dans les autres cas, les points sont sur la surface par construction
//            if (coface->getMeshLaw() == Topo::CoFaceMeshingProperty::transfinite
//                    && !surf->isPlanar()){

            // on projette les points à l'intérieur de la face
            for (uint i=1; i<nbBrasI; i++)
            	for (uint j=1; j<nbBrasJ; j++){
            		Utils::Math::Point &pt = l_points[i+nbNoeudsI*j];

            		try {
            			surf->project(pt);
            		}
            		catch (TkUtil::Exception& exc){
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            			message << "Erreur de projection pour la face topo "
            					<< coface->getName() << ", sur "<<surf->getName();
            			throw TkUtil::Exception (message);
            		}
            		catch (const Standard_Failure& exc){
						TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            			message << "OCC a échoué, projection impossible pour la face  "
            					<< coface->getName() << ", sur "<<surf->getName();
            			throw TkUtil::Exception (message);
            		}
            	}
            //            } // end if transfini && !isPlanar

        } else {
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "Projection impossible pour la face \""
                    << coface->getName() << "\", elle est projetée sur autre chose qu'une surface";
            throw TkUtil::Exception (message);
        }
    } // end if (fa->getGeomAssociation())

#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"création des points d'une coface "<<timer.cpuDuration()/TkUtil::Timer::cpuPerSecond()<<" secondes"<<std::endl;
    timer.reset();
    timer.start();
#endif

} // end preMeshStrutured(CoFace*)
/*----------------------------------------------------------------------------*/
void MeshImplementation::meshStrutured(Mesh::CommandCreateMesh* command, Topo::CoFace* coface)
{
#ifdef _DEBUG_TIMER
    TkUtil::Timer timer(true);
#endif

    if (!coface->isPreMeshed()){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message << "Erreur interne, les points du maillage structuré non précalculés, pour la face \""
    			<< coface->getName()<<"\"";
    	throw TkUtil::Exception (message);
    }

    uint nbBrasI, nbBrasJ;
    coface->getNbMeshingEdges(nbBrasI, nbBrasJ);

    const uint nbNoeudsI = nbBrasI + 1;
    const uint nbNoeudsJ = nbBrasJ + 1;

    for (uint i=1; i<nbBrasI; i++)
        for (uint j=1; j<nbBrasJ; j++){
            Utils::Math::Point &pt = coface->points()[i+nbNoeudsI*j];
            gmds::Node nd = getGMDSMesh().newNode(pt.getX(), pt.getY(), pt.getZ());
#ifdef _DEBUG2
            if (nd.getID()==gmds::NullID){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            	message << "Création d'un noeud GMDS en erreur pour la face "<< coface->getName();
            	throw TkUtil::Exception (message);
            }
#endif
            coface->nodes()[i+nbNoeudsI*j] = nd.getID();
            command->addCreatedNode(nd.getID());
        }

    delete [] coface->points();
    coface->points() = 0;
    coface->getMeshingData()->setPreMeshed(false);

#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"création des noeuds d'une coface "<<timer.cpuDuration()/TkUtil::Timer::cpuPerSecond()<<" secondes"<<std::endl;
    timer.reset();
    timer.start();
#endif

    // ajoute les polygones aux groupes suivant ce qui a été demandé
    _addFacesInSurfaces(command, coface);

#ifdef _DEBUG_TIMER
   timer.stop();
    std::cout<<"création des polygones et surfaces d'une coface "<<timer.cpuDuration()/TkUtil::Timer::cpuPerSecond()<<" secondes"<<std::endl;
#endif

    #ifdef _DEBUG2
        uint nbError = 0;
        for (int i=0; i<nbNoeudsI*nbNoeudsJ; i++)
            if (coface->nodes()[i] == gmds::NullID) {
            	uint ii = i%nbNoeudsI;
            	uint jj = i/nbNoeudsI;
                std::cout<<"nodes["<<i<<"]"<<"["<<ii<<","<<jj<<"] non renseigné !\n";
                nbError++;
            }
        if (nbError){
            std::cout<<"nbNoeudsI = "<<nbNoeudsI<<std::endl;
            std::cout<<"nbNoeudsJ = "<<nbNoeudsJ<<std::endl;
            std::cout<<"nbError = "<<nbError<<std::endl;
            std::cout<<*coface;
        }
#endif
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::discretiseTransfinie (uint nbBrasI, uint nbBrasJ, Utils::Math::Point* l_points)
{
    const uint nbNoeudsI = nbBrasI + 1;
    const uint nbNoeudsJ = nbBrasJ + 1;
#ifdef _DEBUG_MESH
	std::cout<<"MeshImplementation::discretiseTransfinie("<<nbBrasI<<","<<nbBrasJ<<") en cours ..."<<std::endl;
	for (uint j=0; j<nbNoeudsJ; j++)
		for (uint i=0; i<nbNoeudsI; i++)
			std::cout<<"l_points["<<i<<","<<j<<"] : "<<l_points[i+nbNoeudsI*j]<<std::endl;
#endif

    // tableau pour les coordonnées sur le carré unité
    double * ui0  = new double[nbNoeudsI];
    double * uiN  = new double[nbNoeudsI];
    double * u0j  = new double[nbNoeudsJ];
    double * uMj  = new double[nbNoeudsJ];

    _calculDistances(l_points,                   0,         1, nbBrasI, ui0);

    _calculDistances(l_points, nbNoeudsI*nbBrasJ,           1, nbBrasI, uiN);

    _calculDistances(l_points,                   0, nbNoeudsI, nbBrasJ, u0j);

    _calculDistances(l_points,             nbBrasI, nbNoeudsI, nbBrasJ, uMj);

    Utils::Math::Point &pt00 = l_points[0                        ]; // U00
    Utils::Math::Point &pt0N = l_points[0      +nbNoeudsI*nbBrasJ]; // U0N
    Utils::Math::Point &ptM0 = l_points[nbBrasI                  ]; // UM0
    Utils::Math::Point &ptMN = l_points[nbBrasI+nbNoeudsI*nbBrasJ]; // UMN

    for (uint i=1; i<nbBrasI; i++){

        Utils::Math::Point &pti0 = l_points[i                        ]; // C1(x)
        Utils::Math::Point &ptiN = l_points[i      +nbNoeudsI*nbBrasJ]; // C3(x)

        for (uint j=1; j<nbBrasJ; j++){

            Utils::Math::Point &pt0j = l_points[        nbNoeudsI*j ]; // C4(y)
            Utils::Math::Point &ptMj = l_points[nbBrasI+nbNoeudsI*j ]; // C2(y)

            double ux1 = ui0[i];
            double ux2 = uiN[i];
            double uy1 = u0j[j];
            double uy2 = uMj[j];

            double dx =  ux2 - ux1;
            double dy =  uy2 - uy1;

            // intersection entre 2 droites ((ux1,0),(ux2,0)) et ((0,uy1),(0,uy2))
            double x = (dx*uy1 + ux1) / ( 1. - dx*dy);
            double y = (dy*ux1 + uy1) / ( 1. - dx*dy);

            l_points[i+nbNoeudsI*j] =
                    pt0j*(1.-x) + ptMj*x + pti0*(1.-y) + ptiN*y
                    - (pt00*(1.-x)*(1.-y) + ptM0*x*(1.-y) + ptMN*x*y + pt0N*(1.-x)*y);

        } // end for j
    } // end for i


    delete [] ui0;
    delete [] uiN;
    delete [] u0j;
    delete [] uMj;

} // end discretiseTransfinie (2d)
/*----------------------------------------------------------------------------*/
void MeshImplementation::discretiseTransfinie (uint nbBrasI, uint nbBrasJ, uint nbBrasK, Utils::Math::Point* l_points)
{
    //std::cout<<"MeshImplementation::discretiseTransfinie("<<nbBrasI<<","<<nbBrasJ<<","<<nbBrasK<<") en cours ..."<<std::endl;

   const uint nbNoeudsI = nbBrasI + 1;
    const uint nbNoeudsJ = nbBrasJ + 1;
    const uint nbNoeudsK = nbBrasK + 1;

    // une macro pour simplifier l'écriture et la lisibilité
#define getPoint(ii,jj,kk) l_points[(ii)+nbNoeudsI*(jj)+nbNoeudsI*nbNoeudsJ*(kk)]

    // tableau pour les coordonnées sur le cube unité
    Utils::Math::Point* ui0k = new Utils::Math::Point[nbNoeudsI*nbNoeudsK];
    Utils::Math::Point* uiYk = new Utils::Math::Point[nbNoeudsI*nbNoeudsK];
    Utils::Math::Point* uij0 = new Utils::Math::Point[nbNoeudsI*nbNoeudsJ];
    Utils::Math::Point* uijZ = new Utils::Math::Point[nbNoeudsI*nbNoeudsJ];
    Utils::Math::Point* u0jk = new Utils::Math::Point[nbNoeudsJ*nbNoeudsK];
    Utils::Math::Point* uXjk = new Utils::Math::Point[nbNoeudsJ*nbNoeudsK];

    uint ii,jj,kk;

    // --------------------------------------------------------------------------------------
    // face jj=0
    // ---------
    jj = 0;
    // distance au point précédent
    for (kk=1; kk<nbNoeudsK; kk++)
        for (ii=1; ii<nbNoeudsI; ii++){
            ui0k[ii+nbNoeudsI*kk].setCoord(0,(getPoint(ii,jj,kk)-getPoint(ii-1,jj,kk)).norme());
            ui0k[ii+nbNoeudsI*kk].setCoord(2,(getPoint(ii,jj,kk)-getPoint(ii,jj,kk-1)).norme());
        }

    _calculDistancesIfixe(ui0k, 0, nbNoeudsI, nbNoeudsK);
    _calculDistancesJfixe(ui0k, 2, nbNoeudsI, nbNoeudsK);
    _setToVal(ui0k, 1, nbNoeudsI, nbNoeudsK, 0.0);

    // --------------------------------------------------------------------------------------
    // face jj=nbBrasJ
    // ---------------
    jj = nbBrasJ;
    // distance au point précédent
    for (kk=1; kk<nbNoeudsK; kk++)
        for (ii=1; ii<nbNoeudsI; ii++){
            uiYk[ii+nbNoeudsI*kk].setCoord(0,(getPoint(ii,jj,kk)-getPoint(ii-1,jj,kk)).norme());
            uiYk[ii+nbNoeudsI*kk].setCoord(2,(getPoint(ii,jj,kk)-getPoint(ii,jj,kk-1)).norme());
        }

    _calculDistancesIfixe(uiYk, 0, nbNoeudsI, nbNoeudsK);
    _calculDistancesJfixe(uiYk, 2, nbNoeudsI, nbNoeudsK);
    _setToVal(uiYk, 1, nbNoeudsI, nbNoeudsK, 1.0);

    // --------------------------------------------------------------------------------------
    // face ii=0
    // ---------
    ii=0;
    // distance au point précédent
    for (kk=1; kk<nbNoeudsK; kk++)
        for (jj=1; jj<nbNoeudsJ; jj++){
            u0jk[jj+nbNoeudsJ*kk].setCoord(1,(getPoint(ii,jj,kk)-getPoint(ii,jj-1,kk)).norme());
            u0jk[jj+nbNoeudsJ*kk].setCoord(2,(getPoint(ii,jj,kk)-getPoint(ii,jj,kk-1)).norme());
        }

    _calculDistancesIfixe(u0jk, 1, nbNoeudsJ, nbNoeudsK);
    _calculDistancesJfixe(u0jk, 2, nbNoeudsJ, nbNoeudsK);
    _setToVal(u0jk, 0, nbNoeudsJ, nbNoeudsK, 0.0);

    // --------------------------------------------------------------------------------------
    // face ii=nbBrasI
    // ---------------
    ii=nbBrasI;
    // distance au point précédent
    for (kk=1; kk<nbNoeudsK; kk++)
        for (jj=1; jj<nbNoeudsJ; jj++){
            uXjk[jj+nbNoeudsJ*kk].setCoord(1,(getPoint(ii,jj,kk)-getPoint(ii,jj-1,kk)).norme());
            uXjk[jj+nbNoeudsJ*kk].setCoord(2,(getPoint(ii,jj,kk)-getPoint(ii,jj,kk-1)).norme());
        }

    _calculDistancesIfixe(uXjk, 1, nbNoeudsJ, nbNoeudsK);
    _calculDistancesJfixe(uXjk, 2, nbNoeudsJ, nbNoeudsK);
    _setToVal(uXjk, 0, nbNoeudsJ, nbNoeudsK, 1.0);

    // --------------------------------------------------------------------------------------
    // face kk=0
    // ---------
    kk=0;
    // distance au point précédent
    for (jj=1; jj<nbNoeudsJ; jj++)
        for (ii=1; ii<nbNoeudsI; ii++){
            uij0[ii+nbNoeudsI*jj].setCoord(0,(getPoint(ii,jj,kk)-getPoint(ii-1,jj,kk)).norme());
            uij0[ii+nbNoeudsI*jj].setCoord(1,(getPoint(ii,jj,kk)-getPoint(ii,jj-1,kk)).norme());
        }

    _calculDistancesIfixe(uij0, 0, nbNoeudsI, nbNoeudsJ);
    _calculDistancesJfixe(uij0, 1, nbNoeudsI, nbNoeudsJ);
    _setToVal(uij0, 2, nbNoeudsI, nbNoeudsJ, 0.0);

    // --------------------------------------------------------------------------------------
    // face kk=nbBrasK
    // ---------------
    kk=nbBrasK;
    // distance au point précédent
    for (jj=1; jj<nbNoeudsJ; jj++)
        for (ii=1; ii<nbNoeudsI; ii++){
            uijZ[ii+nbNoeudsI*jj].setCoord(0,(getPoint(ii,jj,kk)-getPoint(ii-1,jj,kk)).norme());
            uijZ[ii+nbNoeudsI*jj].setCoord(1,(getPoint(ii,jj,kk)-getPoint(ii,jj-1,kk)).norme());
        }

    _calculDistancesIfixe(uijZ, 0, nbNoeudsI, nbNoeudsJ);
    _calculDistancesJfixe(uijZ, 1, nbNoeudsI, nbNoeudsJ);
    _setToVal(uijZ, 2, nbNoeudsI, nbNoeudsJ, 1.0);

    for (kk=1; kk<nbBrasK; kk++)
        for (jj=1; jj<nbBrasJ; jj++)
            for (ii=1; ii<nbBrasI; ii++){
                // détermination du point uijk interne au cube unité qui minimise la somme des carrés des distances aux 3 droites
                Utils::Math::Point uijk = minDist3Droites(u0jk[jj+nbNoeudsJ*kk],uXjk[jj+nbNoeudsJ*kk],
                        ui0k[ii+nbNoeudsI*kk],uiYk[ii+nbNoeudsI*kk],
                        uij0[ii+nbNoeudsI*jj],uijZ[ii+nbNoeudsI*jj]);

                const double xx = uijk.getX();
                const double yy = uijk.getY();
                const double zz = uijk.getZ();

                // l'interpolation
                getPoint(ii,jj,kk)
                = (1-xx)*getPoint(0,jj,kk) + xx*getPoint(nbBrasI,jj,kk)
                + (1-yy)*getPoint(ii,0,kk) + yy*getPoint(ii,nbBrasJ,kk)
                + (1-zz)*getPoint(ii,jj,0) + zz*getPoint(ii,jj,nbBrasK)

                - (1-xx)*((1-yy)*getPoint(0,0,kk)       + yy*getPoint(0,nbBrasJ,kk))
                - xx    *((1-yy)*getPoint(nbBrasI,0,kk) + yy*getPoint(nbBrasI,nbBrasJ,kk))

                - (1-yy)*((1-zz)*getPoint(ii,0,0)       + zz*getPoint(ii,0,nbBrasK))
                - yy    *((1-zz)*getPoint(ii,nbBrasJ,0) + zz*getPoint(ii,nbBrasJ,nbBrasK))

                - (1-zz)*((1-xx)*getPoint(0,jj,0)       + xx*getPoint(nbBrasI,jj,0))
                - zz    *((1-xx)*getPoint(0,jj,nbBrasK) + xx*getPoint(nbBrasI,jj,nbBrasK))

                + (1-xx)*( (1-yy)*((1-zz)*getPoint(0,0,0)       + zz*getPoint(0,0,nbBrasK))
                        + yy  *((1-zz)*getPoint(0,nbBrasJ,0) + zz*getPoint(0,nbBrasJ,nbBrasK)))

                        + xx*( (1-yy)*((1-zz)*getPoint(nbBrasI,0,0)       + zz*getPoint(nbBrasI,0,nbBrasK))
                                + yy  *((1-zz)*getPoint(nbBrasI,nbBrasJ,0) + zz*getPoint(nbBrasI,nbBrasJ,nbBrasK)))
                                ;

            }

    delete[] ui0k;
    delete[] uiYk;
    delete[] uij0;
    delete[] uijZ;
    delete[] u0jk;
    delete[] uXjk;

#ifdef _DEBUG_MESH
    std::cout<<"MeshImplementation::discretiseTransfinie ("<<nbBrasI<<", "<<nbBrasJ<<", "<<nbBrasK<<")\n";
    for (kk=0; kk<nbNoeudsK; kk++)
        for (jj=0; jj<nbNoeudsJ; jj++)
            for (ii=0; ii<nbNoeudsI; ii++){
                std::cout<<"getPoint("<<ii<<","<<jj<<","<<kk<<") = "<<getPoint(ii,jj,kk)<<std::endl;
            }
#endif

#undef getPoint
} // end discretiseTransfinie (3d)
/*----------------------------------------------------------------------------*/
void MeshImplementation::discretiseDirection (
        Topo::CoEdgeMeshingProperty* dni,
        Topo::CoEdgeMeshingProperty* dnj,
        Topo::CoEdgeMeshingProperty* dnk,
        Utils::Math::Point* l_points,
        uint dir)
{
#ifdef _DEBUG_MESH
    std::cout<<"MeshImplementation::discretiseDirection("
            <<dni->getNbEdges()<<","<<dnj->getNbEdges()<<","<<dnk->getNbEdges()<<","
            <<dir<<")"<<std::endl;
#endif
    uint indMax1;
    uint indMax2;
    uint indMax3;
    uint incr1 = 1;
    uint incr2 = 1;
    uint incr3 = 1;
    Topo::CoEdgeMeshingProperty* disc;

    switch (dir) {
    case 0:
        indMax1 = dnj->getNbEdges();
        indMax2 = dnk->getNbEdges();
        indMax3 = dni->getNbEdges();
        incr1 = indMax3+1;
        incr2 = (indMax3+1)*(indMax1+1);
        incr3 = 1;
        disc = dni;
        break;
    case 1:
        indMax1 = dni->getNbEdges();
        indMax2 = dnk->getNbEdges();
        indMax3 = dnj->getNbEdges();
        incr1 = 1;
        incr2 = (indMax1+1)*(indMax3+1);
        incr3 = indMax1+1;
        disc = dnj;
        break;
    case 2:
        indMax1 = dni->getNbEdges();
        indMax2 = dnj->getNbEdges();
        indMax3 = dnk->getNbEdges();
        incr1 = 1;
        incr2 = indMax1+1;
        incr3 = (indMax1+1)*(indMax2+1);
        disc = dnk;
        break;
    default:
		TkUtil::UTF8String	messageErr (TkUtil::Charset::UTF_8);
        messageErr << "direction  "<<(short)dir<<" de discrétisation non prévue";
        throw TkUtil::Exception(messageErr);
    }

    for (uint ind1=1; ind1<indMax1; ind1++){
        for (uint ind2=1; ind2<indMax2; ind2++){
            Utils::Math::Point &pt0 = l_points[incr1*ind1+incr2*ind2];
            Utils::Math::Point &ptN = l_points[incr1*ind1+incr2*ind2+incr3*indMax3];
#ifdef _DEBUG2
            std::cout<<"entre pt0["<<ind1<<", "<<ind2<<", "<<0<<"] ["<<incr1*ind1+incr2*ind2<<"] = "<<pt0
                    <<" et ptN["<<ind1<<", "<<ind2<<", "<<indMax3<<"] ["<<incr1*ind1+incr2*ind2+incr3*indMax3<<"] = "<<ptN
                    <<std::endl;
#endif

            Utils::Math::Point vect = (ptN - pt0);
            disc->initCoeff(vect.norme());

            for (uint ind3=1; ind3<indMax3; ind3++)
                l_points[incr1*ind1+incr2*ind2+incr3*ind3] = (vect * disc->nextCoeff()) + pt0;
        }
    }

} // end discretiseDirection (3d)
/*----------------------------------------------------------------------------*/
void MeshImplementation::discretiseOrthogonalPuisCourbe (
		Topo::CoEdgeMeshingProperty* dni,
		Topo::CoEdgeMeshingProperty* dnj,
		Topo::CoEdgeMeshingProperty* dnk,
		Utils::Math::Point* l_points,
		uint dir,
		uint side,
		uint nbLayers,
		Geom::Surface* surface)
{
#ifdef _DEBUG_MESH
    std::cout<<"MeshImplementation::discretiseOrthogonalPuisCourbe("
            <<dni->getNbEdges()<<","<<dnj->getNbEdges()<<","<<dnk->getNbEdges()<<","
            <<dir<<", "<<side<<", "<<nbLayers<<", "<<(surface?surface->getName():"0")<<")"<<std::endl;
#endif
    uint indMax1;
    uint indMax2;
    uint indMax3;
    uint incr1 = 1;
    uint incr2 = 1;
    uint incr3 = 1;
    Topo::CoEdgeMeshingProperty* disc;

    switch (dir) {
    case 0:
        indMax1 = dnj->getNbEdges();
        indMax2 = dnk->getNbEdges();
        indMax3 = dni->getNbEdges();
        incr1 = indMax3+1;
        incr2 = (indMax3+1)*(indMax1+1);
        incr3 = 1;
        disc = dni;
        break;
    case 1:
        indMax1 = dni->getNbEdges();
        indMax2 = dnk->getNbEdges();
        indMax3 = dnj->getNbEdges();
        incr1 = 1;
        incr2 = (indMax1+1)*(indMax3+1);
        incr3 = indMax1+1;
        disc = dnj;
        break;
    case 2:
        indMax1 = dni->getNbEdges();
        indMax2 = dnj->getNbEdges();
        indMax3 = dnk->getNbEdges();
        incr1 = 1;
        incr2 = indMax1+1;
        incr3 = (indMax1+1)*(indMax2+1);
        disc = dnk;
        break;
    default:
		TkUtil::UTF8String	messageErr (TkUtil::Charset::UTF_8);
        messageErr << "direction  "<<(short)dir<<" de discrétisation non prévue";
        throw TkUtil::Exception(messageErr);
    }
    // inversion du sens
    if (side == 1)
    	disc->setDirect(!disc->getDirect());

    for (uint ind1=1; ind1<indMax1; ind1++){
        for (uint ind2=1; ind2<indMax2; ind2++){
            Utils::Math::Point &pt0 = l_points[incr1*ind1+incr2*ind2];
            Utils::Math::Point &ptN = l_points[incr1*ind1+incr2*ind2+incr3*indMax3];
#ifdef _DEBUG_MESH
            std::cout<<"entre pt0["<<ind1<<", "<<ind2<<", "<<0<<"] ["<<incr1*ind1+incr2*ind2<<"] = "<<pt0
                    <<" et ptN["<<ind1<<", "<<ind2<<", "<<indMax3<<"] ["<<incr1*ind1+incr2*ind2+incr3*indMax3<<"] = "<<ptN
                    <<std::endl;
#endif

            Utils::Math::Point vect = (ptN - pt0);
            double dist = vect.norme();

            // vecteur intermédiaire pour les points
            std::vector<Utils::Math::Point> ptInternes;

            // recherche de la normale à la surface pour le point pt0 si side == 0, pt1 sinon
            if (side == 0){
            	Utils::Math::Vector normale;
            	if (surface){
            		surface->normal(pt0, normale);
            	}
            	else {
            		Utils::Math::Point v1 = l_points[incr1*(ind1+1)+incr2*(ind2)]
													 -l_points[incr1*(ind1-1)+incr2*(ind2)];
            		Utils::Math::Point v2 = l_points[incr1*(ind1)+incr2*(ind2+1)]
													 -l_points[incr1*(ind1)+incr2*(ind2-1)];
            		normale = v1*v2;
            	}

            	normale/=normale.norme();
            	normale*=dist;

            	// orientation de la normale dans la même direction que vect
            	if (Utils::Math::scaMul(vect, normale)<0.0)
            		normale *= -1;

                disc->initCoeff(dist);

                // tous les points alignés et orthogonaux
                for (uint ind3=1; ind3<indMax3; ind3++)
                	ptInternes.push_back((normale * disc->nextCoeff()) + pt0);
                // le point image de ptN
                ptInternes.push_back((normale * disc->nextCoeff()) + pt0);

                // déplace les point pour se raccorder à ptN
                courbeDiscretisation(ptN, ptInternes, nbLayers);

                // récupération des points
                for (uint ind3=1; ind3<indMax3; ind3++)
                	l_points[incr1*ind1+incr2*ind2+incr3*ind3] = ptInternes[ind3-1];

            } else {
            	Utils::Math::Vector normale;
            	if (surface){
            		surface->normal(ptN, normale);
            	}
            	else {

            		Utils::Math::Point v1 = l_points[incr1*(ind1+1)+incr2*(ind2)+incr3*indMax3]
													 -l_points[incr1*(ind1-1)+incr2*(ind2)+incr3*indMax3];
            		Utils::Math::Point v2 = l_points[incr1*(ind1)+incr2*(ind2+1)+incr3*indMax3]
													 -l_points[incr1*(ind1)+incr2*(ind2-1)+incr3*indMax3];

            		normale = v1*v2;
            	}

            	normale/=normale.norme();
            	normale*=dist;

            	// orientation de la normale dans la même direction que vect
            	if (Utils::Math::scaMul(vect, normale)<0.0)
            		normale *= -1;

                disc->initCoeff(dist);

                // tous les points alignés et orthogonaux
                for (uint ind3=indMax3-1; ind3>=1; ind3--)
                	ptInternes.push_back(ptN - (normale * disc->nextCoeff()));
                // le point image de pt0
                ptInternes.push_back(ptN - (normale * disc->nextCoeff()));

                // déplace les point pour se raccorder à pt0
                courbeDiscretisation(pt0, ptInternes, nbLayers);

                for (uint ind3=1; ind3<indMax3; ind3++)
                	l_points[incr1*ind1+incr2*ind2+incr3*ind3] = ptInternes[indMax3-ind3-1];

            }

        } // end for ind2
    }
    // inversion du sens (retour au sens initial)
    if (side == 1)
    	disc->setDirect(!disc->getDirect());


} // end discretiseOrthogonalPuisCourbe (3d)
/*----------------------------------------------------------------------------*/
void MeshImplementation::courbeDiscretisation(Utils::Math::Point& ptN,
			std::vector<Utils::Math::Point>& ptInternes,
			uint nbLayers)
{
#ifdef _DEBUG_MESH
	std::cout<<"courbeDiscretisation avec ptInternes.size() = "<<ptInternes.size()<<", nbLayers = "<<nbLayers<<std::endl;
	for (uint i=0; i<ptInternes.size();i++)
		std::cout<<"ptInternes["<<i<<"] "<<ptInternes[i]<<std::endl;
#endif

	// utilisation d'une courbe de Bézier pour positionner les points entre
	// le dernier point imposé par la condition d'orthogonalité et le point d'arrivé

	// premier test avec un point intermédiaire au 1/3 du segment dans la continuité de l'orthogonalité
	// les 3 points de contrôle pour la courbe de Bézier:
	Utils::Math::Point p1 = ptInternes[nbLayers-1];
	Utils::Math::Point p2 = p1 + (ptInternes[ptInternes.size()-1] - p1) / 3.0;
	Utils::Math::Point p3 = ptN;

	// longueur du segment
	double lg = (ptInternes[ptInternes.size()-1] - p1).norme();

#ifdef _DEBUG_MESH
	std::cout<<"p1 "<<p1<<std::endl;
	std::cout<<"p2 "<<p2<<std::endl;
	std::cout<<"p3 "<<p3<<std::endl;
	std::cout<<"lg "<<lg<<std::endl;
#endif
	for (uint i=nbLayers-1; i<ptInternes.size();i++){
		double param = (ptInternes[i]-p1).norme() / lg;
		ptInternes[i] = bezier3(p1, p2, p3, param);
#ifdef _DEBUG_MESH
		std::cout<<"param "<<param<<" ptInternes["<<i<<"] "<<ptInternes[i]<<std::endl;
#endif
	}

} // end courbeDiscretisation
/*----------------------------------------------------------------------------*/
void MeshImplementation::discretiseDirection (
        Topo::CoEdgeMeshingProperty* dni,
        Topo::CoEdgeMeshingProperty* dnj,
        Utils::Math::Point* l_points,
        uint dir)
{
#ifdef _DEBUG_MESH
	std::cout<<"MeshImplementation::discretiseDirection("
			<<dni->getNbEdges()<<","<<dnj->getNbEdges()<<","
			<<dir<<")"<<std::endl;
	std::cout<<"dni.getMeshLawName() = "<<dni->getMeshLawName()<<std::endl;
	std::cout<<"dnj.getMeshLawName() = "<<dnj->getMeshLawName()<<std::endl;
#endif

    uint indMax1;
    uint indMax2;
    uint incr1 = 1;
    uint incr2 = 1;
    Topo::CoEdgeMeshingProperty* disc;

    switch (dir) {
    case 0:
      indMax1 = dnj->getNbEdges();
      indMax2 = dni->getNbEdges();
      incr1 = indMax2+1;
      disc = dni;
      break;
    case 1:
      indMax1 = dni->getNbEdges();
      indMax2 = dnj->getNbEdges();
      incr2 = indMax1+1;
      disc = dnj;
      break;
    default:
		TkUtil::UTF8String	messageErr (TkUtil::Charset::UTF_8);
        messageErr << "direction  "<<(short)dir<<" de discrétisation non prévue";
        throw TkUtil::Exception(messageErr);
    }

    for (uint ind1=1; ind1<indMax1; ind1++){
      Utils::Math::Point &pt0 = l_points[incr1*ind1];
      Utils::Math::Point &ptN = l_points[incr1*ind1+incr2*indMax2];

      Utils::Math::Point vect = (ptN - pt0);
      disc->initCoeff(vect.norme());

      for (uint ind2=1; ind2<indMax2; ind2++)
        l_points[incr1*ind1+incr2*ind2] = (vect * disc->nextCoeff()) + pt0;
    }

} // end discretiseDirection (2d)
/*----------------------------------------------------------------------------*/
void MeshImplementation::discretiseOrthogonalPuisCourbe (
        Topo::CoEdgeMeshingProperty* dni,
        Topo::CoEdgeMeshingProperty* dnj,
            Utils::Math::Point* l_points,
            uint dir,
			uint side,
			uint nbLayers,
			Geom::Surface* surface)
{
#ifdef _DEBUG_MESH
    std::cout<<"MeshImplementation::discretiseOrthogonalPuisCourbe("
            <<dni->getNbEdges()<<","<<dnj->getNbEdges()<<","
            <<dir<<", "<<side<<", "<<nbLayers<<", "<<(surface?surface->getName():"0")<<")"<<std::endl;
#endif
    uint indMax1;
    uint indMax2;
    uint incr1 = 1;
    uint incr2 = 1;
    Topo::CoEdgeMeshingProperty* disc;

    switch (dir) {
    case 0:
      indMax1 = dnj->getNbEdges();
      indMax2 = dni->getNbEdges();
      incr1 = indMax2+1;
      disc = dni;
      break;
    case 1:
      indMax1 = dni->getNbEdges();
      indMax2 = dnj->getNbEdges();
      incr2 = indMax1+1;
      disc = dnj;
      break;
    default:
		TkUtil::UTF8String	messageErr (TkUtil::Charset::UTF_8);
        messageErr << "direction  "<<(short)dir<<" de discrétisation non prévue";
        throw TkUtil::Exception(messageErr);
    }

    // inversion du sens
    if (side == 1)
    	disc->setDirect(!disc->getDirect());

    for (uint ind1=1; ind1<indMax1; ind1++){
      Utils::Math::Point &pt0 = l_points[incr1*ind1];
      Utils::Math::Point &ptN = l_points[incr1*ind1+incr2*indMax2];

      Utils::Math::Point vect = (ptN - pt0);
      double dist = vect.norme();

      // vecteur intermédiaire pour les points
      std::vector<Utils::Math::Point> ptInternes;

      // recherche de la normale pour le point pt0 si side == 0, pt1 sinon
      if (side == 0){
    	  Utils::Math::Vector normale;
    	  if (surface){
    		  surface->normal(pt0, normale);
    	  }
    	  else {
    		  Utils::Math::Point v1 = l_points[incr1*(ind1+1)]
											   - l_points[incr1*(ind1-1)];
    		  Utils::Math::Point v2 = v1*vect;
    		  normale = v1*v2;
    	  }
    	  normale/=normale.norme();
    	  normale*=dist;

    	  // orientation de la normale dans la même direction que vect
    	  if (Utils::Math::scaMul(vect, normale)<0.0)
    		  normale *= -1;

    	  disc->initCoeff(dist);

          for (uint ind2=1; ind2<indMax2; ind2++)
        	  ptInternes.push_back((normale * disc->nextCoeff()) + pt0);
          // le point image de ptN
          ptInternes.push_back((normale * disc->nextCoeff()) + pt0);

          // déplace les point pour se raccorder à ptN
          courbeDiscretisation(ptN, ptInternes, nbLayers);

          // récupération des points
          for (uint ind2=1; ind2<indMax2; ind2++)
        	  l_points[incr1*ind1+incr2*ind2] = ptInternes[ind2-1];
      }
      else {
    	  Utils::Math::Vector normale;
    	  if (surface){
    		  surface->normal(ptN, normale);
    	  }
    	  else {
    		  Utils::Math::Point v1 = l_points[incr1*(ind1+1)+incr2*indMax2]
											   - l_points[incr1*(ind1-1)+incr2*indMax2];
    		  Utils::Math::Point v2 = v1*vect;
    		  normale = v1*v2;
    	  }
    	  normale/=normale.norme();
    	  normale*=dist;

    	  // orientation de la normale dans la même direction que vect
    	  if (Utils::Math::scaMul(vect, normale)<0.0)
    		  normale *= -1;

    	  disc->initCoeff(dist);

          for (uint ind2=indMax2-1; ind2>=1; ind2--)
        	  ptInternes.push_back(ptN - (normale * disc->nextCoeff()));
          // le point image de pt0
          ptInternes.push_back(ptN - (normale * disc->nextCoeff()));

          // déplace les point pour se raccorder à ptN
          courbeDiscretisation(pt0, ptInternes, nbLayers);

          // récupération des points
          for (uint ind2=1; ind2<indMax2; ind2++)
        	  l_points[incr1*ind1+incr2*ind2] = ptInternes[indMax2-ind2-1];
      }
    }

    // inversion du sens (retour au sens initial)
    if (side == 1)
    	disc->setDirect(!disc->getDirect());

} // end discretiseOrthogonalPuisCourbe (2d)
/*----------------------------------------------------------------------------*/
void MeshImplementation::discretiseRotation (
        uint nbBrasI, uint nbBrasJ, uint nbBrasK,
        Utils::Math::Point* l_points,
        Utils::Math::Point axis1,
        Utils::Math::Point axis2,
        uint direction)
{
#ifdef _DEBUG_MESH
    std::cout<<"MeshImplementation::discretiseRotation("
            <<nbBrasI<<","<<nbBrasJ<<","<<nbBrasK<<","
            <<axis1<<","<<axis2<<","<<direction<<")"<<std::endl;
#endif

    if (axis1 == axis2)
        throw TkUtil::Exception (TkUtil::UTF8String ("Discrétisation avec méthode par rotation impossible, "
                                 "les 2 points qui doivent définir un axe sont confondus\n"
                                 "Certainement un oubli de définition de l'axe ...", TkUtil::Charset::UTF_8));

    uint indMax1;
    uint indMax2;
    uint indMax3;
    uint incr1 = 1;
    uint incr2 = 1;
    uint incr3 = 1;
    uint disc;

    switch (direction) {
    case 0:
        indMax1 = nbBrasJ;
        indMax2 = nbBrasK;
        indMax3 = nbBrasI;
        incr1 = indMax3+1;
        incr2 = (nbBrasK?(indMax3+1)*(indMax1+1):0);
        incr3 = 1;
        disc = nbBrasI;
        break;
    case 1:
        indMax1 = nbBrasI;
        indMax2 = nbBrasK;
        indMax3 = nbBrasJ;
        incr1 = 1;
        incr2 = (nbBrasK?(indMax1+1)*(indMax3+1):0);
        incr3 = indMax1+1;
        disc = nbBrasJ;
        break;
    case 2:
        indMax1 = nbBrasI;
        indMax2 = nbBrasJ;
        indMax3 = nbBrasK;
        incr1 = 1;
        incr2 = indMax1+1;
        incr3 = (indMax1+1)*(indMax2+1);
        disc = nbBrasK;
        break;
    default:
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, direction non prévue", TkUtil::Charset::UTF_8));
    }

    // recherche d'une paire de points en dehors de l'axe
    // (2 points distincts entre les 2 faces qui se font face)
    bool trouve = false;

    Utils::Math::Point pt_dep;
    Utils::Math::Point pt_arr;

    for (uint ind1=0; ind1<=indMax1 && !trouve; ind1++){
        for (uint ind2=0; ind2<=indMax2 && !trouve; ind2++){
            Utils::Math::Point &pt0 = l_points[incr1*ind1+incr2*ind2];
            Utils::Math::Point &pt1 = l_points[incr1*ind1+incr2*ind2+incr3*indMax3];

            if (!Utils::Math::MgxNumeric::isNearlyZero((pt1-pt0).norme2())){
                trouve = true;
                pt_dep = pt0;
                pt_arr = pt1;
#ifdef _DEBUG_MESH
                std::cout<<"pt_dep["<<ind1<<", "<<ind2<<", "<<0<<"] = "<<pt0<<std::endl;
                std::cout<<"pt_arr["<<ind1<<", "<<ind2<<", "<<indMax3<<"] = "<<pt1<<std::endl;
#endif

            }
        }
    }

    if (!trouve)
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur Interne, pas de sommet hors de l'axe de trouvé", TkUtil::Charset::UTF_8));

    // axe de la rotation
    gp_Pnt p1(axis1.getX(),axis1.getY(),axis1.getZ());

    gp_Dir dir( axis2.getX()-axis1.getX(),
                axis2.getY()-axis1.getY(),
                axis2.getZ()-axis1.getZ());

    gp_Ax1 axis(p1,dir);

    // projection d'un point sur l'axe
    gp_Pnt pt2proj(pt_dep.getX(), pt_dep.getY(), pt_dep.getZ());

    gp_Vec aVDirection(axis.Direction());
    gp_Pnt anOrigin=axis.Location();
    gp_Vec aV(anOrigin, pt2proj);
    Standard_Real aDist = aVDirection.Dot(aV);
    //
    gp_Pnt proj= anOrigin.Translated(aDist*aVDirection);

    Utils::Math::Point pt_proj(proj.X(), proj.Y(), proj.Z());

    // calcul de l'angle entre les 2 extrémités
    gp_Vec vecFrom(pt_dep.getX()-pt_proj.getX(),pt_dep.getY()-pt_proj.getY(),pt_dep.getZ()-pt_proj.getZ());
    gp_Vec vecTo(pt_arr.getX()-pt_proj.getX(),pt_arr.getY()-pt_proj.getY(),pt_arr.getZ()-pt_proj.getZ());

    gp_Quaternion quarternion(vecFrom, vecTo);

    Standard_Real angle = quarternion.GetRotationAngle();

#ifdef _DEBUG_MESH
    std::cout<<" angle "<<angle<<" rad,  "<<angle*180/M_PI<<" deg"<<std::endl;
#endif


    // mise en place de la transformation
    gp_Trsf T;

    // que l'on fasse quelque chose pour les CoFaces
    if (nbBrasK == 0)
        indMax2  = 2;

    // test le sens de rotation
    {
    	double x,y,z;
    	x = pt_dep.getX();
    	y = pt_dep.getY();
    	z = pt_dep.getZ();
    	T.SetRotation(axis,angle);
    	T.Transforms(x, y, z);
    	Utils::Math::Point pt_sens1(x,y,z);

    	x = pt_dep.getX();
    	y = pt_dep.getY();
    	z = pt_dep.getZ();
    	T.SetRotation(axis,-angle);
    	T.Transforms(x, y, z);
    	Utils::Math::Point pt_sens2(x,y,z);

    	// cas où on inverse le sens
    	if (pt_arr.length2(pt_sens2)<pt_arr.length2(pt_sens1)){
    		angle = -angle;
#ifdef _DEBUG_MESH
    std::cout<<" angle inversé => "<<angle*180/M_PI<<" deg"<<std::endl;
#endif
    	}
    }

    for (uint ind3=1; ind3<indMax3; ind3++){
        T.SetRotation(axis,angle/disc*ind3);

        for (uint ind1=1; ind1<indMax1; ind1++){
            for (uint ind2=1; ind2<indMax2; ind2++){
                Utils::Math::Point &pt0 = l_points[incr1*ind1+incr2*ind2];
                Utils::Math::Point &ptI = l_points[incr1*ind1+incr2*ind2+incr3*ind3];

                double x = pt0.getX();
                double y = pt0.getY();
                double z = pt0.getZ();

                T.Transforms(x, y, z);

                ptI.setX(x);
                ptI.setY(y);
                ptI.setZ(z);

#ifdef _DEBUG_MESH2
                std::cout<<"pt0["<<ind1<<", "<<ind2<<", "<<0<<"] ["<<incr1*ind1+incr2*ind2<<"] = "<<pt0
                        <<" => ptI["<<ind1<<", "<<ind2<<", "<<ind3<<"] ["<<incr1*ind1+incr2*ind2+incr3*ind3<<"] = "
                        << ptI<<std::endl;;
#endif
            }
        }
    } // end for ind3

#ifdef _DEBUG3
    if (nbBrasK == 0)
        indMax2  = 0;
    for (uint ind3=0; ind3<=indMax3; ind3++){
        for (uint ind2=0; ind2<=indMax2; ind2++){
            for (uint ind1=0; ind1<=indMax1; ind1++){
                Utils::Math::Point &pt = l_points[incr1*ind1+incr2*ind2+incr3*ind3];
                std::cout<<"pt["<<ind1<<", "<<ind2<<", "<<ind3<<"] = "<<pt<<std::endl;
            }
        }
    }
#endif

#ifdef _DEBUG_MESH
    std::cout<<"MeshImplementation::discretiseRotation terminé"<<std::endl;
#endif
} // end discretiseRotation (3d)
/*----------------------------------------------------------------------------*/
void MeshImplementation::_calculDistances(Utils::Math::Point* l_points, uint indDep, uint increment, uint nbBras, double* ui)
{
    // on commence par y mettre les distances au point précédent
    ui[0] = 0.0;
    for (uint i=1; i<=nbBras; i++){
      Utils::Math::Point &ptip = l_points[indDep+(i-1)*increment];
      Utils::Math::Point &pti = l_points[indDep+i*increment];
      ui[i] = (pti-ptip).norme();
    }
    // calcul de la distance totale
    for (uint i=1; i<=nbBras; i++)
      ui[i]+=ui[i-1];

    // on normalise
    if(Utils::Math::MgxNumeric::isNearlyZero(ui[nbBras]))
      for (uint i=1; i<=nbBras; i++)
        ui[i]=((double)i)/(double)nbBras;
    else
      for (uint i=1; i<=nbBras; i++)
        ui[i]/=ui[nbBras];
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::_calculDistancesIfixe(Utils::Math::Point* uij, uint indCoord, uint nbNoeudsI, uint nbNoeudsJ)
{
    // distance totale
    for (uint jj=1; jj<nbNoeudsJ; jj++)
        for (uint ii=1; ii<nbNoeudsI; ii++)
            uij[ii+nbNoeudsI*jj].setCoord(indCoord,uij[ii+nbNoeudsI*jj].getCoord(indCoord)+uij[ii-1+nbNoeudsI*jj].getCoord(indCoord));

    uint nbBrasI = nbNoeudsI-1;
    uint nbBrasJ = nbNoeudsJ-1;

    // normalisation
    for (uint jj=1; jj<nbNoeudsJ; jj++)
        if (Utils::Math::MgxNumeric::isNearlyZero(uij[nbBrasI+nbNoeudsI*jj].getCoord(indCoord)))
            for (uint ii=1; ii<nbNoeudsI; ii++)
                uij[ii+nbNoeudsI*jj].setCoord(indCoord,((double)ii)/(double)nbBrasI);
        else
            for (uint ii=1; ii<nbNoeudsI; ii++)
                uij[ii+nbNoeudsI*jj].setCoord(indCoord,uij[ii+nbNoeudsI*jj].getCoord(indCoord)/uij[nbBrasI+nbNoeudsI*jj].getCoord(indCoord));
}
//---------------------------------------------------------------------------------------
void MeshImplementation::_calculDistancesJfixe(Utils::Math::Point* uij, uint indCoord, uint nbNoeudsI, uint nbNoeudsJ)
{
    // distance totale
    for (uint jj=1; jj<nbNoeudsJ; jj++)
        for (uint ii=1; ii<nbNoeudsI; ii++)
            uij[ii+nbNoeudsI*jj].setCoord(indCoord,uij[ii+nbNoeudsI*jj].getCoord(indCoord)+uij[ii+nbNoeudsI*(jj-1)].getCoord(indCoord));

    uint nbBrasI = nbNoeudsI-1;
    uint nbBrasJ = nbNoeudsJ-1;

    // normalisation
    for (uint ii=1; ii<nbNoeudsI; ii++)
        if (Utils::Math::MgxNumeric::isNearlyZero(uij[ii+nbNoeudsI*nbBrasJ].getCoord(indCoord)))
            for (uint jj=1; jj<nbNoeudsJ; jj++)
                uij[ii+nbNoeudsI*jj].setCoord(indCoord,((double)jj)/(double)nbBrasJ);
        else
            for (uint jj=1; jj<nbNoeudsJ; jj++)
                uij[ii+nbNoeudsI*jj].setCoord(indCoord,uij[ii+nbNoeudsI*jj].getCoord(indCoord)/uij[ii+nbNoeudsI*nbBrasJ].getCoord(indCoord));
}
//---------------------------------------------------------------------------------------
void MeshImplementation::_setToVal(Utils::Math::Point* uij, uint indCoord, uint nbNoeudsI, uint nbNoeudsJ, double val)
{
    for (uint jj=1; jj<nbNoeudsJ; jj++)
        for (uint ii=1; ii<nbNoeudsI; ii++)
            uij[ii+nbNoeudsI*jj].setCoord(indCoord,val);
}
/*----------------------------------------------------------------------------*/
void MeshImplementation::_addRegionsInVolumes(Mesh::CommandCreateMesh* command, Topo::Block* bl,
        uint nbBrasI, uint nbBrasJ, uint nbBrasK)
//#define _DEBUG2
{
    std::vector<std::string> groupsName;
    bl->getGroupsName(groupsName);

#ifdef _DEBUG_GROUP_BY_TOPO_ENTITY
    // on ajoute un groupe pour distinguer les blocs en mode debug
    groupsName.push_back(bl->getName());
#endif

    if (groupsName.empty()){
		TkUtil::UTF8String	message1 (TkUtil::Charset::UTF_8);
        message1 <<"Le bloc "<<bl->getName()<<" n'a pas de groupe pour les mailles";
        getContext().getLogStream()->log (TkUtil::TraceLog (message1, TkUtil::Log::ERROR));
    }

    // création des polyèdres
    const uint nbNoeudsI = nbBrasI + 1;
    const uint nbNoeudsJ = nbBrasJ + 1;
    const uint nbNoeudsK = nbBrasK + 1;

    std::vector<gmds::TCellID>& nodes = bl->nodes();
    std::vector<gmds::TCellID>& elem = bl->regions();

#define nodeIJ(ii,jj,kk) nodes[ii+(jj)*nbNoeudsI+(kk)*nbNoeudsI*nbNoeudsJ]

    uint iBegin = 0, iEnd = nbBrasI;
    uint jBegin = 0, jEnd = nbBrasJ;
    uint kBegin = 0, kEnd = nbBrasK;

    if (bl->getNbVertices() != 8) // K MAX
        kEnd-=1;

    gmds::IGMesh& gmds_mesh = getGMDSMesh();
    bool areRegionsTested = false;
    bool areRegionsInverted = false;
    unsigned int testOnDir = 9;
    unsigned int nbTests = 0;
    unsigned int nbPos = 0;
    unsigned int nbTestsInv = 0;
    unsigned int nbPosInv = 0;
    // first loop in order to check if the cells are inverted
    {
    	Qualif::Hexaedre* maille_tmp = new Qualif::Hexaedre();
#ifdef _DEBUG2
    	std::cout<<" iEnd-iBegin = "<<iEnd-iBegin<<std::endl;
    	std::cout<<" jEnd-jBegin = "<<jEnd-jBegin<<std::endl;
    	std::cout<<" kEnd-kBegin = "<<kEnd-kBegin<<std::endl;
    	uint id = 0;
#endif
        for (uint k=kBegin; k<kEnd; k++) {
            for (uint j=jBegin; j<jEnd; j++) {
                for (uint i=iBegin; i<iEnd; i++) {
#ifdef _DEBUG2
                	id++;
#endif

                	if(
                		(k==kBegin || k==kEnd-1 || (k-kBegin)%testOnDir==(kEnd-kBegin)%testOnDir) &&
                		(j==jBegin || j==jEnd-1 || (j-jBegin)%testOnDir==(jEnd-jBegin)%testOnDir) &&
                		(i==iBegin || i==iEnd-1 || (i-iBegin)%testOnDir==(iEnd-iBegin)%testOnDir)
                	) {
                		areRegionsTested = true;

                		nbTests++;
                        gmds::Node nd1 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j,k));
                        gmds::Node nd2 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j,k));
                        gmds::Node nd3 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j+1,k));
                        gmds::Node nd4 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j+1,k));
                        gmds::Node nd5 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j,k+1));
                        gmds::Node nd6 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j,k+1));
                        gmds::Node nd7 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j+1,k+1));
                        gmds::Node nd8 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j+1,k+1));

                    	Qualif::Vecteur *sommets = new Qualif::Vecteur[8];
                    	sommets[0] = Qualif::Vecteur(nd1.X(), nd1.Y(), nd1.Z());
                    	sommets[1] = Qualif::Vecteur(nd2.X(), nd2.Y(), nd2.Z());
                    	sommets[2] = Qualif::Vecteur(nd3.X(), nd3.Y(), nd3.Z());
                    	sommets[3] = Qualif::Vecteur(nd4.X(), nd4.Y(), nd4.Z());
                    	sommets[4] = Qualif::Vecteur(nd5.X(), nd5.Y(), nd5.Z());
                    	sommets[5] = Qualif::Vecteur(nd6.X(), nd6.Y(), nd6.Z());
                    	sommets[6] = Qualif::Vecteur(nd7.X(), nd7.Y(), nd7.Z());
                    	sommets[7] = Qualif::Vecteur(nd8.X(), nd8.Y(), nd8.Z());

                    	maille_tmp->Init_Sommets(sommets);
                    	double crit = maille_tmp->AppliqueCritere((Qualif::VALIDITY));
#ifdef _DEBUG2
                    	std::cout<<"AppliqueCritere pour i,j,k: "<<i<<","<<j<<","<<k<<", id = "<<id<<" => "<<crit<<std::endl;
#endif
                    	if(crit==1.) {
                    		nbPos++;
                    	}
                    	else {
                    		// on teste la maille inversée
                    		nbTestsInv++;

                        	sommets[4] = Qualif::Vecteur(nd1.X(), nd1.Y(), nd1.Z());
                        	sommets[5] = Qualif::Vecteur(nd2.X(), nd2.Y(), nd2.Z());
                        	sommets[6] = Qualif::Vecteur(nd3.X(), nd3.Y(), nd3.Z());
                        	sommets[7] = Qualif::Vecteur(nd4.X(), nd4.Y(), nd4.Z());
                        	sommets[0] = Qualif::Vecteur(nd5.X(), nd5.Y(), nd5.Z());
                        	sommets[1] = Qualif::Vecteur(nd6.X(), nd6.Y(), nd6.Z());
                        	sommets[2] = Qualif::Vecteur(nd7.X(), nd7.Y(), nd7.Z());
                        	sommets[3] = Qualif::Vecteur(nd8.X(), nd8.Y(), nd8.Z());
                        	maille_tmp->Init_Sommets(sommets);
                        	double crit = maille_tmp->AppliqueCritere((Qualif::VALIDITY));
#ifdef _DEBUG2
                        	std::cout<<"AppliqueCritere inversé pour i,j,k: "<<i<<","<<j<<","<<k<<", id = "<<id<<" => "<<crit<<std::endl;
#endif
                        	if(crit==1.) {
                        		nbPosInv++;
                        	}
                    	}
						delete [] sommets;
                	}
                }
            }
        }

        if (nbPos<nbPosInv)
        	areRegionsInverted = true;
#ifdef _DEBUG2
        std::cout<<" nbTests = "<<nbTests<<std::endl;
        std::cout<<" nbPos = "<<nbPos<<std::endl;
        std::cout<<" nbTestsInv = "<<nbTestsInv<<std::endl;
        std::cout<<" nbPosInv = "<<nbPosInv<<std::endl;
        std::cout<<" areRegionsInverted = "<<areRegionsInverted<<std::endl;
        std::cout<<" areRegionsTested = "<<areRegionsTested<<std::endl;
#endif

        if ((nbPos != nbTests) && (nbPosInv != nbTestsInv)){
			TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << "Des hexaédres semblent inversés "
                    << "dans le maillage structuré du bloc "
                    << bl->getName()
                    <<". Il est recommandé de vérifier la qualité du maillage "
                    <<"et de vérifier que les blocs sont bien tels qu'attendus.";
            getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));

            bl->getMeshingData()->setMeshCrossed(true);
        }
        delete maille_tmp;
    }

    // les hexaèdres
    for (uint k=kBegin; k<kEnd; k++) {
        for (uint j=jBegin; j<jEnd; j++) {
            for (uint i=iBegin; i<iEnd; i++) {
                gmds::Node nd1 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j,k));
                gmds::Node nd2 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j,k));
                gmds::Node nd3 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j+1,k));
                gmds::Node nd4 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j+1,k));
                gmds::Node nd5 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j,k+1));
                gmds::Node nd6 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j,k+1));
                gmds::Node nd7 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j+1,k+1));
                gmds::Node nd8 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j+1,k+1));

                gmds::Region r = gmds::Region();

                if(!areRegionsInverted) {
                	r = getGMDSMesh().newHex(nd1,nd2,nd3,nd4,nd5,nd6,nd7,nd8);
                } else {
                	r = getGMDSMesh().newHex(nd5,nd6,nd7,nd8,nd1,nd2,nd3,nd4);
                }
                elem.push_back(r.getID());
                command->addCreatedRegion(r.getID());

            } // for (uint i=iBegin; i<iEnd; i++) {
        } // for (uint j=jBegin; j<jEnd; j++) {
    } // for (uint k=kBegin; k<kEnd; k++) {

    // s'il y a une dégénérescence, on traite ici la création d'une couche de mailles
    if (bl->getNbVertices() != 8){  // K MAX
        bool degI = (bl->getFace(2)->getNbVertices() == 3);
        bool degJ = (bl->getFace(0)->getNbVertices() == 3);

        if(!areRegionsTested) {
        	for (uint k=kEnd; k<kEnd+1; k++) {
        		for (uint j=jBegin; j<jEnd; j++) {
        			for (uint i=iBegin; i<iEnd; i++){

                    	if(
                    		(k==kBegin || k==kEnd-1 || (k-kBegin)%testOnDir==(kEnd-kBegin)%testOnDir) &&
                    		(j==jBegin || j==jEnd-1 || (j-jBegin)%testOnDir==(jEnd-jBegin)%testOnDir) &&
                    		(i==iBegin || i==iEnd-1 || (i-iBegin)%testOnDir==(iEnd-iBegin)%testOnDir)
                    	) {
                    		areRegionsTested = true;

                    		nbTests++;
                    		gmds::Node nd1 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j,k));
                    		gmds::Node nd2 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j,k));
                    		gmds::Node nd3 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j+1,k));
                    		gmds::Node nd4 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j+1,k));
                    		gmds::Node nd5 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j,k+1));
        					gmds::Node nd6 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j,k+1));
        					gmds::Node nd7 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j+1,k+1));
        					gmds::Node nd8 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j+1,k+1));

        					// check whether the polyedron generated should be inverted because of a wrong
        					// block-edges order

        					if (degI && degJ) {
        						Qualif::Vecteur *sommets = new Qualif::Vecteur[5];  // pyramide
        						sommets[0] = Qualif::Vecteur(nd1.X(),nd1.Y(),nd1.Z());
        						sommets[1] = Qualif::Vecteur(nd2.X(),nd2.Y(),nd2.Z());
        						sommets[2] = Qualif::Vecteur(nd3.X(),nd3.Y(),nd3.Z());
        						sommets[3] = Qualif::Vecteur(nd4.X(),nd4.Y(),nd4.Z());
        						sommets[4] = Qualif::Vecteur(nd5.X(),nd5.Y(),nd5.Z());

        						Qualif::Pyramide* maille_tmp = new Qualif::Pyramide(sommets);
        						double crit = maille_tmp->AppliqueCritere((Qualif::JACOBIENMIN));
        						if(crit>0.0) {
        							nbPos++;
        						}
        						else {
        							// on teste la maille inversée
									nbTestsInv++;
									sommets[3] = Qualif::Vecteur(nd1.X(),nd1.Y(),nd1.Z());
									sommets[2] = Qualif::Vecteur(nd2.X(),nd2.Y(),nd2.Z());
									sommets[1] = Qualif::Vecteur(nd3.X(),nd3.Y(),nd3.Z());
									sommets[0] = Qualif::Vecteur(nd4.X(),nd4.Y(),nd4.Z());
									sommets[4] = Qualif::Vecteur(nd5.X(),nd5.Y(),nd5.Z());
									maille_tmp->Init_Sommets(sommets);
									double crit = maille_tmp->AppliqueCritere((Qualif::JACOBIENMIN));
									if(crit>0.0)
										nbPosInv++;
        						}
        						delete [] sommets;
        						delete maille_tmp;

        					} else if (degI) {
        						Qualif::Vecteur *sommets = new Qualif::Vecteur[6];
        						sommets[0] = Qualif::Vecteur(nd1.X(),nd1.Y(),nd1.Z());
        						sommets[1] = Qualif::Vecteur(nd5.X(),nd5.Y(),nd5.Z());
        						sommets[2] = Qualif::Vecteur(nd2.X(),nd2.Y(),nd2.Z());
        						sommets[3] = Qualif::Vecteur(nd4.X(),nd4.Y(),nd4.Z());
        						sommets[4] = Qualif::Vecteur(nd8.X(),nd8.Y(),nd8.Z());
        						sommets[5] = Qualif::Vecteur(nd3.X(),nd3.Y(),nd3.Z());

        						Qualif::Prisme* maille_tmp = new Qualif::Prisme(sommets);
        						double crit = maille_tmp->AppliqueCritere((Qualif::JACOBIENMIN));
        						if(crit>0.0) {
        							nbPos++;
        						}
        						else {
									nbTestsInv++;
									sommets[3] = Qualif::Vecteur(nd1.X(),nd1.Y(),nd1.Z());
        							sommets[4] = Qualif::Vecteur(nd5.X(),nd5.Y(),nd5.Z());
        							sommets[5] = Qualif::Vecteur(nd2.X(),nd2.Y(),nd2.Z());
        							sommets[0] = Qualif::Vecteur(nd4.X(),nd4.Y(),nd4.Z());
        							sommets[1] = Qualif::Vecteur(nd8.X(),nd8.Y(),nd8.Z());
        							sommets[2] = Qualif::Vecteur(nd3.X(),nd3.Y(),nd3.Z());
        							maille_tmp->Init_Sommets(sommets);
        							double crit = maille_tmp->AppliqueCritere((Qualif::JACOBIENMIN));
        							if(crit>0.0)
        								nbPosInv++;
        						}
        						delete [] sommets;
        						delete maille_tmp;

        					} else if (degJ) {
        						Qualif::Vecteur *sommets = new Qualif::Vecteur[6];
        						sommets[0] = Qualif::Vecteur(nd1.X(),nd1.Y(),nd1.Z());
        						sommets[1] = Qualif::Vecteur(nd4.X(),nd4.Y(),nd4.Z());
        						sommets[2] = Qualif::Vecteur(nd5.X(),nd5.Y(),nd5.Z());
        						sommets[3] = Qualif::Vecteur(nd2.X(),nd2.Y(),nd2.Z());
        						sommets[4] = Qualif::Vecteur(nd3.X(),nd3.Y(),nd3.Z());
        						sommets[5] = Qualif::Vecteur(nd6.X(),nd6.Y(),nd6.Z());

        						Qualif::Prisme* maille_tmp = new Qualif::Prisme(sommets);
        						double crit = maille_tmp->AppliqueCritere((Qualif::JACOBIENMIN));
        						if(crit>0.0) {
        							nbPos++;
        						}
        						else {
									nbTestsInv++;
									sommets[3] = Qualif::Vecteur(nd1.X(),nd1.Y(),nd1.Z());
            						sommets[4] = Qualif::Vecteur(nd4.X(),nd4.Y(),nd4.Z());
            						sommets[5] = Qualif::Vecteur(nd5.X(),nd5.Y(),nd5.Z());
            						sommets[0] = Qualif::Vecteur(nd2.X(),nd2.Y(),nd2.Z());
            						sommets[1] = Qualif::Vecteur(nd3.X(),nd3.Y(),nd3.Z());
            						sommets[2] = Qualif::Vecteur(nd6.X(),nd6.Y(),nd6.Z());
            						maille_tmp->Init_Sommets(sommets);
            						double crit = maille_tmp->AppliqueCritere((Qualif::JACOBIENMIN));
            						if(crit>0.0)
            							nbPosInv++;
        						}
        						delete [] sommets;
        						delete maille_tmp;
        					}
                        }

                    } // for (uint i=iBegin; i<iEnd; i++){
        		} // for (uint j=jBegin; j<jEnd; j++) {
        	} // 	for (uint k=kEnd; k<kEnd+1; k++) {

        	if (nbPos<nbPosInv)
            	areRegionsInverted = true;
#ifdef _DEBUG2
            std::cout<<" nbTests = "<<nbTests<<std::endl;
            std::cout<<" nbPos = "<<nbPos<<std::endl;
            std::cout<<" nbTestsInv = "<<nbTestsInv<<std::endl;
            std::cout<<" nbPosInv = "<<nbPosInv<<std::endl;
            std::cout<<" areRegionsInverted = "<<areRegionsInverted<<std::endl;
            std::cout<<" areRegionsTested = "<<areRegionsTested<<std::endl;
#endif

            if ((nbPos != nbTests) && (nbPosInv != nbTestsInv)){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            	message << "MeshImplementation::_addRegionsInVolumes "
            			<<"produit des pyramides ou prismes inversés "
						<< "lors du maillage structuré du bloc "
						<< bl->getName()
						<<". Il est recommandé de vérifier la qualité du maillage "
						<<"et de vérifier que les blocs sont bien tels qu'attendus.";
            	getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::ERROR));

            	bl->getMeshingData()->setMeshCrossed(true);
            }

        } // if(!areRegionsTested) {

        for (uint k=kEnd; k<kEnd+1; k++) {
        	for (uint j=jBegin; j<jEnd; j++) {
        		for (uint i=iBegin; i<iEnd; i++) {


        			gmds::Node nd1 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j,k));
        			gmds::Node nd2 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j,k));
        			gmds::Node nd3 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j+1,k));
        			gmds::Node nd4 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j+1,k));
        			gmds::Node nd5 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j,k+1));
        			gmds::Node nd6 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j,k+1));
        			gmds::Node nd7 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j+1,k+1));
        			gmds::Node nd8 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j+1,k+1));

        			gmds::Region r = gmds::Region();
        			if(!areRegionsInverted) {
        				if (degI && degJ) {
        					r = getGMDSMesh().newPyramid(nd1,nd2,nd3,nd4,nd5); // pyramide
        				} else if (degI) {
        					r = getGMDSMesh().newPrism3(nd1,nd5,nd2,nd4,nd8,nd3); // prisme
        				} else if (degJ) {
        					r = getGMDSMesh().newPrism3(nd1,nd4,nd5,nd2,nd3,nd6); // prisme
        				}
        			} else {
        				if (degI && degJ) {
        					r = getGMDSMesh().newPyramid(nd4,nd3,nd2,nd1,nd5); // pyramide
        				} else if (degI) {
        					r = getGMDSMesh().newPrism3(nd1,nd2,nd5,nd4,nd3,nd8); // prisme
        				} else if (degJ) {
        					r = getGMDSMesh().newPrism3(nd1,nd5,nd4,nd2,nd6,nd3); // prisme
        				}
        			}

        			elem.push_back(r.getID());
        			command->addCreatedRegion(r.getID());

        		} // for (uint i=iBegin; i<iEnd; i++){
        	} // for (uint j=jBegin; j<jEnd; j++) {
        } // for (uint k=kEnd; k<kEnd+1; k++) {

	} // if (bl->getNbVertices() != 8){  // K MAX

#undef nodeIJ

    // ajout des polyedres aux volumes
    for (size_t i=0; i<groupsName.size(); i++){
        std::string& nom = groupsName[i];

        bool isNewVolume = false;
        try {
            //std::cout<<"appel à getVolume("<<nom<<")"<<std::endl;
            Mesh::Volume* vol = getContext().getLocalMeshManager().getVolume(nom);
            //std::cout<<"vol est détruit : "<<(vol->isDestroyed()?"vrai":"faux")<<std::endl;
            //std::cout<<"appel à addModifiedVolume("<<nom<<")"<<std::endl;
            command->addModifiedVolume(nom);
            //std::cout<<"fin normale"<<std::endl;
        } catch (...) {
            //std::cout<<"saute à addNewVolume("<<nom<<")"<<std::endl;
            command->addNewVolume(nom);
            isNewVolume = true;
            //std::cout<<"fin alternative"<<std::endl;
        }
        // le volume de maillage que l'on vient de créer/modifier
        Mesh::Volume* vol = getContext().getLocalMeshManager().getVolume(nom);
        vol->addBlock(bl);
        if (isNewVolume)
            getContext().newGraphicalRepresentation (*vol);
    } // end for i<groupsName.size()

#ifdef _DEBUG2
    std::cout<<"_addRegionsInVolumes, nbTests = "<<nbTests<<std::endl;
#endif

} // end _addRegionsInVolumes
#undef _DEBUG2
/*----------------------------------------------------------------------------*/
void MeshImplementation::_addFacesInSurfaces(Mesh::CommandCreateMesh* command, Topo::CoFace* fa)
{
    std::vector<std::string> groupsName;
    fa->getGroupsName(groupsName);

#ifdef _DEBUG_GROUP_BY_TOPO_ENTITY
    // on ajoute un groupe pour distinguer les faces en mode debug
    groupsName.push_back(fa->getName());
#endif

    // on ajoute la face dans un groupe pour permettre de la représenter
    if (groupsName.empty() && getContext().getLocalMeshManager().coFaceAllwaysInGroups())
    	groupsName.push_back(getContext().getLocalGroupManager().getDefaultName(2));

    // création des polygones même si aucune surface de maillage les attend
    // en effet, ces mailles seront utilies pour la représentation des blocs structurés

    uint nbBrasI, nbBrasJ;
    fa->getNbMeshingEdges(nbBrasI, nbBrasJ);

    const uint nbNoeudsI = nbBrasI + 1;
    const uint nbNoeudsJ = nbBrasJ + 1;

    std::vector<gmds::TCellID>& nodes = fa->nodes();
    std::vector<gmds::TCellID>& elem = fa->faces();

    gmds::IGMesh& gmds_mesh = getGMDSMesh();

#define nodeIJ(ii,jj) nodes[ii+(jj)*nbNoeudsI]

    //#ifdef _DEBUG
    //    // affichage des noeuds de la surface, id puis coordonnées
    //    std::cout<<"Noeuds dans la face "<<fa->getName()<<std::endl;
    //
    //    std::cout<<"Coordonnées des noeuds:\n";
    //    for (uint j=0; j<nbNoeudsJ; j++){
    //         for (uint i=0; i<nbNoeudsI; i++)
    //             std::cout<<nodeIJ(i,j)<<" : "<<getCoordNode(nodeIJ(i,j))<<std::endl;
    //         std::cout<<std::endl;
    //     }
    //#endif

    uint iBegin = 0, iEnd = nbBrasI;
    uint jBegin = 0, jEnd = nbBrasJ;

    // les triangles (juste à une extrémité: j_max)
    if (fa->getNbVertices() == 3){
        jEnd-=1;
        for (uint i=iBegin; i<iEnd; i++)
            for (uint j=jEnd; j<jEnd+1; j++){
                gmds::Node nd1 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j));
                gmds::Node nd2 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j));
                gmds::Node nd3 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j+1));

                gmds::Face f = getGMDSMesh().newTriangle(nd1,nd2,nd3);
                elem.push_back(f.getID());
                command->addCreatedFace(f.getID());
            }
    }

    // teste si deux polygones sont de sens opposés
    Utils::Math::Vector vect1;
    bool isOpposite = false;

    // les quadrangles (reste de la surface)
    for (uint i=iBegin; i<iEnd; i++)
        for (uint j=jBegin; j<jEnd; j++){
            gmds::Node nd1 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j));
            gmds::Node nd2 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j));
            gmds::Node nd3 = gmds_mesh.get<gmds::Node>(nodeIJ(i+1,j+1));
            gmds::Node nd4 = gmds_mesh.get<gmds::Node>(nodeIJ(i,j+1));


            gmds::Face f = getGMDSMesh().newQuad(nd1,nd2,nd3,nd4);
            //std::cout<<"Création du quad d'id "<<f<< " ("<<nd1<<", "<<nd2<<", "<<nd3<<", "<<nd4<<")\n";
            elem.push_back(f.getID());
            command->addCreatedFace(f.getID());

            // vect2 = (nd2-nd1)*(nd4-nd1)
            Utils::Math::Vector vect2 = Utils::Math::Vector(nd2.X()-nd1.X(), nd2.Y()-nd1.Y(), nd2.Z()-nd1.Z())
                                      * Utils::Math::Vector(nd4.X()-nd1.X(), nd4.Y()-nd1.Y(), nd4.Z()-nd1.Z());

            if (j!=jBegin){
            	double scal = vect1.dot(vect2);
            	isOpposite = (scal<0.0)||isOpposite;
            	//std::cout<<" dans CoFace "<<fa->getName()<<" i="<<i<<", j="<<j<<", scal = "<<scal<<std::endl;
            }

            vect1 = vect2;
        }
#undef nodeIJ

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
        sf->addCoFace(fa);
    } // end for i<groupsName.size()

    if (isOpposite){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message << "Des polygones semblent retournés "
    			<< "dans la face structurée "
    			<< fa->getName()
				<< " (normales de directions opposées dans une même face, ce n'est pas forcément un pb)";
    	getContext().getLogStream()->log (TkUtil::TraceLog (message, TkUtil::Log::WARNING));
    } // end if (isOpposite)

} // end _addFacesInSurfaces
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
