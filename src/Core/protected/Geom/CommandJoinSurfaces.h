/*----------------------------------------------------------------------------*/
/*
 * CommandJoinSurfaces.h
 *
 *  Created on: 10 mars 2014
 *      Author: ledouxf
 *  Refait le 23/6/2015
 *  	Par Eric B
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDJOINSURFACES_H_
#define COMMANDJOINSURFACES_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandJoinEntities.h"
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
/** \class CommandJoinSurfaces
 *  \brief Commande permettant de réunir plusieurs surfaces en une seule
 */
/*----------------------------------------------------------------------------*/
class CommandJoinSurfaces: public CommandJoinEntities
{
public:

	/*------------------------------------------------------------------------*/
	/** \brief  Constructeur
	 *
	 *  \param c le contexte
	 *  \param e les entités à réunir
	 */
	CommandJoinSurfaces(Internal::Context& c,
			std::vector<GeomEntity*>& e);

	/*------------------------------------------------------------------------*/
	/** \brief  Destructeur
	 */
	virtual ~CommandJoinSurfaces();


	/*------------------------------------------------------------------------*/
	/** \brief  exécute la commande
	 */
	virtual void internalSpecificExecute();

	/*------------------------------------------------------------------------*/
	/** \brief  Initialise les attributs de cette classe dans les constructeurs
	 *          des classes filles.
	 */
	virtual void init(std::vector<GeomEntity*>& es);

	/*------------------------------------------------------------------------*/
	/// valide les paramètres (uniquement des surfaces)
	virtual void validate();

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDJOINSURFACES_H_ */
/*----------------------------------------------------------------------------*/

