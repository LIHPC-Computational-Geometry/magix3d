/**
 * \file		QtMeshSubVolumeBetweenSheetsAction.h
 * \author		Charles PIGNEROL
 * \date		07/12/2016
 */
#ifndef QT_MESH_SUB_VOLUME_BETWEEN_SHEETS_ACTION_H
#define QT_MESH_SUB_VOLUME_BETWEEN_SHEETS_ACTION_H


#include "QtComponents/QtMgx3DMeshOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DEntityPanel.h"

#include <QtUtil/QtIntTextField.h>

#include <QVBoxLayout>

#include <memory>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération d'extraction d'un
 * sous-volume de mailles délimité par 2 feuillets.
 */
class QtMeshSubVolumeBetweenSheetsPanel : public QtMgx3DOperationPanel
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
	QtMeshSubVolumeBetweenSheetsPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action,
			const std::string& helpURL, const std::string& helpTag);

	/**
	 * RAS.
	 */
	virtual ~QtMeshSubVolumeBetweenSheetsPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * Méthode appelée pour vérifier les paramètres saisis par l'utilisateur.
	 * En cas de paramètrage invalide cette méthode doit leve une exception de
	 * type <I>TkUtil::Exception</I> contenant un descriptif des erreurs
	 * rencontrées.
	 */
	virtual void validate ( );

    /**
     * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
     * Restitue l'environnement dans son état initial.
	 * \see		autoUpdate
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );

	/**
	 * \return	Le nom du sous-volume créé.
	 */
	virtual std::string getName ( ) const;

	/**
	 * \return	La liste des blocks à mailler (dans l'hypothèse où l'on
	 *			ne maille qu'une sélection).
	 * \see		getEdge
	 */
	virtual std::vector<std::string> getBlocks ( ) const;

	/**
	 * \return	L'arête orthogonale aux feuillets
	 * \see		getBlocks
	 */
	virtual std::string getEdge ( ) const;

	/**
	 * \return	L'index du premier feuillet à prendre en compte.
	 * \see		to
	 */
	virtual int from ( ) const;

	/**
	 * \return	L'index du dernier feuillet à prendre en compte.
	 * \see		from
	 */
	virtual int to ( ) const;

	/**
	 * Arrête la sélection en mode interactif.
	 */
	virtual void stopInteractiveMode ( );


	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * Invoque <I>preview (false, true).</I>
	 * \see		preview
	 */
	virtual void operationCompleted ( );


	protected :

	/**
	 * \return		Le gestionnaire de mise en forme du panneau.
	 */
	virtual QVBoxLayout& getVLayout ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtMeshSubVolumeBetweenSheetsPanel (
									const QtMeshSubVolumeBetweenSheetsPanel&);
	QtMeshSubVolumeBetweenSheetsPanel& operator = (
									const QtMeshSubVolumeBetweenSheetsPanel&);

	/** Le nom du sous-volume. */
	QtTextField*						_nameTextField;

	/** Les blocs sièges de l'extraction. */
	QtMgx3DEntityPanel*					_blocksPanel;

	/** L'arête orthogonale aux feuillets. */
	QtMgx3DEntityPanel*					_edgePanel;

	/** Les indices des feuillets délimitant le sous-volume extrait. */
	QtIntTextField*						_fromTextField;
	QtIntTextField*						_toTextField;

	/** Le gestionnaire de mise en forme du panneau. */
	QVBoxLayout*						_verticalLayout;
};	// class QtMeshSubVolumeBetweenSheetsPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtMeshSubVolumeBetweenSheetsPanel</I> d'extraction de sous-volume délimité
 * par 2 feuillets.
 */
class QtMeshSubVolumeBetweenSheetsAction : public QtMgx3DMeshOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtMeshSubVolumeBetweenSheetsPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et la barre d'icônes
	 *				d'opérations Magix 3D.
	 * \param		Tooltip décrivant l'action.
	 */
	QtMeshSubVolumeBetweenSheetsAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMeshSubVolumeBetweenSheetsAction ( );

	/**
	 * \return		Le panneau d'édition de la génération de maillage.
	 */
	virtual QtMeshSubVolumeBetweenSheetsPanel* getMeshPanel ( );

	/**
	 * Créé/modifie les maillages des blocs topologiques.
	 * Invoque préalablement
	 * <I>QtMgx3DMeshOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	protected :


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMeshSubVolumeBetweenSheetsAction (
									const QtMeshSubVolumeBetweenSheetsAction&);
	QtMeshSubVolumeBetweenSheetsAction& operator = (
									const QtMeshSubVolumeBetweenSheetsAction&);
};  // class QtMeshSubVolumeBetweenSheetsAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MESH_SUB_VOLUME_BETWEEN_SHEETS_ACTION_H
