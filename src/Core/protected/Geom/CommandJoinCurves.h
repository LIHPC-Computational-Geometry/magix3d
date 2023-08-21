/*----------------------------------------------------------------------------*/
/*
* CommandJoinCurves.h
 *
 *  Created on: 17 déc. 2013
 *      Author: ledouxf
 *
 *  Refait le 25/6/2015
 *  par Eric B
 */
/*----------------------------------------------------------------------------*/

#ifndef COMMANDJOINCURVES_H_
#define COMMANDJOINCURVES_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Geom/CommandJoinEntities.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandJoinCurves
 *  \brief Commande permettant de réunir plusieurs courbes en une seule
 */
/*----------------------------------------------------------------------------*/
class CommandJoinCurves: public CommandJoinEntities
{
public:

	/*------------------------------------------------------------------------*/
	/** \brief  Constructeur
	 *
	 *  \param c le contexte
	 *  \param e les entités à réunir
	 */
	CommandJoinCurves(Internal::Context& c,
			std::vector<GeomEntity*>& e);

	/*------------------------------------------------------------------------*/
	/** \brief  Destructeur
	 */
	virtual ~CommandJoinCurves();


	/*------------------------------------------------------------------------*/
	/** \brief  exécute la commande
	 */
	void internalSpecificExecute();

	/*------------------------------------------------------------------------*/
	/// valide les paramètres (uniquement des courbes)
	virtual void validate();

private:
	/// recherche d'une courbe qui contienne un sommet
	GeomEntity* getCurveContains(std::vector<GeomEntity*>& entities, Vertex* vtx);

	/// recherche du sommet opposé sur une courbe
	Vertex* getOppositeVtx(GeomEntity* crv, Vertex* vtx);

	/// recherche d'une courbe à partir d'un sommet en excluant une première courbe
	GeomEntity* getCurve(Vertex* vtx, GeomEntity* crvExclude);
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDJOINCURVES_H_ */
/*----------------------------------------------------------------------------*/
