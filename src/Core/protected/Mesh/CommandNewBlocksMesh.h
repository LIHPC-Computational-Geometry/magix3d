/*----------------------------------------------------------------------------*/
/*
 * \file CommandNewBlocksMesh.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 8/12/2011
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWBLOCKSMESH_H_
#define COMMANDNEWBLOCKSMESH_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/CommandCreateMesh.h"
#include <Utils/MgxThreadedTaskManager.h>
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Mesh
{


/*----------------------------------------------------------------------------*/
/** \class CommandNewBlocksMesh
 *  \brief Commande permettant de créer le maillage pour une liste de blocs
 */
/*----------------------------------------------------------------------------*/
class CommandNewBlocksMesh: public Mesh::CommandCreateMesh{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour une liste de blocs
     *
     *  \param c le contexte
     *  \param blocks une liste de blocs
     *  \param nombre de tâches qui seront au maximum lancées en concurrence
     */
    CommandNewBlocksMesh(Internal::Context& c, std::vector<Mgx3D::Topo::Block*>& blocks, size_t tasksNum);

    /** \brief  Constructeur pour tous les blocs existants
     *
     *  \param c le contexte
     *  \param nombre de tâches qui seront au maximum lancées en concurrence
     */
    CommandNewBlocksMesh(Internal::Context& c, size_t tasksNum);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewBlocksMesh();

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
    /** Ajoute un message (Warning - pop-up) à la commande comme quoi des blocs
     *  ont des mailles qui semblent retournées
     */
    void signalMeshCrossed(std::vector<std::string>& blockCrossed);

    /*------------------------------------------------------------------------*/
    /// stocke les blocs non maillés et retourne une erreur s'il n'y en a pas
    virtual void validate(std::vector<Topo::Block* > &blocs);

    /*------------------------------------------------------------------------*/
    /// les blocs pour lesquels on souhaite réaliser le maillage
    std::vector<Topo::Block* > m_blocks;

};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* COMMANDNEWBLOCKMESH_H_ */
