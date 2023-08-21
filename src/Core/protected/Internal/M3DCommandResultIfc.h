/*----------------------------------------------------------------------------*/
/** \file		M3DCommandResultIfc.h
 *	\author		Charles
 *	\date		02/04/2013
 */
/*----------------------------------------------------------------------------*/

#ifndef M3D_COMMAND_RESULT_IFC_H
#define M3D_COMMAND_RESULT_IFC_H

#include "Utils/CommandResult.h"

#include <vector>
#include <string>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Internal
{


/** \class M3DCommandResultIfc
 * <P>Interface de fourniseur d'accès aux résultats d'une commande
 * <I>Magix 3D</I>qui a été exécutée.
 * </P>
 */
class M3DCommandResultIfc : public Mgx3D::Utils::CommandResult
{
	public :

	/**
	 * Destructeur.
	 */
	virtual ~M3DCommandResultIfc ( );

	/**
	 * Accesseurs aux entités géométriques.
	 */
	//@{

	/**
	 * \return	La liste des volumes créés par la commande.
	 */
	virtual std::vector<std::string> getVolumes ( );

	/**
	 * \return	Le i-ème volume créé par la commande.
	 * \except	Lève une exception s'il n'existe pas de tel volume.
	 */
	virtual std::string getVolume (size_t index);

	/**
	 * \return	Le seul volume créé par la commande.
	 * \except	Lève une exception s'il n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getVolume ( );

	/**
	 * \return	La liste des surfaces créées par la commande.
	 */
	virtual std::vector<std::string> getSurfaces ( );

	/**
	 * \return	La i-ème surface créée par la commande.
	 * \except	Lève une exception si elle n'existe pas de telle surface.
	 */
	virtual std::string getSurface (size_t index);

	/**
	 * \return	La seule surface créée par la commande.
	 * \except	Lève une exception si elle n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getSurface ( );

	/**
	 * \return	La liste des courbe créées par la commande.
	 */
	virtual std::vector<std::string> getCurves ( );

	/**
	 * \return	La i-ème courbe créée par la commande.
	 * \except	Lève une exception si elle n'existe pas de telle courbe.
	 */
	virtual std::string getCurve (size_t index);

	/**
	 * \return	La seule courbe créée par la commande.
	 * \except	Lève une exception si elle n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getCurve ( );

	/**
	 * \return	La liste des point créés par la commande.
	 */
	virtual std::vector<std::string> getVertices ( );

	/**
	 * \return	Le i-ème point créé par la commande.
	 * \except	Lève une exception s'il n'existe pas de tel point.
	 */
	virtual std::string getVertex (size_t index);

	/**
	 * \return	Le seul point créé par la commande.
	 * \except	Lève une exception s'il n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getVertex ( );

	//@}	// Accesseurs aux entités géométriques.

	/**
	 * Accesseurs aux entités topologiques.
	 */
	//@{

	/**
	 * \return	La liste des blocs créés par la commande.
	 */
	virtual std::vector<std::string> getBlocks ( );

	/**
	 * \return	Le i-ème bloc créé par la commande.
	 * \except	Lève une exception s'il n'existe pas de tel bloc.
	 */
	virtual std::string getBlock (size_t index);

	/**
	 * \return	Le seul bloc créé par la commande.
	 * \except	Lève une exception s'il n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getBlock ( );

	/**
	 * \return	La liste des faces créées par la commande.
	 */
	virtual std::vector<std::string> getFaces ( );

	/**
	 * \return	La i-ème face créée par la commande.
	 * \except	Lève une exception si elle n'existe pas de telle face.
	 */
	virtual std::string getFace (size_t index);

	/**
	 * \return	La seule face créée par la commande.
	 * \except	Lève une exception si elle n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getFace ( );

	/**
	 * \return	La liste des arête créées par la commande.
	 */
	virtual std::vector<std::string> getEdges ( );

	/**
	 * \return	La i-ème arête créée par la commande.
	 * \except	Lève une exception si elle n'existe pas de telle arête.
	 */
	virtual std::string getEdge (size_t index);

	/**
	 * \return	La seule arête créée par la commande.
	 * \except	Lève une exception si elle n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getEdge ( );

	/**
	 * \return	La liste des sommet créés par la commande.
	 */
	virtual std::vector<std::string> getTopoVertices ( );

	/**
	 * \return	Le i-ème sommet créé par la commande.
	 * \except	Lève une exception s'il n'existe pas de tel sommet.
	 */
	virtual std::string getTopoVertex (size_t index);

	/**
	 * \return	Le seul sommet créé par la commande.
	 * \except	Lève une exception s'il n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getTopoVertex ( );

	//@}	// Accesseurs aux entités topologiques.

	/**
	 * Accesseurs aux entités groupes.
	 */
	//@{

	/**
	 * \return	La liste des groupes 0D créés par la commande.
	 */
	virtual std::vector<std::string> getGroups0D ( );

	/**
	 * \return	La liste des groupes 1D créés par la commande.
	 */
	virtual std::vector<std::string> getGroups1D ( );

	/**
	 * \return	La liste des groupes 2D créés par la commande.
	 */
	virtual std::vector<std::string> getGroups2D ( );

	/**
	 * \return	La liste des groupes 3D créés par la commande.
	 */
	virtual std::vector<std::string> getGroups3D ( );

	//@}	// Accesseurs aux entités groupes.

	/// accesseur sur un repère local
	virtual std::string getSysCoord ( );


	protected :

	/**
	 * Constructeur.
	 * \param	Commande représentée.
	 */
	M3DCommandResultIfc (Mgx3D::Utils::Command&);

	/**
	 * Constructeur.
	 * sans commande
	 */
	M3DCommandResultIfc ();


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	M3DCommandResultIfc (const M3DCommandResultIfc&);
	M3DCommandResultIfc& operator = (const M3DCommandResultIfc&);
};	// class M3DCommandResultIfc


/*----------------------------------------------------------------------------*/
}	// namespace Internal
/*----------------------------------------------------------------------------*/
}	// namespace Mgx3D
/*----------------------------------------------------------------------------*/



#endif	// M3D_COMMAND_RESULT_IFC_H
/*----------------------------------------------------------------------------*/

