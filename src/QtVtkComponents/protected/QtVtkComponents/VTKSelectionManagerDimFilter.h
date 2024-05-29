/**
 * \file		VTKSelectionManagerDimFilter.h
 * \author		Charles PIGNEROL
 * \date		18/10/2012
 */

#ifndef VTK_SELECTION_MANAGER_DIM_FILTER_H
#define VTK_SELECTION_MANAGER_DIM_FILTER_H


#include "Utils/SelectionManagerDimFilter.h"
#include "QtVtkComponents/VTKMgx3DSelectionManager.h"


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace QtVtkComponents {


/**
 * <P>Classe de gestionnaire de sélection Magix 3D pour une IHM reposant sur
 * <I>VTK</I> et dont la politique évènementielle de sélection interactive
 * repose sur un mécanisme de sélection filtrant les entités sélectionnables
 * selon leur dimension et leur type.
 * </P>
 * <P>Les observateurs de ce gestionnaires de sélections sont informés du
 * modification de la politique de gestion (filtres utilisés sur la dimension
 * et/ou le type d'entités) via leur méthode <I>selectionPolicyModified</I>.
 * </P>
 *
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 * @date		18/10/2012
 */
class VTKSelectionManagerDimFilter :
						public Mgx3D::Utils::SelectionManagerDimFilter,
						public Mgx3D::QtVtkComponents::VTKMgx3DSelectionManager
{
	public :

	/**
	 * Constructeur.
	 * \param		Nom unique du gestionnaire de sélection
	 * \param		los le LogOutputStream
	 */
	VTKSelectionManagerDimFilter (
					const std::string& name, TkUtil::LogOutputStream* los);

	/**
	 * Invoque selectionManagerDeleted de chacun de ses observateurs.
	 */
	virtual ~VTKSelectionManagerDimFilter ( );

	/**
	 * Politique évènementielle et évènements pouvant arriver au gestionnaire de
	 * sélection.
	 * L'invocation de ces méthodes rend <I>sélectionnable</I> ou non
	 * les entités de dimensions correspondantes.
	 */
	//@{

	/**
	 * Activation de la sélection d'entités de dimensions transmises en premier
	 * argument et de type correspondant au masque transmis en 2nd argument.
	 * \see		SelectionManagerIfc::selectionPolicyModified
	 */
	virtual void activateSelection (
			SelectionManagerIfc::DIM dimensions, Mgx3D::Utils::FilterEntity::objectType mask = Mgx3D::Utils::FilterEntity::All);

	//@}	// Politique évènementielle.


	protected :


	private :

	/**
	 * Constructeur de copie : interdit.
	 */
	VTKSelectionManagerDimFilter (const VTKSelectionManagerDimFilter&);

	/**
	 * Opérateur = : interdit.
	 */
	VTKSelectionManagerDimFilter& operator = (
									const VTKSelectionManagerDimFilter&);
};	// class VTKSelectionManagerDimFilter

/*----------------------------------------------------------------------------*/
}	// end namespace QtVtkComponents

/*----------------------------------------------------------------------------*/
}	// end namespace Mgx3D

/*----------------------------------------------------------------------------*/

#endif	// VTK_SELECTION_MANAGER_DIM_FILTER_H
