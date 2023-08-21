/**
 * \file		VTKConfiguration.h
 * \author		Charles PIGNEROL
 * \date		19/10/2012
 */

#ifndef VTK_CONFIGURATION_H
#define VTK_CONFIGURATION_H


#include <PrefsCore/DoubleNamedValue.h>


/*----------------------------------------------------------------------------*/
namespace Mgx3D {

/*----------------------------------------------------------------------------*/
namespace QtVtkComponents {


/**
 * <P>Classe utilitaire de stockage des préférences utilisateurs spécifiques à
 * l'utilisation de <I>VTK</I>.
 * </P>
 * \warning		Ne sont pas entreposées ici les paramètres généraux
 *				d'<I>API 3D</I> tels que l'utilisation de <I>display list</I> ou
 *				autres.
 *
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 * @date		18/10/2012
 */
class VTKConfiguration
{
	public :

	/**
	 * Les préférences utilisateur relatives au paramétrage de <I>VTK</I>.
	 */
	//@{

	static VTKConfiguration& instance ( );
	
	/**
	 * Applique la configuration reçue en argument.
	 * \param		Section racine de la configuration à appliquer.
	 */
	static void applyConfiguration (const Preferences::Section& mainSection);

	/**
	 * Ajoute à la section tranmise en argument la configuration actuelle.
	 * \param		Section principale de la configuration de l'application.
	 * \warning		N'effectue aucune action persistante.
	 */
	static void saveConfiguration (Preferences::Section& mainSection);

	/**
	 * Commentaire de la section <I>Picking</I>.
	 */
	std::string							_pickingComment;

	/**
	 * <P>Les tolérances appliquées lors de la recherche d'entités pointées 
	 * respectivement de dimensions 0, 1, 2 ou 3.</P>
	 * 
	 * <P>La tolérance est spécifiée comme une fraction de la diagonale de la
	 * fenêtre de rendu.
	 * </P>
	 */
	Preferences::DoubleNamedValue		_0DPickerTolerance,
										_1DPickerTolerance,
										_2DPickerTolerance,
										_3DPickerTolerance,
										_cellPickerTolerance;

	//@}	// Les préférences utilisateur relatives au paramétrage de <I>VTK</I>.


	protected :



	private :

	/**
	 * Constructeurs : interdit.
	 */
	VTKConfiguration ( );
	VTKConfiguration (const VTKConfiguration&);

	/**
	 * Opérateur = : interdit.
	 */
	VTKConfiguration& operator = (const VTKConfiguration&);

	/**
	 * Destructeur : interdit.
	 */
	virtual ~VTKConfiguration ( );
	
	static VTKConfiguration*	_instance;
};	// class VTKConfiguration

/*----------------------------------------------------------------------------*/
}	// end namespace QtVtkComponents

/*----------------------------------------------------------------------------*/
}	// end namespace Mgx3D

/*----------------------------------------------------------------------------*/

#endif	// VTK_CONFIGURATION_H
