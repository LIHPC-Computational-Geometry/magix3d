/**
 * \file		QtRepresentationTypesDialog.h
 * \author		Charles PIGNEROL
 * \date		17/01/2011
 */
#ifndef QT_REPRESENTATION_TYPES_DIALOG_H
#define QT_REPRESENTATION_TYPES_DIALOG_H


#include "Utils/Entity.h"
#include "QtRepresentationTypesPanel.h"
#include <QtUtil/QtDlgClosurePanel.h>
#include <QDialog>

#include <map>


namespace Mgx3D 
{

namespace QtComponents
{

class RenderingManager;
class QtMgx3DMainWindow;


/**
 * Boite de dialogue pemettant la sélection de types de représentation à
 * utiliser pour afficher des entités 3D : points, filaire, ...
 * Ces types de représentation sont ceux définis dans la classe
 * <I>GraphicalEntityRepresentation<I> :
 * <I>CLOUDS<I>, <I>CURVES</I>, ... Pour chaque type de représentation 3 
 * choix sont possibles : <I>ON<I> (utilisation), <I>OFF</I> (non utilisation),
 * <I>UNDETERMINED</I> (indéterminé, valeur utilisée par exemple lorsque
 * initialement certaines entités utilisent un type de représentation et pas
 * d'autres).
 */
class QtRepresentationTypesDialog : public QDialog
{
	Q_OBJECT

	public :

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Nom de l'application
	 * \param	Entités soumises aux modifications.
	 * \param   URL de l'aide
	 * \param   Tag de l'aide dans la page précédemment donnée
	 */
	QtRepresentationTypesDialog (QtMgx3DMainWindow* parent,
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
	QtRepresentationTypesDialog (QtMgx3DMainWindow* parent, const std::string& name, bool globalMode,
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
			bool displayValue, QtRepresentationTypesPanel::DISPLAYED_VALUE displayedValue,
			const std::set< std::string >& nodesValuesNames,
			const std::set< std::string >& cellsValuesNames,
			const std::string& displayedValueName,
			const std::string& helpURL, const std::string& helpTag
		);

	/**
	 * RAS.
	 */
	virtual ~QtRepresentationTypesDialog ( );

	/**
	 * \return		<I>true</I> si une couleur a été modifiée.
	 */
	virtual bool colorModified ( ) const;

	/**
	 * \return		Les propriétés d'affichage définies par cette boite
	 * 				de dialogue.
	 */
	virtual Mgx3D::QtComponents::QtRepresentationTypesPanel::ExtendedDisplayProperties getDisplayProperties ( ) const;

	/**
	 * \return		Le choix de l'utilisateur quant à l'utilisation d'une
	 *				représentation de type <I>points</I>.
	 */
	virtual TkUtil::THREESTATES useCloudRepresentation ( ) const;

	/**
	 * \return		<I>true</I> si la représentation des nuages est modifiée,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool cloudRepresentationModified ( ) const;

	/**
	 * \return		<I>true</I> s'il faut prendre la couleur des nuages en
	 *				compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useCloudColor ( ) const;

	/**
	 * \return		La couleur d'affichage des nuages.
	 */
	virtual TkUtil::Color getCloudColor ( ) const;

	/**
	 * \return		<I>true</I> s'il faut prendre l'opacité des nuages en
	 *				compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useCloudOpacity ( ) const;

	/**
	 * \return		L'opacité des nuages.
	 */
	virtual double getCloudOpacity ( ) const;

	/**
	 * \return		Le choix de l'utilisateur quant à l'utilisation d'une
	 *				représentation de type <I>filaire</I>.
	 */
	virtual TkUtil::THREESTATES useCurvesRepresentation ( ) const;

	/**
	 * \return		<I>true</I> si la représentation des courbes est modifiée,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool curvesRepresentationModified ( ) const;

	/**
	 * \return		<I>true</I> s'il faut prendre la couleur des courbes en
	 *				compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useCurvesColor ( ) const;

	/**
	 * \return		La couleur d'affichage des courbes.
	 */
	virtual TkUtil::Color getCurvesColor ( ) const;

	/**
	 * \return		<I>true</I> s'il faut prendre l'opacité des courbes en
	 *				compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useCurvesOpacity ( ) const;

	/**
	 * \return		L'opacité des courbes.
	 */
	virtual double getCurvesOpacity ( ) const;

	/**
	 * \return		Le choix de l'utilisateur quant à l'utilisation d'une
	 *				représentation de type <I>complément filaire</I>.
	 */
	virtual TkUtil::THREESTATES useIsoCurvesRepresentation ( ) const;

	/**
	 * \return		<I>true</I> si la représentation des <I>compléments
	 *				filaires</I> est modifiée, <I>false</I> dans le cas
	 *				contraire.
	 */
	virtual bool isoCurvesRepresentationModified ( ) const;

	/**
	 * \return		<I>true</I> s'il faut prendre la couleur des <I>compléments
	 *				filaires</I> en
	 *				compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useIsoCurvesColor ( ) const;

	/**
	 * \return		La couleur d'affichage des compléments filaires.
	 */
	virtual TkUtil::Color getIsoCurvesColor ( ) const;

	/**
	 * \return		<I>true</I> s'il faut prendre l'opacité des <I>compléments
	 *				filaires</I> en
	 *				compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useIsoCurvesOpacity ( ) const;

	/**
	 * \return		L'opacité des compléments filaires.
	 */
	virtual double getIsoCurvesOpacity ( ) const;

	/**
	 * \return		Le choix de l'utilisateur quant à l'utilisation d'une
	 *				représentation de type <I>surfaces</I>.
	 */
	virtual TkUtil::THREESTATES useSurfacesRepresentation ( ) const;

	/**
	 * \return		<I>true</I> si la représentation des surfaces est modifiée,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool surfacesRepresentationModified ( ) const;

	/**
	 * \return		<I>true</I> s'il faut prendre la couleur des surfaces en
	 *				compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useSurfacesColor ( ) const;

	/**
	 * \return		La couleur d'affichage des surfaces.
	 */
	virtual TkUtil::Color getSurfacesColor ( ) const;

	/**
	 * \return		<I>true</I> s'il faut prendre l'opacité des surfaces en
	 *				compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useSurfacesOpacity ( ) const;

	/**
	 * \return		L'opacité des surfaces.
	 */
	virtual double getSurfacesOpacity ( ) const;

    /**
     * \return      Le choix de l'utilisateur quant à l'utilisation d'une
     *              représentation de type <I>volumes</I>.
     */
    virtual TkUtil::THREESTATES useVolumesRepresentation ( ) const;

    /**
     * \return      Le choix de l'utilisateur quant à l'utilisation d'une
     *              représentation de type <I>noms</I>.
     */
    virtual TkUtil::THREESTATES useNamesRepresentation ( ) const;

	/**
	 * \return		<I>true</I> si la représentation des volumes est modifiée,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool volumesRepresentationModified ( ) const;

	/**
	 * \return		<I>true</I> s'il faut prendre la couleur des volumes en
	 *				compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useVolumesColor ( ) const;

	/**
	 * \return		La couleur d'affichage des volumes.
	 */
	virtual TkUtil::Color getVolumesColor ( ) const;

	/**
	 * \return		<I>true</I> s'il faut prendre l'opacité des volumes en
	 *				compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useVolumesOpacity ( ) const;

	/**
	 * \return		L'opacité des volumes.
	 */
	virtual double getVolumesOpacity ( ) const;

	/**
	 * \return		<I>true</I> s'il faut prendre le facteur de rétrécissement 
	 *				en compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useShrinkFactor ( ) const;

	/**
	 * \return		Le facteur de rétrécissement
	 */
	virtual double getShrinkFactor ( ) const;

	/**
	 * \return		<I>true</I> s'il faut prendre le facteur d'agrandissement 
	 *			en compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useArrowComul ( ) const;

	/**
	 * \return		Le facteur d'agrandissement des flèches d'associations
	 */
	virtual double getArrowComul ( ) const;

	/**
	 * \return		Les propriétés de représentation textuelle.
	 */	
	virtual void getFontProperties (
					int& family, int& size,	
					TkUtil::THREESTATES& bold, TkUtil::THREESTATES& italic,
					TkUtil::Color& color, bool& colorModified) const;

	/**
	 * \return		Quel type de valeur faut-il éventuellement représenter ?
	 * \see			getDisplayedValueName
	 */
	virtual QtRepresentationTypesPanel::DISPLAYED_VALUE getDisplayedValueType ( ) const;

	/**
	 * \return		Nom de la valeur à représenter éventuellement ?
	 * \see			getDisplayedValueType
	 */
	virtual std::string getDisplayedValueName ( ) const;

    /**
     * \return      Le choix de l'utilisateur quant à l'utilisation d'une
     *              représentation de type <I>associations</I>.
     */
    virtual TkUtil::THREESTATES useAssociationsRepresentation ( ) const;

    /**
     * \return      Le choix de l'utilisateur quant à l'utilisation de
     *              la couleur des entités géométriques associées pour la 
     *              représentation des entités topologiques.
     */
    virtual TkUtil::THREESTATES topoUseGeomColor ( ) const;

    /**
     * \return      Le choix de l'utilisateur quant à l'utilisation d'une
     *              représentation de type <I>meshShape</I>.
     */
    virtual TkUtil::THREESTATES useMeshShapeRepresentation ( ) const;

    /**
      * \return      Le choix de l'utilisateur quant à l'utilisation d'une
      *              représentation de type <I>discretisationRep</I>.
      */
     virtual TkUtil::THREESTATES useDiscretisationRepresentation ( ) const;

     /**
      * \return      Le choix de l'utilisateur quant à l'utilisation d'une
      *              représentation du nombre de bras <I>nbMeshRep</I>.
      */
     virtual TkUtil::THREESTATES useNbMeshRepresentation ( ) const;
     
     /**
      * Pour faire des chose horribles.
      */
     virtual QtDlgClosurePanel& getClosurePanel ( );


	protected :

	/**
	 * \return		Le panneau d'édition des paramètres.
	 */
	virtual QtRepresentationTypesPanel& getRepresentationTypesPanel ( );

	/**
	 * \return		La fenêtre principale de l'application.
	 */
	virtual Mgx3D::QtComponents::QtMgx3DMainWindow& getMainWindow ( );

	/**
	 * \return		Le gestionnaire de rendu utilisé.
	 */
	virtual Mgx3D::QtComponents::RenderingManager& getRenderingManager ( );


	protected slots :

	/**
	 * Actualise les entités conformément au paramètres du panneau.
	 */
	virtual void applyCallback ( );


	private :

	// Opérations interdites :
	QtRepresentationTypesDialog (const QtRepresentationTypesDialog&);
	QtRepresentationTypesDialog& operator = (const QtRepresentationTypesDialog&);

	/** La fenêtre principale de l'application, utilisée pour forcer les mises
	 * à jour graphiques. */
	QtMgx3DMainWindow*					_mainWindow;

	/** Le panneau permettant à l'utilisateur d'effectuer le choix des types de
	 * représentation. */
	QtRepresentationTypesPanel*			_representationTypesPanel;

	/** La fermeture de la boite de dialogue. */
	QtDlgClosurePanel*					_closurePanel;

	/** Les entités soumises aux modifications de propriétés graphiques. */
	std::vector<Mgx3D::Utils::Entity*>	_entities;

	/** Les masques d'affichage utilisé par les différents types d'entité. */
	std::map<Mgx3D::Utils::Entity::objectType, unsigned long>	_globalMasks;
};	// class QtRepresentationTypesDialog


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_REPRESENTATION_TYPES_DIALOG_H
