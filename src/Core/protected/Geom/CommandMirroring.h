/*
 * CommandMirroring.h
 *
 *  Created on: 12/4/2016
 *      Author: Eric B
 */

#ifndef COMMANDMIRRORING_H_
#define COMMANDMIRRORING_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Utils/Plane.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandMirroring
 *  \brief Commande permettant de faire une symétrie
 */
/*----------------------------------------------------------------------------*/
class CommandMirroring: public Geom::CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param e les entités à scaler
     *  \param plane le plan de symétrie
     */
    CommandMirroring(Internal::Context& c,
            std::vector<GeomEntity*>& e,
			Utils::Math::Plane* plane);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cmd la commande de construction des entités à déplacer
     *  \param plane le plan de symétrie
     */
    CommandMirroring(Internal::Context& c,
    		Geom::CommandGeomCopy* cmd,
			Utils::Math::Plane* plane);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param plane le plan de symétrie
     */
    CommandMirroring(Internal::Context& c,
			Utils::Math::Plane* plane);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandMirroring();

    /*------------------------------------------------------------------------*/
     /** \brief  exécute la commande
      */
     void internalSpecificExecute();


     /*------------------------------------------------------------------------*/
     /** \brief  initialisation de la commande
      */
     void internalSpecificPreExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  Traitement spécifique délégué aux classes filles
     */
    void internalSpecificUndo();
    void internalSpecificRedo();

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDMIRRORING_H_ */
/*----------------------------------------------------------------------------*/

