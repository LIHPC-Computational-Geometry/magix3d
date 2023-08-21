/**
 * \file		QtRepresentationTypesPanel.h
 * \author		Charles PIGNEROL
 * \date		17/01/2011
 */
#ifndef QT_REPRESENTATION_TYPES_PANEL_H
#define QT_REPRESENTATION_TYPES_PANEL_H


#include <QtComponents/QtMgx3DGroupBox.h>
#include "Utils/Entity.h"
#include "Utils/DisplayProperties.h"

#include <TkUtil/ThreeStates.h>
#include <QtUtil/QtGroupBox.h>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

#include <TkUtil/PaintAttributes.h>
#include <QtUtil/QtDoubleTextField.h>

#include <set>


namespace Mgx3D 
{

namespace QtComponents
{

class QtColorationWidget;
class QtOpacityWidget;


/**
 * Panneau pemettant la sélection de types de représentation à utiliser pour
 * afficher des entités 3D : points, filaire, ... Ces types de représentation
 * sont ceux définis dans la classe <I>GraphicalEntityRepresentation<I> :
 * <I>CLOUDS<I>, <I>CURVES</I>, ... Pour chaque type de représentation 3 
 * choix sont possibles : <I>ON<I> (utilisation), <I>OFF</I> (non utilisation),
 * <I>UNDETERMINED</I> (indéterminé, valeur utilisée par exemple lorsque
 * initialement certaines entités utilisent un type de représentation et pas
 * d'autres).
 */
class QtRepresentationTypesPanel : public QtMgx3DGroupBox
{
	Q_OBJECT

	public :

	/**
	 * Les types de valeurs affichables : aucun (représentation pleine et non
	 * colorée des mailles), valeurs aux noeuds, valeurs aux mailles.
	 */
	enum DISPLAYED_VALUE { NO_VALUE, NODE_VALUE, CELL_VALUE };

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	<I>true</I> si on est en mode d'utilisation des propriétés
	 * 			globales d'affichage, <I>false</I> dans le cas contraire.
	 * \param	Valeur initiale pour la représentation de type <I>points</I>.
	 * \param	Valeur initiale pour la représentation de type <I>filaire</I>.
	 * \param	Valeur initiale pour la représentation de type <I>complément filaire</I>.
	 * \param	Valeur initiale pour la représentation de type <I>surfaces</I>.
     * \param   Valeur initiale pour la représentation de type <I>volumes</I>.
     * \param   Valeur initiale pour la représentation de type <I>noms</I>.
	 * \param	Valeur initiale pour la représentation de type <I>associations</I>.
	 * \param	Valeur initiale pour la couleur utilisée pour les entités
	 *			topologiques (standard/entité géométrique associée).
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
	 */
	QtRepresentationTypesPanel (QWidget* parent, const std::string& name, bool globalMode,
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
			bool displayValue, DISPLAYED_VALUE displayedValue,
			const std::set< std::string >& nodesValuesNames,
			const std::set< std::string >& cellsValuesNames,
			const std::string& displayedValueName
		);

	/**
	 * RAS.
	 */
	virtual ~QtRepresentationTypesPanel ( );

	/**
	 * Classe "étendue" sur les propriétés d'affichage d'entités. L'extension
	 * porte sur l'utilisation ou non de certains attributs.
	 */
	class ExtendedDisplayProperties : public Mgx3D::Utils::DisplayProperties
	{
		public :

		/**
		 * Constructeurs, destructeur : RAS. "Tout est utilisé".
		 */
		ExtendedDisplayProperties ( );
		ExtendedDisplayProperties (const Mgx3D::Utils::DisplayProperties& dp);
		ExtendedDisplayProperties (const ExtendedDisplayProperties&);
		ExtendedDisplayProperties& operator =(const ExtendedDisplayProperties&);
		virtual ~ExtendedDisplayProperties ( );

		virtual bool useCloudColor ( ) const;
		virtual void useCloudColor (bool use);
		virtual bool useWireColor ( ) const;
		virtual void useWireColor (bool use);
		virtual bool useSurfacicColor ( ) const;
		virtual void useSurfacicColor (bool use);
		virtual bool useVolumicColor ( ) const;
		virtual void useVolumicColor (bool use);
		virtual bool useCloudOpacity ( ) const;
		virtual void useCloudOpacity (bool use);
		virtual bool useWireOpacity ( ) const;
		virtual void useWireOpacity (bool use);
		virtual bool useSurfacicOpacity ( ) const;
		virtual void useSurfacicOpacity (bool use);
		virtual bool useVolumicOpacity ( ) const;
		virtual void useVolumicOpacity (bool use);
		virtual bool useShrinkFactor ( ) const;
		virtual void useShrinkFactor (bool use);
		virtual bool useArrowComul ( ) const;
		virtual void useArrowComul (bool use);

		/**
		 * Actualise la propriété transmise en dernier argument à partir de
		 * celle transmise en premier argument. Les attributs modifiés dépendent
		 * du type d'entité associé et du mode d'affichage (propriété
		 * globale/individuelle).
		 */	
		static void updateDisplayProperties (
			const ExtendedDisplayProperties& p1, Utils::Entity::objectType type,
			bool globalMode, Utils::DisplayProperties& p2);


		private :

		bool			_useCloudColor, _useWireColor,
						_useSurfacicColor, _useVolumicColor;
		bool			_useCloudOpacity, _useWireOpacity,
						_useSurfacicOpacity, _useVolumicOpacity;
		bool			_useShrinkFactor, _useArrowComul;
	};	// class ExtendedDisplayProperties

	/**
	 * \return		<I>true</I> si une couleur a été modifiée.
	 */
	virtual bool colorModified ( ) const;

	/**
	 * \return		Les propriétés d'affichage définies par cette boite
	 * 				de dialogue.
	 */
	virtual ExtendedDisplayProperties getDisplayProperties ( ) const;

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
	 * \return      Le choix de l'utilisateur quant à l'utilisation d'une
	 *              représentation de type <I>noms</I>.
	 */
	virtual TkUtil::THREESTATES useNamesRepresentation ( ) const;

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
	 * \return		Les propriétés de représentation textuelle.
	 * \warning		<B>Un panneau de saisie de la fonte doit être associé
	 * 				à l'instance.</B>
	 * \see			setFontPropertiesPanel
	 */
	virtual void getFontProperties (
					int& family, int& size,
					TkUtil::THREESTATES& bold, TkUtil::THREESTATES& italic,
					TkUtil::Color& color, bool& colorModified) const;

	/**
	 * \return		Quel type de valeur faut-il éventuellement représenter ?
	 * \see			getDisplayedValueName
	 */
	virtual DISPLAYED_VALUE getDisplayedValueType ( ) const;

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
	 * \return		<I>true</I> s'il faut prendre le facteur d'agrandissement 
	 *			en compte, <I>false</I> dans le cas contraire.
	 */
	virtual bool useArrowComul ( ) const;

	/**
	 * \return		Le facteur d'agrandissement de la flèche par rapport
	 *			à la valeur par défaut.
	 */
	virtual double getArrowComul ( ) const;

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
     * \return      Le choix de l'utilisateur quant à l'affichage
     *              d'une représentation du type de la discrétisation.
     */
    virtual TkUtil::THREESTATES useDiscretisationRepresentation ( ) const;

    /**
      * \return      Le choix de l'utilisateur quant à l'affichage
      *              du nombre de bras de la discrétisation.
      */
    virtual TkUtil::THREESTATES useNbMeshRepresentation ( ) const;


	protected slots :

	/**
	 * Appelé lorsque l'utilisateur change de type de valeur affichée.
	 * Actualise l'IHM.
	 */
	virtual void valueTypeCallback ( );


	public :

	/**
	 * <I>API</I> à respecter pour un panneau de choix de la fonte.
	 */
	class QtAbstractFontPropertiesPanel : public QWidget
	{
		public :

		virtual ~QtAbstractFontPropertiesPanel ( );

		virtual void getFontProperties (
					int& family, int& size,
					TkUtil::THREESTATES& bold, TkUtil::THREESTATES& italic,
					TkUtil::Color& color, bool& colorModified) const;


		protected :

		/**
		 * Constructeur par défaut.
		 */
		QtAbstractFontPropertiesPanel (QWidget* parent);


		private :

		QtAbstractFontPropertiesPanel (const QtAbstractFontPropertiesPanel&);
		QtAbstractFontPropertiesPanel& operator = (const QtAbstractFontPropertiesPanel&);
	};	// class QtAbstractFontPropertiesPanel

	/**
	 * A utiliser par les classes dérivées.
	 */
	virtual void setFontPropertiesPanel (QtAbstractFontPropertiesPanel& panel);


	private :

	// Opérations interdites :
	QtRepresentationTypesPanel (const QtRepresentationTypesPanel&);
	QtRepresentationTypesPanel& operator = (const QtRepresentationTypesPanel&);

	/** Les valeurs affichables aux noeuds et aux mailles. */
	const std::set < std::string >	_nodesValuesNames, _cellsValuesNames;

	/** Les choix possibles pour l'utilisateur. */
	QCheckBox			*_repCloudCheckBox, *_repCurvesCheckBox,
					*_repIsoCurvesCheckBox, *_repSurfacesCheckBox,
					*_repVolumesCheckBox, *_repNamesCheckBox,
					*_repAssociationCheckBox, *_topoColorCheckBox,
					*_repMeshShapeCheckBox, *_repDiscretisationCheckBox,
					*_repNbMeshCheckBox;
	QtColorationWidget		*_cloudColorWidget, *_curvesColorWidget,
					*_isoCurvesColorWidget, *_surfacesColorWidget,
					*_volumesColorWidget;
	QPushButton			*_cloudColorButton, *_curvesColorButton,
					*_isoCurvesColorButton, *_surfacesColorButton,
					*_volumesColorButton;
	QtOpacityWidget			*_cloudOpacityTextField, *_curvesOpacityTextField,
					*_isoCurvesOpacityTextField, *_surfacesOpacityTextField,
					*_volumesOpacityTextField;
	QtOpacityWidget			*_shrinkFactorTextField;
	QtOpacityWidget			*_arrowComulTextField;
	QComboBox			*_valueTypeComboBox, *_valueNameComboBox;
	QtAbstractFontPropertiesPanel	*_fontPanel;
	QGridLayout			*_gridLayout;
};	// class QtRepresentationTypesPanel


/**
 * Classe à usage interne.
 */
class QtColorationWidget : public QWidget
{
	Q_OBJECT

	public :

	QtColorationWidget (QWidget* parent, const TkUtil::Color& bg, bool useColor);
	virtual ~QtColorationWidget ( );
	virtual bool isModified ( ) const;
	virtual void setBackground (const QColor& c);
	virtual TkUtil::Color getColor ( ) const;
	virtual bool useColor ( ) const;


	public slots :

	virtual void editColorCallback ( );


	private :

	QtColorationWidget (const QtColorationWidget&);
	QtColorationWidget& operator = (const QtColorationWidget&);
	const TkUtil::Color	_initialColor;
	bool				_useColor;
};	// class QtColorationWidget


/**
 * Pour des besoins internes.
 */
class QtOpacityWidget : public QtDoubleTextField
{
	public :

	QtOpacityWidget (QWidget* parent, double opacity, bool useOpacity);
	virtual ~QtOpacityWidget ( );
	virtual bool isModified ( ) const;
	virtual double getOpacity ( ) const;
	virtual bool useOpacity ( ) const;


	protected :

	virtual bool validate ( );


	private :

	QtOpacityWidget (const QtOpacityWidget&);
	QtOpacityWidget& operator = (const QtOpacityWidget&);
	const double		_initialOpacity;
	bool				_useOpacity;
};	// class QtOpacityWidget


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_REPRESENTATION_TYPES_PANEL_H
