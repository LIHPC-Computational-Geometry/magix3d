/*----------------------------------------------------------------------------*/
/*
 * \file CommandRotateMesh.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/2/2016
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDROTATEMESH_H_
#define COMMANDROTATEMESH_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/CommandTransformMesh.h"
#include "Utils/Rotation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** \class CommandRotateMesh
 *  \brief Commande permettant de faire une rotation de tout le maillage
 *
 */
/*----------------------------------------------------------------------------*/
class CommandRotateMesh: public CommandTransformMesh {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour rotation de tout
     *
     *  \param c le contexte
     *  \param rot la rotation
     */
    CommandRotateMesh(Internal::Context& c,
			const Utils::Math::Rotation& rot);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandRotateMesh();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** Ce qui est fait de propre à la commande pour annuler une commande */
    virtual void internalUndo();

private:
    /// la rotation
    Utils::Math::Rotation m_rotation;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDROTATEMESH_H_ */
/*----------------------------------------------------------------------------*/
