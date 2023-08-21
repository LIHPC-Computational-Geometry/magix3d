/*----------------------------------------------------------------------------*/
/*
 * \file CommandNewFacesMesh.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 23/5/2012
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Mesh/CommandNewFacesMesh.h"
#include "Utils/MgxException.h"
#include "Internal/Context.h"
#include "Topo/TopoManager.h"
#include "Topo/CoFace.h"
#include "Topo/TopoHelper.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
CommandNewFacesMesh::
CommandNewFacesMesh(Internal::Context& c, std::vector<Mgx3D::Topo::CoFace*>& faces, size_t tasksNum)
: CommandCreateMesh(c, "Création du maillage pour des faces", tasksNum)
{
    std::vector<Topo::CoFace* > hfaces;
    for (std::vector<Mgx3D::Topo::CoFace*>::iterator iter = faces.begin();
            iter != faces.end(); ++iter) {
        hfaces.push_back(*iter);
    }
    validate(hfaces);
}
/*----------------------------------------------------------------------------*/
CommandNewFacesMesh::
CommandNewFacesMesh(Internal::Context& c, size_t tasksNum)
: CommandCreateMesh(c, "Création du maillage pour toutes les faces", tasksNum)
{
    std::vector<Topo::CoFace* > faces;
    getContext().getLocalTopoManager().getCoFaces(faces);

    validate(faces);
}
/*----------------------------------------------------------------------------*/
void CommandNewFacesMesh::
validate(std::vector<Topo::CoFace* > &faces)
{
    for (std::vector<Topo::CoFace* >::iterator iter = faces.begin();
            iter != faces.end(); ++iter) {
        if (!(*iter)->isMeshed())
            m_faces.push_back(*iter);
    }

    if (m_faces.empty())
        throw TkUtil::Exception (TkUtil::UTF8String ("Il n'y a pas de face à mailler (c'est peut-être déjà fait)", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandNewFacesMesh::~CommandNewFacesMesh()
{
}
/*----------------------------------------------------------------------------*/
unsigned long CommandNewFacesMesh::getEstimatedDuration (PLAY_TYPE playType)
{
	uint nbTot = 0;
	// vérification que les faces sont correctes
	for (std::vector<Topo::CoFace* >::iterator iter = m_faces.begin();
			iter != m_faces.end(); ++iter) {
		nbTot += (*iter)->check();
	}

    uint val = nbTot/2000; // 2 000 polyèdres à la seconde
#ifdef _DEBUG2
    std::cout<<"CommandNewBlocksMesh::getEstimatedDuration => "<<val<<std::endl;
#endif
    return val;
}
/*----------------------------------------------------------------------------*/
void CommandNewFacesMesh::
internalExecute()
{
	setStepNum (7);			// L'exécution se fait en 5 étapes ...
	size_t	step	= 1;	// Etape courrante de la commande
	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "CommandNewFacesMesh::execute pour la commande " << getName ( )
                     << " de nom unique " << getUniqueName ( );

	setStepProgression (1.);
	setStep (step, "Recensement des faces", 0.);
    std::list<Topo::CoFace*> list_cofaces;

    for (uint j=0; j<m_faces.size(); j++)
    	list_cofaces.push_back(m_faces[j]);
    list_cofaces.sort(Utils::Entity::compareEntity);
    list_cofaces.unique();

    // Vérification des faces, et des entités en dessous
    for (std::vector<Topo::CoFace* >::iterator iter = m_faces.begin();
            iter != m_faces.end(); ++iter)
        (*iter)->check();

    // maille et modifie le maillage pour les modifications 2D, s'il y en a
    setStepProgression (1.);
	setStep (++step, "Lissage et perturbation des surfaces", 0.);
    meshAndModify(list_cofaces);

	setStepProgression (1.);
	setStep (++step, "Maillage des sommets", 0.);
    std::vector<Topo::Vertex*> vertices;
    Topo::TopoHelper::getVertices(m_faces, vertices);

    for (uint i=0; i<vertices.size(); i++){
    	if (Command::CANCELED == getStatus ( ))
    		break;
    	mesh(vertices[i]);
    }

    std::vector<Topo::CoEdge*> aretes;
    Topo::TopoHelper::getCoEdges(m_faces, aretes);

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

    // effectue le maillage des faces communes
    // en forçant la visibilité des mailles hors groupes (en les mettant dans Hors Groupe 2D)
    getContext().getLocalMeshManager().setCoFaceAllwaysInGroups(true);

	setStepProgression (1.);
	setStep (++step, "Pré-maillage des faces", 0.);
	preMesh (m_faces);

	setStepProgression (1.);
	setStep (++step, "Maillage des faces", 0.);
    double nb_faces = (double)m_faces.size();
    double nbFaits = 0.0;
    for (std::vector<Topo::CoFace* >::iterator iter = m_faces.begin();
    		iter != m_faces.end(); ++iter){
    	if (Command::CANCELED == getStatus ( ))
    		break;
    	mesh(*iter);
    	nbFaits += 1.0;
    	setProgression(nbFaits/nb_faces);
    }
	setStepProgression (1.);
    getContext().getLocalMeshManager().setCoFaceAllwaysInGroups(false);

    // on parcours les entités modifiées pour sauvegarder leur état d'avant la commande
    saveInternalsStats();

    log (TkUtil::TraceLog (message, TkUtil::Log::TRACE_1));
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
