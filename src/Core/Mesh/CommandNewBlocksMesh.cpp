/*----------------------------------------------------------------------------*/
/*
 * \file CommandNewBlocksMesh.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 8/12/2011
 */
/*----------------------------------------------------------------------------*/
#include "Mesh/CommandNewBlocksMesh.h"

#include "Topo/TopoManager.h"
#include "Topo/Block.h"
#include "Topo/CoFace.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
#include "Topo/TopoHelper.h"

#include "Internal/Context.h"
#include "Internal/ClientServerProperties.h"

#include "Utils/Magix3DEvents.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/MemoryError.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Timer.h>
/*----------------------------------------------------------------------------*/
//#define _DEBUG2
//#define _DEBUG_TIMER
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {


/*----------------------------------------------------------------------------*/
CommandNewBlocksMesh::
CommandNewBlocksMesh(Internal::Context& c, std::vector<Mgx3D::Topo::Block*>& blocks, size_t tasksNum)
: CommandCreateMesh(c, "Création du maillage pour des blocs", tasksNum)
{
    validate(blocks);
}
/*----------------------------------------------------------------------------*/
CommandNewBlocksMesh::
CommandNewBlocksMesh(Internal::Context& c, size_t tasksNum)
: CommandCreateMesh(c, "Création du maillage pour tous les blocs", tasksNum)
{
    std::vector<Topo::Block* > blocs;
    getContext().getLocalTopoManager().getBlocks(blocs, true);

    validate(blocs);
}
/*----------------------------------------------------------------------------*/
void CommandNewBlocksMesh::
validate(std::vector<Topo::Block* > &blocs)
{
	for (std::vector<Topo::Block* >::iterator iter = blocs.begin();
			iter != blocs.end(); ++iter) {
		if (!(*iter)->isMeshed())
			m_blocks.push_back(*iter);
	}

	if (m_blocks.empty())
		throw TkUtil::Exception (TkUtil::UTF8String ("Il n'y a pas de bloc à mailler (c'est peut-être déjà fait)", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandNewBlocksMesh::~CommandNewBlocksMesh()
{
}
/*----------------------------------------------------------------------------*/
void CommandNewBlocksMesh::
internalExecute()
{
	setStepNum (11);		// L'exécution se fait en 11 étapes ...
	size_t	step	= 0;	// Etape courrante de la commande
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewBlocksMesh::execute pour la commande " << getName ( )
                     << " de nom unique " << getUniqueName ( );

#ifdef _DEBUG2
	std::cout<<"CommandNewBlocksMesh::internalExecute()"<<std::endl;
	std::cout<<" getNbNodes() = "<<getContext().getLocalMeshManager().getNbNodes()<<std::endl;
	std::cout<<" getNbFaces() = "<<getContext().getLocalMeshManager().getNbFaces()<<std::endl;
	std::cout<<" getNbRegions() = "<<getContext().getLocalMeshManager().getNbRegions()<<std::endl;
#endif

	setStepProgression (1.);
	setStep (++step, "Recensement des faces", 0.);
	std::list<Topo::CoFace*> list_cofaces;
    std::vector<Topo::CoFace* > cofaces;
    for (uint i=0; i<m_blocks.size(); i++){
        m_blocks[i]->getCoFaces(cofaces);

        for (uint j=0; j<cofaces.size(); j++)
            if (!cofaces[j]->isMeshed())
                list_cofaces.push_back(cofaces[j]);
    }
    list_cofaces.sort(Utils::Entity::compareEntity);
    list_cofaces.unique();

#ifdef _DEBUG_TIMER
   TkUtil::Timer timer(false);
#endif



    // maille et modifie le maillage pour les modifications 2D, s'il y en a
    // cette étape est à effectuer avant le maillage de toutes les arêtes car le lissage
    // peut déplacer les extrémités des arêtes.
#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
    setStepProgression (1.);
	setStep (++step, "Lissage et perturbation des surfaces", 0.);

    meshAndModify(list_cofaces);
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"création et modification du maillage surfacique en "<<timer.strDuration()<<std::endl;
#endif


    // le maillage se faisait avant les perturbations mais dans ce cas les sommets ne bougent plus.
    // pas de trace de pourquoi il était indiqué de faire le maillage des sommets avant de perturber
    setStepProgression (1.);
    setStep (++step, "Maillage des sommets", 0.);
    std::vector<Topo::Vertex*> vertices;
    Topo::TopoHelper::getVertices(m_blocks, vertices);

 #ifdef _DEBUG_TIMER
     timer.reset();
     timer.start();
 #endif
     for (uint i=0; i<vertices.size(); i++){
     	if (Command::CANCELED == getStatus ( ))
     		break;
     	mesh(vertices[i]);
     }
 #ifdef _DEBUG_TIMER
     timer.stop();
     std::cout<<"  maillage des sommets en "<<timer.strDuration()<<std::endl;
 #endif



    std::vector<Topo::CoEdge*> aretes;
    Topo::TopoHelper::getCoEdges(m_blocks, aretes);

#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
    setStepProgression (1.);
	setStep (++step, "Calcul des points du maillage des arêtes", 0.);
	preMesh(aretes);

    setStepProgression (1.);
	setStep (++step, "Maillage des arêtes", 0.);

    for (uint i=0; i<aretes.size(); i++){
    	if (Command::CANCELED == getStatus ( ))
    		break;
    	mesh(aretes[i]);
    }
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"  maillage des arêtes en "<<timer.strDuration()<<std::endl;
#endif

    std::vector<Topo::CoFace*> faces;
    Topo::TopoHelper::getCoFaces(m_blocks, faces);

#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif

    setStepProgression (1.);
	setStep (++step, "Pré-maillage des faces", 0.);
	preMesh (faces);

#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"  calcul des points du maillage des faces en "<<timer.strDuration()<<std::endl;
#endif

#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
    setStepProgression (1.);
	setStep (++step, "Maillage des faces", 0.);
    double nbTotFa = (double)faces.size();
    double nbFaitsFa = 0;
    for (uint i=0; i<faces.size(); i++){
    	if (Command::CANCELED == getStatus ( ))
    		break;
    	mesh(faces[i]);
    	nbFaitsFa += 1.0;
    	setStepProgression(nbFaitsFa/nbTotFa);
    	notifyObserversForModifications();
    }
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"  maillage des faces en "<<timer.strDuration()<<std::endl;
#endif


#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif

    setStepProgression (1.);
	setStep (++step, "Pré-maillage des blocs", 0.);
	preMesh (m_blocks);

#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"  calcul des points du maillage des blocs structurés en "<<timer.strDuration()<<std::endl;
#endif


#ifdef _DEBUG_TIMER
    timer.reset();
    timer.start();
#endif
    setStepProgression (1.);
    setStep (++step, "Maillage des blocs", 0.);
    double nbTotBl = (double)m_blocks.size();
    double nbFaitsBl = 0.0;
#ifdef _DEBUG2
    std::cout<<"nbTotBl = "<<nbTotBl<<std::endl;
#endif
    for (uint i=0; i<m_blocks.size(); i++){
    	//std::cout<<"getStatus ( ) => "<<getStrStatus()<<std::endl;
    	if (Command::CANCELED == getStatus ( ))
    		break;

    	mesh(m_blocks[i]);
    	nbFaitsBl += 1.0;
    	setStepProgression(nbFaitsBl/nbTotBl);
    	notifyObserversForModifications();
    }
#ifdef _DEBUG_TIMER
    timer.stop();
    std::cout<<"  maillage des blocs en "<<timer.strDuration()<<std::endl;
#endif


    setStepProgression (1.);
	setStep (++step, "Vérifications", 0.);
    // vérification de la présence de blocs avec mailles croisées
    std::vector<std::string> blockCrossed;
    for (std::vector<Topo::Block*>::iterator iter=m_blocks.begin(); iter!=m_blocks.end(); ++iter)
    	if ((*iter)->getMeshingData()->isMeshCrossed())
    		blockCrossed.push_back((*iter)->getName());
    if (!blockCrossed.empty())
    	signalMeshCrossed(blockCrossed);

    // lisse si nécessaire les blocs
    setStepProgression (1.);
	setStep (++step, "Lissage des blocs", 0.);
    modify(m_blocks);

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
unsigned long CommandNewBlocksMesh::getEstimatedDuration (PLAY_TYPE playType)
{
	uint nbTot = 0;
	// Vérification des blocs, et des entités en dessous (faces)
	for (std::vector<Topo::Block* >::iterator iter = m_blocks.begin();
			iter != m_blocks.end(); ++iter) {
		nbTot += (*iter)->check();
	}

    uint val = nbTot/50000; // 50 000 polyèdres à la seconde
#ifdef _DEBUG2
    std::cout<<"CommandNewBlocksMesh::getEstimatedDuration => "<<val<<std::endl;
#endif
    return val;
//return 50000 * 100;
}
/*----------------------------------------------------------------------------*/
void CommandNewBlocksMesh::signalMeshCrossed(std::vector<std::string>& blockCrossed)
{
	if (blockCrossed.empty())
		return;

	m_warning_to_pop_up << (short)blockCrossed.size();
	if (blockCrossed.size() == 1){
		m_warning_to_pop_up<<" bloc semble avoir des mailles croisées";
		m_warning_to_pop_up<<"\nIl s'agit de "<<blockCrossed[0];
	}
	else {
		m_warning_to_pop_up<<" blocs semblent avoir des mailles croisées";
		m_warning_to_pop_up<<"\nIl s'agit de";
		for (uint i=0; i<blockCrossed.size(); i++)
			m_warning_to_pop_up<<" "<<blockCrossed[i];
	}

	std::cout<<"WARNING: "<<m_warning_to_pop_up.iso()<<std::endl;
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
