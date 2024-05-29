/*----------------------------------------------------------------------------*/
/*
 * \file CommandTranslateMesh.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/2/2016
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDTRANSLATEMESH_H_
#define COMMANDTRANSLATEMESH_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/CommandTransformMesh.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** \class CommandTranslateMesh
 *  \brief Commande permettant de déplacer tout le maillage
 *
 */
/*----------------------------------------------------------------------------*/
class CommandTranslateMesh: public CommandTransformMesh {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param dp le vecteur de translation
     */
    CommandTranslateMesh(Internal::Context& c, const Vector& dp);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandTranslateMesh();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** Ce qui est fait de propre à la commande pour annuler une commande */
    virtual void internalUndo();

private:
    /// le vecteur de translation
    Utils::Math::Vector m_vector;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDTRANSLATEMESH_H_ */
/*----------------------------------------------------------------------------*/
