/*----------------------------------------------------------------------------*/
/*
 * \file CommandNewFacesMesh.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 23/5/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWFACESMESH_H_
#define COMMANDNEWFACESMESH_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/CommandCreateMesh.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** \class CommandNewFacesMesh
 *  \brief Commande permettant de créer le maillage pour une liste de faces communes
 */
/*----------------------------------------------------------------------------*/
class CommandNewFacesMesh: public Mesh::CommandCreateMesh{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une liste de faces
     *
     *  \param c le contexte
     *  \param faces une liste de faces communes
     *  \param nombre de tâches qui seront au maximum lancées en concurrence
     */
    CommandNewFacesMesh(Internal::Context& c, std::vector<Mgx3D::Topo::CoFace*>& faces, size_t tasksNum);

    /** \brief  Constructeur pour toutes les faces communes existantes
     *
     *  \param c le contexte
     *  \param nombre de tâches qui seront au maximum lancées en concurrence
     */
    CommandNewFacesMesh(Internal::Context& c, size_t tasksNum);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewFacesMesh();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /**
     * la commande a vocation à être lancée dans un autre <I>thread</I>.
     */
    virtual bool threadable ( ) const
        { return true; }

    /**
     * la commande peut être interrompue par appel
     * de la méthode <I>cancel</I>
     */
    virtual bool cancelable ( ) const
        { return true; }

    /*------------------------------------------------------------------------*/
    /// estimation du temps prévu pour mailler les blocs
    virtual unsigned long getEstimatedDuration (PLAY_TYPE playType = DO);


protected:
    /*------------------------------------------------------------------------*/
    /// stocke les faces non maillées et retourne une erreur s'il n'y en a pas
    void validate(std::vector<Topo::CoFace* > &faces);


    /*------------------------------------------------------------------------*/
    /// les faces pour lesquels on souhaite réaliser le maillage
    std::vector<Topo::CoFace* > m_faces;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* COMMANDNEWFACESMESH_H_ */
