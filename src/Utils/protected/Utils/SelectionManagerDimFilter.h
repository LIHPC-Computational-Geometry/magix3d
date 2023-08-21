/**
 * \file		SelectionManagerDimFilter.h
 * \author		Charles PIGNEROL
 * \date		14/10/2013
 */

#ifndef SELECTION_MANAGER_DIM_FILTER_H
#define SELECTION_MANAGER_DIM_FILTER_H


#include "Utils/SelectionManager.h"


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace Utils {


/**
 * <P>Classe de gestionnaire de sélection Magix 3D pour une IHM
 * dont la politique évènementielle de sélection interactive
 * repose sur un mécanisme de sélection filtrant les entités sélectionnables
 * selon leur dimension et leur type.
 * </P>
 * <P>Les observateurs de ce gestionnaires de sélections sont informés du
 * modification de la politique de gestion (filtres utilisés sur la dimension
 * et/ou le type d'entités) via leur méthode <I>selectionPolicyModified</I>.
 * </P>
 *
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 * @date		14/10/2013
 */
class SelectionManagerDimFilter : public virtual Mgx3D::Utils::SelectionManager
{
	public :

	/**
	 * Constructeur.
	 * \param		Nom unique du gestionnaire de sélection
	 * \param		los le LogOutputStream
	 */
	SelectionManagerDimFilter (
					const std::string& name, TkUtil::LogOutputStream* los);

	/**
	 * Invoque selectionManagerDeleted de chacun de ses observateurs.
	 */
	virtual ~SelectionManagerDimFilter ( );

	/**
	 * Politique évènementielle et évènements pouvant arriver au gestionnaire de
	 * sélection.
	 * L'invocation de ces méthodes rend <I>sélectionnable</I> ou non
	 * les entités de dimensions correspondantes.
	 */
	//@{

	/**
	 * \return		<I>true</I> si la sélection est activée pour l'entité
	 *				transmise en argument, sinon <I>false</I>. Le choix
	 *				repose sur la dimension et le type de l'entité.
	 * \see			is0DSelectionActivated
	 * \see			is1DSelectionActivated
	 * \see			is2DSelectionActivated
	 * \see			is3DSelectionActivated
	 * \see			getFilteredTypes
	 */
	virtual bool isSelectionActivated (const Mgx3D::Utils::Entity&) const;

	/**
	 * \return		Le masque de sélection sur le type d'entité sélectionnable.
	 * \see			getFilteredDimensions
	 */
	virtual Mgx3D::Utils::FilterEntity::objectType getFilteredTypes ( ) const;

	/**
	 * \return		Le masque de sélection sur les dimensions sélectionnables.
	 * \see			getFilteredTypes
	 */
	virtual SelectionManagerIfc::DIM getFilteredDimensions ( ) const;

	/**
	 * \return		<I>true</I> si la sélection d'entités 0D est activée, sinon
	 *				<I>false</I>.
	 * \see			activateSelection
	 */
	virtual bool is0DSelectionActivated ( ) const;
	
	/**
	 * \return		<I>true</I> si la sélection d'entités 1D est activée, sinon
	 *				<I>false</I>.
	 * \see			activateSelection
	 */
	virtual bool is1DSelectionActivated ( ) const;

	/**
	 * \return		<I>true</I> si la sélection d'entités 2D est activée, sinon
	 *				<I>false</I>.
	 * \see			activateSelection
	 */
	virtual bool is2DSelectionActivated ( ) const;

	/**
	 * \return		<I>true</I> si la sélection d'entités 3D est activée, sinon
	 *				<I>false</I>.
	 * \see			activateSelection
	 */
	virtual bool is3DSelectionActivated ( ) const;

	/**
	 * Activation de la sélection d'entités 3D de dimensions et de type
	 * correspondant au masque.
	 * \see		SelectionManagerIfc::selectionPolicyModified
	 */
	virtual void activateSelection (
			SelectionManagerIfc::DIM dimensions,
			Mgx3D::Utils::FilterEntity::objectType mask = Mgx3D::Utils::FilterEntity::All);

	//@}	// Politique évènementielle.


	protected :


	private :

	/**
	 * Constructeur de copie : interdit.
	 */
	SelectionManagerDimFilter (const SelectionManagerDimFilter&);

	/**
	 * Opérateur = : interdit.
	 */
	SelectionManagerDimFilter& operator = (
									const SelectionManagerDimFilter&);

	/** Les dimensions d'entités sélectionnables. */
	SelectionManagerIfc::DIM					_dimensions;

	/** Les types d'entités sélectionnables. */
	Mgx3D::Utils::FilterEntity::objectType		_filteredTypes;
};	// class SelectionManagerDimFilter

/*----------------------------------------------------------------------------*/
}	// end namespace Utils

/*----------------------------------------------------------------------------*/
}	// end namespace Mgx3D

/*----------------------------------------------------------------------------*/

#endif	// SELECTION_MANAGER_DIM_FILTER_H
