/*----------------------------------------------------------------------------*/
/*
 * \file CommandTransformMesh.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/2/2016
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDTRANSFORMMESH_H_
#define COMMANDTRANSFORMMESH_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Utils/Vector.h"

// OCC
class gp_Trsf;
class gp_GTrsf;
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/** \class CommandTransformMesh
 *  \brief Commande permettant transformer un maillage
 *
 */
/*----------------------------------------------------------------------------*/
class CommandTransformMesh: public Internal::CommandInternal {

protected:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le contexte
     * \param le nom de la commande
     */
    CommandTransformMesh(Internal::Context& c,
            std::string name);

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandTransformMesh();

    /** Ce qui est fait de propre à la commande pour rejouer une commande */
    virtual void internalRedo();

protected:
    // applique la transformation à tous les noeuds du maillage
    void transform(gp_Trsf* transf);
    void transform(gp_GTrsf* transf);

private:
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDTRANSFORMMESH_H_ */
/*----------------------------------------------------------------------------*/
