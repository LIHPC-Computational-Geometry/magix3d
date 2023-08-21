/**
 * \file		QtSmoothSurfaceOperationAction.h
 * \author      Eric Brière de l'Isle
 * \date        4/4/2016
 */
#ifndef QT_SMOOTH_SURFACE_OPERATION_ACTION_H
#define QT_SMOOTH_SURFACE_OPERATION_ACTION_H

#include "Internal/ContextIfc.h"

#include "Smoothing/SurfacicSmoothing.h"

#include "QtComponents/QtMgx3DMeshOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DEntityPanel.h"

#include <QVBoxLayout>

#include <memory>

#include <QtUtil/QtIntTextField.h>
#include <QComboBox>

namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau pour ajouter une méthode de lissage à une surface de maillage.
 */
class QtSmoothSurfaceOperationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 * \param	Fichier d'aide contextuelle associé à ce panneau.
	 * \param	Balise dans le fichier d'aide contextuelle associé à ce panneau
	 *			identifiant le paragraphe sur ce panneau.
	 */
	QtSmoothSurfaceOperationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action,
			const std::string& helpURL, const std::string& helpTag);

	/**
	 * RAS.
	 */
	virtual ~QtSmoothSurfaceOperationPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );


	/**
	 * \return		Le nom du groupe concerné.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * Actualise le panneau en fonction du contexte.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );

	/**
	 * \return	<I>true</I> s'il faut utiliser les paramètres par défaut, <I>false</I> dans le
	 * 			cas contraire.
	 */
	virtual bool smoothDefault ( ) const;

	/// nombre d'itérations pour le lissage
	uint getNbIterations ( ) const;

	/// méthode de lissage
	Mesh::SurfacicSmoothing::eSurfacicMethod getMethod ( ) const;

	/// solveur
	Mesh::SurfacicSmoothing::eSolver getSolver ( ) const;

	protected slots :

	/**
	 * Callback sur le changement d'état du checkbox "Lissage avec options par défaut".
	 * Actualise le panneau.
	 */
	virtual void smoothDefaultCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtSmoothSurfaceOperationPanel (const QtSmoothSurfaceOperationPanel&);
	QtSmoothSurfaceOperationPanel& operator = (
										const QtSmoothSurfaceOperationPanel&);

	/** Le nom de la surface. */
	QtMgx3DGroupNamePanel*				_namePanel;

	/** Faut-il lisser avec les paramètres par défaut ? */
	QCheckBox*							_smoothDefaultCheckBox;

	/// nombre d'itérations
	QtIntTextField* 					_nbIterationsTextField;

	/// méthode de lissage
	QComboBox* 							_methodComboBox;

	/// Solveur
	QComboBox* 							_solverComboBox;

};	// class QtSmoothSurfaceOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtSmoothSurfaceOperationPanel</I> de contrôle d'une opération de maillage
 * des faces topologiques.
 */
class QtSmoothSurfaceOperationAction : public QtMgx3DMeshOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtSmoothSurfaceOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et la barre d'icônes
	 *				d'opérations Magix 3D.
	 * \param		Tooltip décrivant l'action.
	 */
	QtSmoothSurfaceOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtSmoothSurfaceOperationAction ( );

	/**
	 * \return		Le panneau d'édition du lissage de maillage.
	 */
	virtual QtSmoothSurfaceOperationPanel* getSmoothPanel ( );

	/**
	 * Créé/modifie les maillages des faces topologiques.
	 * Invoque préalablement
	 * <I>QtMgx3DMeshOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtSmoothSurfaceOperationAction (const QtSmoothSurfaceOperationAction&);
	QtSmoothSurfaceOperationAction& operator = (
									const QtSmoothSurfaceOperationAction&);
};  // class QtSmoothSurfaceOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_SMOOTH_SURFACE_OPERATION_ACTION_H
