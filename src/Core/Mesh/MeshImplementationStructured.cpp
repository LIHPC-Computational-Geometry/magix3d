/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"

#include "Mesh/MeshImplementation.h"
#include "Mesh/CommandCreateMesh.h"

#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/Edge.h"
#include "Topo/TopoHelper.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyTabulated.h"
#include "Topo/BlockMeshingData.h"
#include "Topo/TopoHelper.h"

#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"

#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Geom/Surface.h"
#include "Geom/GeomProjectImplementation.h"
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
void MeshImplementation::preMeshStructured(Topo::Block* bl)
{
#ifdef _DEBUG_MESH_FUNCTION
	{
    std::cout <<"Maillage du bloc structuré "<<bl->getName()<<" avec la méthode "
            << bl->getMeshLawName();
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
                throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, MeshImplementation::meshStructured pour nbPtI et nbPtJ", TkUtil::Charset::UTF_8));


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
                		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, MeshImplementation::meshStructured, cote trop grand", TkUtil::Charset::UTF_8));
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
                    message << "Erreur interne dans MeshImplementation::meshStructured pour le bloc"
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
                		message << "Erreur interne dans MeshImplementation::meshStructured pour le bloc"
                				<< bl->getName() <<" de taille "<< (short)nbNoeudsI<<" X "<< (short)nbNoeudsJ<<" X "<< (short)nbNoeudsK
                				<<", avec la face "
                				<< face->getName() <<" de taille "<<(short)nbPtI<<" X "<<(short)nbPtJ
                				<< ", en dehors des limites pour nodes de taille "<<nodes.size()
                				<<" alors que l'on cherche à obtenir l'indice "<<(long)iface+jface*ifacesize;
                		throw TkUtil::Exception (message);
                	}
#endif
                    bl->nodes()[ibloc+jbloc] = nodes[iface+jface*ifacesize].id();
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
                        bl->nodes()[ibloc+jbloc] = node.id();
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
                message << "Erreur interne dans MeshImplementation::meshStructured pour le bloc"
                        << bl->getName() << ", les faces dégénérées ne rentre pas dans l'une des 3 possibilitées";
                throw TkUtil::Exception (message);
            } // else ... (bl->getNbVertices() == 5)

        } // end else (cote<bl->getNbFaces())
    } // end for cote<6

    if (bl->getMeshLaw() == Topo::BlockMeshingProperty::transfinite){
        discretiseTransfinie(nbBrasI, nbBrasJ, nbBrasK, l_points);
    } else {
    	throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans MeshImplementation::meshStructured pour block, type de maillage invalide", TkUtil::Charset::UTF_8));
    }
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"création des points d'un bloc "<<timer.cpuDuration()/TkUtil::Timer::cpuPerSecond()<<" secondes"<<std::endl;
    timer.reset();
    timer.start();
#endif

} // preMeshStructured(Block*)
/*----------------------------------------------------------------------------*/
void MeshImplementation::meshStructured(Mesh::CommandCreateMesh* command, Topo::Block* bl)
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
                bl->nodes()[i+nbNoeudsI*j+k*nbNoeudsI*nbNoeudsJ] = nd.id();
                command->addCreatedNode(nd.id());
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
//    std::cout<<"MeshImplementation::meshStructured () avec comme nbNoeuds en I, J et K: "
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

} // meshStructured (Block*)
/*----------------------------------------------------------------------------*/
void MeshImplementation::preMeshStructured(Topo::CoFace* coface)
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
                coface->nodes()[jface] = nodes[iarete].id();
                l_points[jface] = getCoordNode(nodes[iarete]);
            }
        }
        else {
            // cas d'une arête dégénérées (la dernière)
            uint jface = 0;
            uint nbPtI = coface->getEdge(cote-2)->getNbNodes();
            gmds::Node node = getGMDSMesh().get<gmds::Node>(coface->getVertex(0)->getNode());
            for(uint iarete=0, jface=idep[cote]; iarete<nbPtI; iarete++, jface+=ipas[cote]){
                coface->nodes()[jface] = node.id();
                l_points[jface] = getCoordNode(node);
            }
        }

    }// end for cote<4

    if (coface->getMeshLaw() == Topo::CoFaceMeshingProperty::transfinite) {
        discretiseTransfinie(nbBrasI, nbBrasJ, l_points);
    } else {
        throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne dans MeshImplementation::meshStructured pour face, type de maillage invalide", TkUtil::Charset::UTF_8));
    }


    // on ne fait pas de projection sur la surface pour les discrétisations
    // directionnelles et rotationnelles
    if (coface->getGeomAssociation()){
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
                        pt = Geom::GeomProjectImplementation().project(surf, pt).first;
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

} // end preMeshStructured(CoFace*)
/*----------------------------------------------------------------------------*/
void MeshImplementation::meshStructured(Mesh::CommandCreateMesh* command, Topo::CoFace* coface)
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
            if (nd.id()==gmds::NullID){
				TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            	message << "Création d'un noeud GMDS en erreur pour la face "<< coface->getName();
            	throw TkUtil::Exception (message);
            }
#endif
            coface->nodes()[i+nbNoeudsI*j] = nd.id();
            command->addCreatedNode(nd.id());
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

    gmds::Mesh& gmds_mesh = getGMDSMesh();
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
                elem.push_back(r.id());
                command->addCreatedRegion(r.id());

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

                    elem.push_back(r.id());
                    command->addCreatedRegion(r.id());

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
            Mesh::Volume* vol = getContext().getMeshManager().getVolume(nom);
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
        Mesh::Volume* vol = getContext().getMeshManager().getVolume(nom);
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
    if (groupsName.empty() && getContext().getMeshManager().coFaceAllwaysInGroups())
    	groupsName.push_back(getContext().getGroupManager().getDefaultName(2));

    // création des polygones même si aucune surface de maillage les attend
    // en effet, ces mailles seront utilies pour la représentation des blocs structurés

    uint nbBrasI, nbBrasJ;
    fa->getNbMeshingEdges(nbBrasI, nbBrasJ);

    const uint nbNoeudsI = nbBrasI + 1;
    const uint nbNoeudsJ = nbBrasJ + 1;

    std::vector<gmds::TCellID>& nodes = fa->nodes();
    std::vector<gmds::TCellID>& elem = fa->faces();

    gmds::Mesh& gmds_mesh = getGMDSMesh();

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
                elem.push_back(f.id());
                command->addCreatedFace(f.id());
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
            elem.push_back(f.id());
            command->addCreatedFace(f.id());

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
            getContext().getMeshManager().getSurface(nom);
            command->addModifiedSurface(nom);
        } catch (...) {
            command->addNewSurface(nom);
        }
        // la surface de maillage que l'on vient de créer/modifier
        Mesh::Surface* sf = getContext().getMeshManager().getSurface(nom);
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
