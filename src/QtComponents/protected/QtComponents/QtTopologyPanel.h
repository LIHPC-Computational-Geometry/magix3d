/**
 * \file		QtTopologyPanel.h
 * \author		Charles PIGNEROL
 * \date		18/12/2012
 */
#ifndef QT_TOPOLOGY_PANEL_H
#define QT_TOPOLOGY_PANEL_H


#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DGroupNamePanel.h"

#include "Utils/ValidatedField.h"

#include <QtUtil/QtIntTextField.h>
#include <QtUtil/QtDoubleTextField.h>

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

#include <vector>


namespace Mgx3D 
{

namespace QtComponents
{

class QtMgx3DMainWindow;

/**
 * Classe permettant la saisie d'une topologie associée à une géométrie et 
 * offrant une <I>API</I> de validation <I>ValidatedField</I>.
 * \see		ValidatedField
 */
class QtTopologyPanel :
			public QWidget, public Mgx3D::Utils::ValidatedField
{
	Q_OBJECT

	public :

	/**
	 * Le type de topologie représentée :
	 * <OL>
	 * <LI><I>STRUCTURED_TOPOLOGY</I> : bloc ou face unique structuré,
	 * <LI><I>OGRID_BLOCKS</I></LI> : ensemble de blocs organisés en o-grid.
	 * <LI><I>STRUCTURED_FREE_TOPOLOGY</I> : bloc ou face structuré sans
	 * association à une géométrie, mais rattachement possible à un groupe,
	 * <LI><I>UNSTRUCTURED_TOPOLOGY</I> : bloc ou face unique non structuré,
	 * <LI><I>INSERTION_TOPOLOGY</I> : bloc pour l'insertion,
	 * </OL>
	 */
	enum TOPOLOGY_TYPE { STRUCTURED_TOPOLOGY, OGRID_BLOCKS,
		                 STRUCTURED_FREE_TOPOLOGY, UNSTRUCTURED_TOPOLOGY,
	                     INSERTION_TOPOLOGY};

	/**
	 * Le type de système de coordonnées.
	 */
	// ATTENTION : penser à adapter coordsNames et coordsTips dans le .cpp
	enum COORDINATES_TYPE { NO_COORDINATES, CARTESIAN, CYLINDRIC, POLAR, OGRID };

	/**
	 * \param	Type de topologie dont on souhaite obtenir le nom.
	 * \param	Utiliser des libellés 3D (<I>true</I>, => bloc) ou 2D
	 *			(<I>false</I> => face).
	 * \return		Le nom de la topologie transmise en argument.
	 */
	static std::string typeToName (TOPOLOGY_TYPE type, bool use3DLabels);

	/**
	 * \param	Widget parent.
	 * \param	Fenêtre principale de rattachement (pour accéder au contexte,
	 *			...)
	 * \param	Par défaut, faut-il créer une topologie ?
	 * \param	Le choix d'un groupe de rattachement est-il possible ?
	 * \param	Dimension de l'éventuel group de rattachement.
	 * \param	Type de topologie proposée par défaut.
	 * \param	Type de système de coordonnées.
	 * \param	Valeur par défaut des champs axe 1, axe 2, axe 3.
	 *			Si < 0 le champ n'est pas instancié.
	 * \param	Utiliser des libellés 3D (<I>true</I>, => bloc) ou 2D
	 *			(<I>false</I> => face).
	 *			<I>true</I. si l'organisation des champs de texte doit être
	 *			verticale, <I>false</I> si elle doit être horizontale.
	 * \see		enableTopologyType
	 */
	QtTopologyPanel (QWidget* parent, QtMgx3DMainWindow& mainWindow,
	                bool createTopology, bool allowGroup, int groupDimension,
					TOPOLOGY_TYPE type, COORDINATES_TYPE coordsType,
	                int axe1Default, int axe2Default, int axe3Default,
	                bool use3DLabels, bool verticalLayout = false);

	/**
	 * RAS.
	 */
	virtual ~QtTopologyPanel ( );

	/**
	 * \return		<I>true</I> s'il faut créer une topologie, <I>false</I> dans
	 *				le cas contraire.
	 * \see			getTopologyType
	 * \see			displayTopologyCreation
	 */
	virtual bool createTopology ( ) const;

	/**
	 * \param		<I>true</I> s'il faut créer une topologie, <I>false</I> dans
	 *				le cas contraire.
	 * \see			getTopologyType
	 * \see			displayTopologyCreation
	 */
	virtual void createTopology (bool create);

	/**
	 * \return		Le nom de l'éventuel groupe de rattachement de l'entité
	 * 				topologique.
	 * \see			enableGroupName
	 * \exception	Lève une exception si le rattachement à un groupe n'est pas
	 * 				possible (cf. constructeur).
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \param		<I>true</I> si le rattachement à un groupe doit être
	 * 				autorisé, <I>false</I> dans le cas contraire.
	 * \see			getGroupName
	 */
	virtual void allowGroupName (bool allow);

	/**
	 * \param		<I>true</I> si la case à cocher <I>Créer une topologie</I>
	 * 				doit être affichée, <I>False</I> dans le cas contraire.
	 * \see			createTopology
	 */
	virtual void displayTopologyCreation (bool enable);

	/**
	 * \return		Le type de topologie à créer.
	 * \see			validate
	 * \see			setTopologyType
	 */
	virtual TOPOLOGY_TYPE getTopologyType ( ) const;

	/**
	 * \param		Le type de topologie à créer.
	 * \see			validate
	 * \see			setTopologyType
	 * \see			enableTopologyType
	 */
	virtual void setTopologyType (TOPOLOGY_TYPE type);

	/**
	 * Autorise (<I>true</I>)/interdit (<I>false</I>) la saisie de la
	 * topologie transmise en premier argument.
	 */
	virtual void enableTopologyType (TOPOLOGY_TYPE type, bool enable);

	/**
	 * \return		Le type de topologie proposé par défaut.
	 * \see			getTopologyType
	 * \see			setTopologyType
	 */
	virtual TOPOLOGY_TYPE getDefaultTopologyType ( ) const;

	/**
	 * \return		Le type de système de coordonnées.
	 */
	virtual COORDINATES_TYPE getCoordinatesType ( ) const;

	/**
	 * \return		Le ratio de l'éventuel O-grid.
	 * \see			setOGridRatio
	 * \see			getTopologyType
	 * \see			enableOGridRatio
	 */
	virtual double getOGridRatio ( ) const;

	/**
	 * \param		Le ratio de l'éventuel O-grid.
	 * \see			getOGridRatio
	 * \see			setOGridRatio
	 * \see			enableOGridRatio
	 */
	virtual void setOGridRatio (double ratio);

	/**
	 * (In)Active la saisie du ratio 0-grid.
	 */
	virtual void enableOGridRatio (bool enable);

	/**
	 * \return		Le nombre de bras suivant le premier axe.
	 * \see			setAxe1EdgesNum
	 * \exception	Une exception est levée si ce champ n'est pas instancié
	 *				(cf. constructeur).
	 */
	virtual int getAxe1EdgesNum ( ) const;

	/**
	 * \param		Le nombre de bras suivant le second axe.
	 * \see			getAxe1EdgesNum
	 * \exception	Une exception est levée si ce champ n'est pas instancié
	 *				(cf. constructeur).
	 */
	virtual void setAxe1EdgesNum (int edgesNum);

	/**
	 * \return	  	Le nombre de bras selon le 2ème axe.
	 * \see			setAxe2EdgesNum
	 * \exception	Une exception est levée si ce champ n'est pas instancié
	 *				(cf. constructeur).
	 */
	virtual int getAxe2EdgesNum ( ) const;

	/**
	 * \param		Le nombre de bras suivant le 2ème axe.
	 * \see			getAxe2EdgesNum
	 * \exception	Une exception est levée si ce champ n'est pas instancié
	 *				(cf. constructeur).
	 */
	virtual void setAxe2EdgesNum (int edgesNum);

	/**
	 * \return	  	Le nombre de bras selon le 3ème axe.
	 * \see			setAxe3EdgesNum
	 * \exception	Une exception est levée si ce champ n'est pas instancié
	 *				(cf. constructeur).
	 */
	virtual int getAxe3EdgesNum ( ) const;

	/**
	 * \param		Le nombre de bras suivant le 3ème axe.
	 * \see			getAxe3EdgesNum
	 * \exception	Une exception est levée si ce champ n'est pas instancié
	 *				(cf. constructeur).
	 */
	virtual void setAxe3EdgesNum (int edgesNum);

	/**
	 * Lève une exception de type <I>TkUtil::Exception</I> en cas d'incohérence
	 * dans les données saisies par l'utilisateur.
	 * \see			getTopologyType
	 */
	virtual void validate ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 */
	virtual void autoUpdate ( );


	signals :

	/**
	 * Signal émis lorsque le fait de créer ou non une topologie associée est
	 * modifié.
	 */
	void topologyCreationModified ( );

	/**
	 * Signal émis lorsque le type de topologie est modifié.
	 */
	void topologyTypeModified ( );


	protected slots :

	/**
	 * Callback sur "créer une topologie associée". Actualise le panneau
	 * (selon le type de topologie certains champs de saisie seront
	 * fonctionnels, d'autres non).
	 * \see		updatePanel
	 */
	virtual void createTopologyCallback (int);

	/**
	 * Callback sur le type de topologie. Actualise le panneau.
	 * \see		updatePanel
	 */
	virtual void topologyTypeCallback (int);


	protected :

	/**
	 * Actualise le panneau selon les choix de l'utilisateur (création de
	 * topologie, nature de la topologie).
	 */
	virtual void updatePanel ( );


	private :

	/**
	 * Constructeur de copie/opérateur = : Opérations interdites :
	 */
	QtTopologyPanel (const QtTopologyPanel&);
	QtTopologyPanel& operator = (const QtTopologyPanel&);

	/** Faut-il créer une topologie ? */
	QCheckBox*						_createTopologyCheckBox;

	/** L'éventuel groupe de rattachement. */
	QtMgx3DGroupNamePanel*			_groupNamePanel;

	/** Le widget de paramétrage du type de topologie. */
	QComboBox*						_topologyComboBox;

	/** Le type de topologie utilisée. */
	TOPOLOGY_TYPE					_topologyType;

	/** Le type de système de coordonnées. */
	COORDINATES_TYPE				_coordinatesType;

	/** Par défaut, faut-il créer une topologie ? */
	bool							_defaultCreateTopology;

	/** Le type de topologie utilisée par défaut. */
	TOPOLOGY_TYPE					_defaultTopologyType;

	/** Le ratio de l'éventuel O-grid. */
	QLabel*							_oGridLabel;
	QtDoubleTextField*				_oGridRatioTextField;

	/** Les nombres de bras (naxe, ni, nr). */
	QLabel							*_axe1NumLabel,
									*_axe2NumLabel,
									*_axe3NumLabel;
	QtIntTextField					*_axe1NumTextField,
									*_axe2NumTextField,
									*_axe3NumTextField;

	/** Le ratio par défaut de l'éventuel O-grid. */
	double							_defaultOGridRatio;

	/** Les valeurs par défaut des champs "nombres d'arêtes". */
	int								_defaultAxe1Num, _defaultAxe2Num,
									_defaultAxe3Num;

	/** <I>true</I> s'il faut utiliser des libellés 3D (bloc), false dans le cas
	 * contraire (face). */
	bool							_use3DLabels;
};	// class QtTopologyPanel


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_PANEL_H
