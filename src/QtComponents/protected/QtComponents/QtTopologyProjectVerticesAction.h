/**
 * \file		QtTopologyProjectVerticesAction.h
 * \author		Charles PIGNEROL
 * \date		25/03/2014
 */
#ifndef QT_TOPOLOGY_PROJECT_VERTICES_ACTION_H
#define QT_TOPOLOGY_PROJECT_VERTICES_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DOperationsPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/QtEntityIDTextField.h"

#include <QtUtil/QtDoubleTextField.h>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Panneau d'édition des paramètres de projection de sommets topologiques
 * sur un plan.
 */
class QtTopologyProjectVerticesPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	enum coordinateType {cartesian, spherical, cylindrical};

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtTopologyProjectVerticesPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyProjectVerticesPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
	 * Restitue l'environnement dans son état initial.
	 * Invoque <I>preview (false)</I>.
	 * \see	preview
	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true, true)</I>.
	 * \see	preview
	 */
	virtual void autoUpdate ( );

	/**
	 * \return		Les noms des sommets à projeter.
	 */
	virtual std::vector<std::string> getVerticesNames ( ) const;

	/**
	 * \return le nom du repère (peut être vide)
	 */
	virtual std::string getSysCoordName() const;

	/**
	 * \return		Les nouvelles composantes des sommets.
	 * \see			projectX
	 * \see			projectY
	 * \see			projectZ
	 */
	virtual double getX ( ) const;
	virtual double getY ( ) const;
	virtual double getZ ( ) const;

	/**
	 * \return		Faut-il projeter les composantes X, Y et Z ?
	 * \see			getX
	 * \see			getY
	 * \see			getZ
	 */
	virtual bool projectX ( ) const;
	virtual bool projectY ( ) const;
	virtual bool projectZ ( ) const;


	/**
	 * Retourne le type de coordonnées utilisées
	 */
	virtual coordinateType getTypeCoordinate ();

	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );

	/**
	 * Actualise les coordonnées affichées selon le point sélectionné.
	 * \param		Nom du point sélectionné.
	 */
	virtual void updateCoordinates (const std::string& name);


	signals :

	/**
	 * Signal émis lorsque l'utilisateur vient de finir une action de
	 * modification du point.
	 */
	void pointModified ( );

	protected slots :

	/**
	 * Callback appelé lorsque l'utilisateur sélectionne de manière interactive
	 * un nouveau point.
	 * Emet le signal <I>pointModified</I>
	 */
	virtual void selectedVertexCallback (const QString name);

	/**
	 * Callback appelé lors du changement de type de coordonnées
	 */
	virtual void coordTypeChangedCallback (int index);

	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtTopologyProjectVerticesPanel (const QtTopologyProjectVerticesPanel&);
	QtTopologyProjectVerticesPanel& operator = (
										const QtTopologyProjectVerticesPanel&);

	/** Les sommets à projeter. */
	QtMgx3DEntityPanel*			_verticesPanel;

	/** Les noms des types des composantes. */
	QLabel			*_xTextLabel, *_yTextLabel, *_zTextLabel;

	/** Les nouvelles composantes des coordonnées des sommets. */
	QtDoubleTextField			*_xTextField, *_yTextField, *_zTextField;

	/** Faut-il modifier les composantes des coordonnées des sommets ? */
	QCheckBox					*_xCheckBox, *_yCheckBox, *_zCheckBox;

	/** L'éventuel vertex dont on copie les coordonnées. */
	Mgx3D::QtComponents::QtEntityIDTextField*	_vertexIDTextField;

	/** ComboBox / type de coordonnées: cartésiennes, sphérique ou cylindrique */
	QComboBox* _coordTypeComboBox;

	/** repère local ou non */
	QtMgx3DEntityPanel*   _sysCoordPanel;

};	// class QtTopologyProjectVerticesPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtTopologyProjectVerticesPanel</I> de projection de sommets topologiques
 * sur un plan.
 */
class QtTopologyProjectVerticesAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopologyProjectVerticesPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopologyProjectVerticesAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopologyProjectVerticesAction ( );

	/**
	 * \return		Le panneau de paramétrage de la projection.
	 */
	virtual QtTopologyProjectVerticesPanel* getTopologyProjectVerticesPanel ( );

	/**
	 * Projete les sommets topologiques conformément au paramétrage de
	 * son panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopologyProjectVerticesAction (const QtTopologyProjectVerticesAction&);
	QtTopologyProjectVerticesAction& operator = (
									const QtTopologyProjectVerticesAction&);
};  // class QtTopologyCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPOLOGY_PROJECT_VERTICES_ACTION_H
