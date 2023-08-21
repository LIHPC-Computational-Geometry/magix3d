/*----------------------------------------------------------------------------*/
/*
 * \file CommandFuse2Faces.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2 déc. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDFUSE2FACES_H_
#define COMMANDFUSE2FACES_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandFuse2Faces
 *  \brief Commande permettant de fusionner 2 faces pour unir 2 blocs
 *
 *
 */
/*----------------------------------------------------------------------------*/
class CommandFuse2Faces: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param face_A une face commune qui sera conservée
     *  \param face_B une face commune qui est remplacée par face_A
     */
    CommandFuse2Faces(Internal::Context& c, CoFace* face_A, CoFace* face_B);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandFuse2Faces();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** Ce qui est fait après la commande suivant le cas en erreur ou non
     */
    virtual void postExecute(bool hasError);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités modifiées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:
    /// face qui sera conservée
    CoFace* m_face_A;
    /// face qui perd sa place
    CoFace* m_face_B;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDFUSE2FACES_H_ */
/*----------------------------------------------------------------------------*/
