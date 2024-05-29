/**
 * \file		QtVtkRepresentationTypesDialog.h
 * \author		Charles PIGNEROL
 * \date		24/11/2014
 */
#ifndef QT_VTK_REPRESENTATION_TYPES_DIALOG_H
#define QT_VTK_REPRESENTATION_TYPES_DIALOG_H


#include "QtComponents/QtRepresentationTypesDialog.h"
#include <QtVtk/QtVTKFontParametersPanel.h>
#include <QDialog>


namespace Mgx3D 
{

namespace QtComponents
{
class QtMgx3DMainWindow;
}	// namespace QtComponents


namespace QtVtkComponents
{


/**
 * Boite de dialogue pemettant la sélection de types de représentation à
 * utiliser pour afficher des entités 3D : points, filaire, ...
 * Cette classe est une spécialisation pour environnement <I>VTK</I>.
 */
class QtVtkRepresentationTypesDialog : public Mgx3D::QtComponents::QtRepresentationTypesDialog
{
	public :

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Nom de l'application
	 * \param	<I>true</I> si on est en mode d'utilisation des propriétés globales d'affichage, <I>false</I> dans le cas contraire.
	 * \param	Entités soumises aux modifications.
	 * \param   URL de l'aide
	 * \param   Tag de l'aide dans la page précédemment donnée
	 */
	QtVtkRepresentationTypesDialog (
			Mgx3D::QtComponents::QtMgx3DMainWindow* parent,
			const std::string& appTitle, const std::string& name,
			const std::vector<Mgx3D::Utils::Entity*>& entities,
			const std::string& helpURL, const std::string& helpTag
		);

	/**
	 * Constructeur avec choix prédéfinis (a priori pour mode C/S).
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	<I>true</I> si on est en mode d'utilisation des propriétés globales d'affichage, <I>false</I> dans le cas contraire.
	 * \param	Valeur initiale pour la représentation de type <I>points</I>.
	 * \param	Valeur initiale pour la représentation de type <I>filaire</I>.
	 * \param	Valeur initiale pour la représentation de type <I>complément filaire</I>.
	 * \param	Valeur initiale pour la représentation de type <I>surfaces</I>.
     * \param   Valeur initiale pour la représentation de type <I>volumes</I>.
     * \param   Valeur initiale pour la représentation de type <I>noms</I>.
	 * \param	Valeur initiale pour la représentation de type <I>associations</I>.
	 * \param	Valeur initiale pour la couleur utilisée pour les entités topologiques (standard/entité géométrique associée).
     * \param   Valeur initiale pour la représentation de type <I>meshShape</I>.
     * \param   Valeur initiale pour la représentation de type <I>discRep</I>.
	 * \param	Valeur initiale pour la représentation de type <I>nbMeshRep</I>.
     * \param   Rend visible ou non le bouton pour le type <I>points</I>.
     * \param   Rend visible ou non le bouton pour le type <I>filaire</I>.
     * \param   Rend visible ou non le bouton pour le type <I>complément filaire</I>.
     * \param   Rend visible ou non le bouton pour le type <I>surfaces</I>.
     * \param   Rend visible ou non le bouton pour le type <I>volumes</I>.
     * \param   Rend visible ou non le bouton pour le type <I>noms</I>.
     * \param   Rend visible ou non le bouton pour le type <I>associations</I>.
     * \param   Rend visible ou non le bouton pour le type <I>meshShape</I>.
     * \param   Rend visible ou non le bouton pour le type <I>discRep</I>.
     * \param   Rend visible ou non le bouton pour le type <I>nbMeshRep</I>.
	 * \param	Couleur initiale pour le type <I>points</I>.
	 * \param	Couleur initiale pour le type <I>filaire</I>.
	 * \param	Couleur initiale pour le type <I>complément filaire</I>.
	 * \param	Couleur initiale pour le type <I>surfaces</I>.
	 * \param	Couleur initiale pour le type <I>volumes</I>.
	 * \param	La couleur initiale pour le type <I>points</I> est elle valide ?
	 * \param	La couleur initiale pour le type <I>filaire</I> est elle valide ?
	 * \param	La couleur initiale pour le type <I>complément filaire</I> est  elle valide ?
	 * \param	La couleur initiale pour le type <I>surfaces</I> est elle valide ?
	 * \param	La couleur initiale pour le type <I>volumes</I> est elle valide ?
	 * \param	Opacité initiale pour le type <I>points</I>.
	 * \param	Opacité initiale pour le type <I>filaire</I>.
	 * \param	Opacité initiale pour le type <I>complément filaire</I>.
	 * \param	Opacité initiale pour le type <I>surfaces</I>.
	 * \param	Opacité initiale pour le type <I>volumes</I>.
	 * \param	L'opacité initiale pour le type <I>points</I> est elle valide ?
	 * \param	L'opacité initiale pour le type <I>filaire</I> est elle valide ?
	 * \param	L'opacité initiale pour le type <I>complément filaire</I> est  elle valide ?
	 * \param	L'opacité initiale pour le type <I>surfaces</I> est elle valide ?
	 * \param	L'opacité initiale pour le type <I>volumes</I> est elle valide ?
	 * \param	Facteur de rétrécissement initial (0 - 1, 1 => affichage normal)
	 * \param	Le facteur de rétrécissement initial est il valide ?
	 * \param	Facteur d'agrandissement des flèches association (0 - 10, 1 => affichage normal)
	 * \param	Le facteur d'agrandissement initial est il valide ?
	 * \param	La fonte utilisée pour les affichages textuels
	 * \param	<I>true</I> s'il faut autoriser l'affichage de valeurs aux noeuds ou aux mailles.
	 * \param	Type de valeur affichée par défaut.
	 * \param	Liste des valeurs aux noeuds affichables,
	 * \param	Liste des valeurs aux mailles affichables,
	 * \param	Nom de la valeur (éventuellement) affichée proposée par défaut.
	 * \param   URL de l'aide
	 * \param   Tag de l'aide dans la page précédemment donnée
	 */
	QtVtkRepresentationTypesDialog (Mgx3D::QtComponents::QtMgx3DMainWindow* parent, const std::string& name, bool globalMode,
			TkUtil::THREESTATES repCloud, TkUtil::THREESTATES repCurves,
			TkUtil::THREESTATES repIsoCurves, TkUtil::THREESTATES repSurfaces,
            TkUtil::THREESTATES repVolumes, TkUtil::THREESTATES repNames,
			TkUtil::THREESTATES repAssociations, TkUtil::THREESTATES topoColor,
            TkUtil::THREESTATES repMeshShape, TkUtil::THREESTATES repDiscretisationType,
			TkUtil::THREESTATES repNbMesh,
            bool pointsSelectable, bool linesSelectable,
            bool isoLinesSelectable, bool surfacesSelectable,
            bool volumesSelectable, bool namesSelectable,
			bool associationsSelectable,
            bool meshShapeSelectable, bool discRepSelectable,
            bool nbMeshSelectable,
			const TkUtil::Color& cloudColor, 
			const TkUtil::Color& curvesColor, 
			const TkUtil::Color& isoCurvesColor, 
			const TkUtil::Color& surfacesColor, 
			const TkUtil::Color& volumesColor,
			bool cloudColorOK, bool curvesColorOK, bool isoCurvesColorOK,
			bool surfacesColorOK, bool volumesColorOK,
			double cloudOpacity, double curvesOpacity, double isoCurvesOpacity,
			double surfacesOpacity, double volumesOpacity,
			bool cloudOpacityOK, bool curvesOpacityOK, bool isoCurvesOpacityOK,
			bool surfacesOpacityOK, bool volumesOpacityOK,
			double shrinkFactor, bool shrinkFactorOK,
			double arrowComul, bool arrowComulOK,
			int fontFamily, int fontSize,
			TkUtil::THREESTATES bold, TkUtil::THREESTATES italic,
			const TkUtil::Color& fontColor,
			bool displayValue, Mgx3D::QtComponents::QtRepresentationTypesPanel::DISPLAYED_VALUE displayedValue,
			const std::set< std::string >& nodesValuesNames,
			const std::set< std::string >& cellsValuesNames,
			const std::string& displayedValueName,
			const std::string& helpURL, const std::string& helpTag
		);

	/**
	 * RAS.
	 */
	virtual ~QtVtkRepresentationTypesDialog ( );


	protected :

	/**
	 * Panneau de saisie de la fonte.
	 */
	class QtVtkFontPropertiesPanel : public QtComponents::QtRepresentationTypesPanel::QtAbstractFontPropertiesPanel
	{
		public :

		/**
		 * Constructeur par défaut.
		 */
        QtVtkFontPropertiesPanel (QWidget* parent, const std::string& name,
            int fontFamily, int fontSize,
			TkUtil::THREESTATES bold, TkUtil::THREESTATES italic,
            const TkUtil::Color& fontColor);

		virtual ~QtVtkFontPropertiesPanel ( );

		virtual void getFontProperties (
					int& family, int& size,
					TkUtil::THREESTATES& bold, TkUtil::THREESTATES& italic,
					TkUtil::Color& color, bool& colorModified) const;


		private :

		QtVtkFontPropertiesPanel (const QtVtkFontPropertiesPanel&);
		QtVtkFontPropertiesPanel& operator = (
										const QtVtkFontPropertiesPanel&);

		/** Le panneau permettant de paramétrer la fonte à utiliser pour les
		 * affichages 2D. */
		QtVTKFontParametersPanel*		_fontPanel;
	};	// class QtVtkFontPropertiesPanel


	private :

	// Opérations interdites :
	QtVtkRepresentationTypesDialog (const QtVtkRepresentationTypesDialog&);
	QtVtkRepresentationTypesDialog& operator = (const QtVtkRepresentationTypesDialog&);
};	// class QtVtkRepresentationTypesDialog


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// QT_VTK_REPRESENTATION_TYPES_DIALOG_H
