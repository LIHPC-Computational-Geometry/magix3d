/*----------------------------------------------------------------------------*/
/*
 * \file CommandReplaceTransfiniteMethod.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15/10/13
 */
/*----------------------------------------------------------------------------*/
#include "Topo/CommandReplaceTransfiniteMethod.h"
#include "Topo/Block.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/CoEdge.h"
#include "Topo/TopoHelper.h"
#include "Topo/FaceMeshingPropertyDirectional.h"
#include "Topo/BlockMeshingPropertyDirectional.h"

#include "Utils/Common.h"
#include "Utils/Point.h"

#include "Mesh/MeshImplementation.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/ThreadPool.h>
//#define _DEBUG_TIMER
#ifdef _DEBUG_TIMER
#include <TkUtil/Timer.h>
#endif
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
CommandReplaceTransfiniteMethod::
CommandReplaceTransfiniteMethod(Internal::Context& c)
:Mesh::CommandCreateMesh(c, std::string("Changement de discrétisation pour les blocs et faces avec méthode transfinie"), 0)
{
}
/*----------------------------------------------------------------------------*/
CommandReplaceTransfiniteMethod::
~CommandReplaceTransfiniteMethod()
{
}
/*----------------------------------------------------------------------------*/
//#define _DEBUG_REPLACE
void CommandReplaceTransfiniteMethod::
internalExecute()
{
#ifdef _DEBUG_REPLACE
	std::cout<<"CommandReplaceTransfiniteMethod en cours ..."<<std::endl;
	std::cout<<"mgxTopoDoubleEpsilon = "<<Mgx3D::Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon<<std::endl;
#endif
	setStepNum (4);		// L'exécution se fait en 4 étapes ...
	size_t	step	= 0;	// Etape courrante de la commande

#ifdef _DEBUG_TIMER
	TkUtil::Timer timer(false);
#endif

    setStepProgression (1.);
	setStep (++step, "Calcul des points du maillage des arêtes", 0.);

	// recherche des arêtes rectilignes ou non
	// sert à initialiser m_coedge_isRectiligne
	std::vector<CoEdge*> coedges;
	getContext().getLocalTopoManager().getCoEdges(coedges);
#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
	if (coedges.size()){
		preMesh(coedges);
	} // end if (coedges.size())



#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"calcul du preMesh "<<timer.strDuration()<<std::endl;
#endif


#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
    setStepProgression (1.);
	setStep (++step, "Recherche de la linéarité des arêtes", 0.);
	if (coedges.size()){
		uint nb_vus = 0;
		uint nb_tot = coedges.size();
		uint nb_rectilignes=0;
		for (std::vector<CoEdge*>::iterator iter = coedges.begin();
				iter != coedges.end(); ++iter){
			if (isRectiligne(*iter))
				nb_rectilignes += 1;
			nb_vus += 1.0;
	    	setStepProgression(nb_vus/nb_tot);
	    	notifyObserversForModifications();
		}
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message << "Nb d'arêtes total: "<<(short)coedges.size()<<"\n";
    	message << "Nb d'arêtes rectilignes: "<<(short)nb_rectilignes<<"\n";
    	getContext().getLogStream()->log(TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));

	} // end if (coedges.size())
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"recherche des arêtes rectilignes ou non en "<<timer.strDuration()<<std::endl;
#endif

    // recherche les cofaces avec méthode de maillage transfinie
    std::vector<CoFace*> cofaces;
    getContext().getLocalTopoManager().getCoFaces(cofaces);
#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
    setStepProgression (1.);
	setStep (++step, "Recherche de la possibilité de mailler avec méthode rectiligne pour les cofaces", 0.);
    if (cofaces.size()){
		uint nb_vus = 0;
		uint nb_tot = cofaces.size();
    	// faces concernée par le changement
    	uint nb_transfinites = 0;
    	// faces dont on change la discrétisation
    	uint nb_changed = 0;
    	for (std::vector<CoFace*>::iterator iter = cofaces.begin();
    			iter != cofaces.end(); ++iter){
    		if ((*iter)->getMeshLaw() == CoFaceMeshingProperty::transfinite){
    			CoFace* coface = *iter;
    			nb_transfinites += 1;
#ifdef _DEBUG_REPLACE
    			std::cout<<" pour "<<coface->getName()<<" en cours ..."<<std::endl;
#endif

    			// on vérifie que 2 côtés ont une discrétisation rectiligne
    			// pour passer dans le mode de discrétisation directionnelle
    			// est-ce rectiligne pour un côté donné ?
    			std::vector<bool> rectiligne;
    			for (uint i=0; i<coface->getNbEdges(); i++){
    				rectiligne.push_back(isRectiligne(coface->getEdge(i)));
#ifdef _DEBUG_REPLACE
    				std::cout<<"isRectiligne pour "<<coface->getEdge(i)->getName()
    							<<" => "<<(rectiligne[i]?"vrai":"faux")<<std::endl;
#endif
    			} // end for i<coface->getNbEdges()

    			FaceMeshingPropertyDirectional::meshDirLaw dirLaw = FaceMeshingPropertyDirectional::dir_undef;
    			if (rectiligne.size()>2 && rectiligne[0] && rectiligne[2]
    			    && (coface->getEdge(0)->getNbCoEdges()==1 || coface->getEdge(2)->getNbCoEdges()==1))
    				dirLaw = FaceMeshingPropertyDirectional::dir_j;
    			else if (rectiligne.size()>3 && rectiligne[1] && rectiligne[3]
    			         && (coface->getEdge(1)->getNbCoEdges()==1 || coface->getEdge(3)->getNbCoEdges()==1))
    				dirLaw = FaceMeshingPropertyDirectional::dir_i;

    			if (dirLaw != FaceMeshingPropertyDirectional::dir_undef){
    				nb_changed+=1;
    				coface->saveCoFaceMeshingProperty(&getInfoCommand());
    				FaceMeshingPropertyDirectional* mp = new FaceMeshingPropertyDirectional(dirLaw);
    				coface->switchCoFaceMeshingProperty(&getInfoCommand(), mp);
    				delete mp;
    			}

    		} // end if getMeshLaw() == transfinite
			nb_vus += 1.0;
	    	setStepProgression(nb_vus/nb_tot);
	    	notifyObserversForModifications();
    	} // end for iter

    	// affichage des infos sur faces concernées
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message << "Nb de faces total: "<<(short)cofaces.size()<<"\n";
    	message << "Nb de faces avec transfinie: "<<(short)nb_transfinites<<"\n";
    	message << "Nb de faces changées avec méthode directionnelle: "<<(short)nb_changed<<"\n";
    	message << "Nb de faces qui restent avec méthode transfinie: "<<(short)(nb_transfinites-nb_changed);
    	getContext().getLogStream()->log(TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));

    }// end if (cofaces.size())
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"recherche les cofaces avec méthode de maillage transfinie en "<<timer.strDuration()<<std::endl;
#endif



    // idem avec les blocs

    // recherche les blocs avec méthode de maillage transfinie
    std::vector<Block*> blocs;
    getContext().getLocalTopoManager().getBlocks(blocs);
#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
    setStepProgression (1.);
	setStep (++step, "Recherche de la possibilité de mailler avec méthode rectiligne pour les blocs", 0.);
    if (blocs.size()){
		uint nb_vus = 0;
		uint nb_tot = blocs.size();
    	// blocs concernée par le changement
    	uint nb_transfinites = 0;
    	// blocs dont on change la discrétisation
    	uint nb_changed = 0;
    	for (std::vector<Block*>::iterator iter = blocs.begin();
    			iter != blocs.end(); ++iter){
    		if ((*iter)->getMeshLaw() == BlockMeshingProperty::transfinite){
    			Block* bloc = *iter;
    			nb_transfinites += 1;
#ifdef _DEBUG_REPLACE
    			std::cout<<" pour "<<bloc->getName()<<" en cours ..."<<std::endl;
#endif

    			// on vérifie que les côtés ont une discrétisation rectiligne
    			// pour passer dans le mode de discrétisation directionnelle
    			// est-ce rectiligne pour un côté donné ?

    			std::vector<Topo::CoEdge* > iCoedges[3];
    			bloc->getOrientedCoEdges(iCoedges[0], iCoedges[1], iCoedges[2]);

    			std::vector<Topo::Vertex* > sommets;
    			bloc->getHexaVertices(sommets);

    			// la direction sélectionnée, à définir
    			BlockMeshingProperty::meshDirLaw dirLaw = BlockMeshingProperty::dir_undef;

    			// une des 3 directions convient-elle ?
    			for (uint j=0; j<3 && (dirLaw == BlockMeshingProperty::dir_undef); j++){
    				bool all_rectiligne = true;
    				for (uint k=0; k<4 && all_rectiligne; k++){
    					std::vector<CoEdge* > coedges_dir1;

    					TopoHelper::getCoEdgesBetweenVertices(sommets[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[j][k][0]],
    							sommets[TopoHelper::tabIndVtxByEdgeAndDirOnBlock[j][k][1]],
    							iCoedges[j],
    							coedges_dir1);
    					if (!isRectiligne(coedges_dir1))
    						all_rectiligne = false;

    				} // end for k
    				if (all_rectiligne)
    					dirLaw = (BlockMeshingProperty::meshDirLaw)(j+1);
    			} // end for j

    			if (dirLaw != BlockMeshingProperty::dir_undef){
    				nb_changed+=1;
    				bloc->saveBlockMeshingProperty(&getInfoCommand());
    				BlockMeshingPropertyDirectional* mp = new BlockMeshingPropertyDirectional(dirLaw);
    				bloc->switchBlockMeshingProperty(&getInfoCommand(), mp);
    				delete mp;
    			}

    		} // end if getMeshLaw() == transfinite
			nb_vus += 1.0;
	    	setStepProgression(nb_vus/nb_tot);
	    	notifyObserversForModifications();
    	} // end for iter

    	// affichage des infos sur faces concernées
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    	message << "Nb de blocs total: "<<(short)blocs.size()<<"\n";
    	message << "Nb de blocs avec transfinie: "<<(short)nb_transfinites<<"\n";
    	message << "Nb de blocs changés avec méthode directionnelle: "<<(short)nb_changed<<"\n";
    	message << "Nb de blocs qui restent avec méthode transfinie: "<<(short)(nb_transfinites-nb_changed);
    	getContext().getLogStream()->log(TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));
    } // end if (blocs.size())
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"recherche les blocs avec méthode de maillage transfinie en "<<timer.strDuration()<<std::endl;
#endif

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    // nettoyage de la mémoire
    m_coedge_isRectiligne.clear();
}
/*----------------------------------------------------------------------------*/
void CommandReplaceTransfiniteMethod::getPreviewRepresentation(Utils::DisplayRepresentation& dr)
{
	MGX_FORBIDDEN("getPreviewRepresentation non prévu");
}
/*----------------------------------------------------------------------------*/
bool CommandReplaceTransfiniteMethod::isRectiligne(Edge* edge)
{
	if (edge->getNbCoEdges() == 0)
		return false;
	std::vector<CoEdge*> coedges;
	edge->getCoEdges(coedges);

	return isRectiligne(coedges);
}
/*----------------------------------------------------------------------------*/
bool CommandReplaceTransfiniteMethod::isRectiligne(std::vector<CoEdge*>& coedges)
{
	// vérification pour chacune des coedges si elle est rectiligne
	CoEdge* coedge_prec = 0;
	for (uint i=0; i<coedges.size(); i++){
		CoEdge* coedge = coedges[i];
		if (!isRectiligne(coedge))
			return false;

        // s'il y en a plusieurs, il faut tester la colinéarité
        if (i){
            Utils::Math::Vector v1(coedge_prec->getVertex(0)->getCoord(), coedge_prec->getVertex(1)->getCoord());
            Utils::Math::Vector v2(coedge->getVertex(0)->getCoord(), coedge->getVertex(1)->getCoord());
            v1/=v1.norme();
            v2/=v2.norme();
            Utils::Math::Vector v3 = v1*v2;
            if (v3.abs()>Mgx3D::Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon){
                return false;
            }
        } // end if (i)

		coedge_prec = coedge;

	} // end for i<coedges.size()

	return true;
}
/*----------------------------------------------------------------------------*/
bool CommandReplaceTransfiniteMethod::isRectiligne(CoEdge* coedge)
{
	if (m_coedge_isRectiligne[coedge] == 0){

		// il faut récupérer les points de la coedge
		std::vector<Utils::Math::Point> points;
		coedge->getPoints(points);
#ifdef _DEBUG_REPLACE
		std::cout<<"calcul delta pour "<<coedge->getName()<<" avec "<<points.size()<<" points"<<std::endl;
#endif

		if (points.size()<3)
			m_coedge_isRectiligne[coedge] = 1;
		else {
			// recherche de 2 points distincts
			Utils::Math::Point pt_dep = points.front();
			Utils::Math::Point pt_fin = points.back();
#ifdef _DEBUG_REPLACE2
			std::cout<<"pt_dep: "<<pt_dep<<std::endl;
			std::cout<<"pt_fin: "<<pt_fin<<std::endl;
#endif

			if (pt_dep==pt_fin){
				pt_fin = points[points.size()/2];
				if (pt_dep==pt_fin){
					TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
					messErr << "Erreur interne pour l'arête "<<coedge->getName()
								<< " on tombe sur 3 sommets confondus";
					throw TkUtil::Exception(messErr);
				}
			}

			Utils::Math::Vector v1(pt_dep, pt_fin);
			v1/=v1.norme();
#ifdef _DEBUG_REPLACE2
			std::cout<<"v1: "<<v1<<std::endl;
#endif
			bool isRectiligne = true;
			for (uint i=1; i<points.size() && isRectiligne; i++){
				Utils::Math::Vector v2(pt_dep, points[i]);
				v2/=v2.norme();
				Utils::Math::Vector v3 = v1*v2;
#ifdef _DEBUG_REPLACE2
				std::cout<<"v2: "<<v2<<std::endl;
				std::cout<<"v3: "<<v3<<std::endl;
#endif
				double delta = v3.abs();
#ifdef _DEBUG_REPLACE2
				std::cout<<"delta: "<<delta<<std::endl;
#endif
				if (delta>Mgx3D::Utils::Math::MgxNumeric::mgxTopoDoubleEpsilon){
#ifdef _DEBUG_REPLACE2
					std::cout<<coedge->getName()<<", delta : "<<delta<<std::endl;
#endif
					isRectiligne = false;
				}
			} // end for i<points.size()

			if (isRectiligne)
				m_coedge_isRectiligne[coedge] = 1;
			else
				m_coedge_isRectiligne[coedge] = 2;
		} // end else if (points.size()<3)
	} // end if (m_coedge_isRectiligne[coedge] == 0)


#ifdef _DEBUG_REPLACE
	std::cout<<coedge->getName();
	if (m_coedge_isRectiligne[coedge] == 1)
		std::cout<<" est rectiligne"<<std::endl;
	else
		std::cout<<" n'est pas rectiligne"<<std::endl;
#endif

	if (m_coedge_isRectiligne[coedge] == 1)
		return true;
	else
		return false;
}
/*----------------------------------------------------------------------------*/
unsigned long CommandReplaceTransfiniteMethod::getEstimatedDuration (PLAY_TYPE playType)
{
	uint nbTot = getContext().getLocalTopoManager().getNbEdges();
    uint val = nbTot/100; // 500 arêtes à la seconde
#ifdef _DEBUG2
    std::cout<<"CommandNewBlocksMesh::getEstimatedDuration => "<<val<<std::endl;
#endif
    return val;
}
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
