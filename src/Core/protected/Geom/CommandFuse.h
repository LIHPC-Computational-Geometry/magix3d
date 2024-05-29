/*----------------------------------------------------------------------------*/
/** \file CommandFuse.h
 *
 *  \author Franck Ledoux
 *
 *  \date 04/02/2011
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDFUSE_H_
#define COMMANDFUSE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandFuse
 *  \brief Commande permettant de faire la fusion Booléenne de 2 géométries
 */
/*----------------------------------------------------------------------------*/
class CommandFuse: public CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param e les entités dont ont veut récupérer la fusion
     */
    CommandFuse(Internal::Context& c,std::vector<GeomEntity*> e);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandFuse();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();


    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();


protected:

    void validate();
    /* entités à unir */
    std::vector<GeomEntity*> m_entities;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDFUSE_H_ */
/*----------------------------------------------------------------------------*/


