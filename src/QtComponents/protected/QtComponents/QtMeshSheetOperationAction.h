/**
 * \file		QtMeshSheetOperationAction.h
 * \author		Charles PIGNEROL
 * \date		30/01/2013
 */
#ifndef QT_MESH_SHEET_OPERATION_ACTION_H
#define QT_MESH_SHEET_OPERATION_ACTION_H


#include "QtComponents/QtMgx3DMeshOperationAction.h"
#include "QtComponents/QtMgx3DEdgePanel.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "Mesh/CommandMeshExplorer.h"

#include <QtUtil/QtTextField.h>

#include <QVBoxLayout>

#include <memory>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération d'extraction de feuillet de
 * maillage. Le feuillet est défini comme étant perpendiculaire à une arête
 * topologique donnée.
 */
class QtMeshSheetOperationPanel : public QtMgx3DOperationPanel
{
	public :

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Politique de création/modification du groupe
	 * \param	Arête topologique proposée par défaut.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 * \param	Fichier d'aide contextuelle associé à ce panneau.
	 * \param	Balise dans le fichier d'aide contextuelle associé à ce panneau
	 *			identifiant le paragraphe sur ce panneau.
	 */
	QtMeshSheetOperationPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			const std::string& edgeUID,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action,
			const std::string& helpURL, const std::string& helpTag);

	/**
	 * RAS.
	 */
	virtual ~QtMeshSheetOperationPanel ( );

	/**
	 * \return		Le nom du groupe créé/modifié.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		L'arête définissant le feuillet actuel.
	 */
	virtual std::string getEdgeUniqueName ( ) const;

	/**
	 * \param		L'arête définissant le feuillet actuel.
	 */
	virtual void setEdgeUniqueName (const std::string& name);

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


	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return		Le gestionnaire de mise en forme du panneau.
	 */
	virtual QVBoxLayout& getVLayout ( );

	/**
	 * Exécute l'action associée puis invoque <I>operationCompleted</I>.
	 * \see		operationCompleted
	 */
	virtual void applyCallback ( );



private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtMeshSheetOperationPanel (const QtMeshSheetOperationPanel&);
	QtMeshSheetOperationPanel& operator = (
										const QtMeshSheetOperationPanel&);

	/** Le nom du feuillet. */
	QtMgx3DGroupNamePanel*				_namePanel;

	/** L'identifiant unique de l'arête définissant le feuillet. */
	QtMgx3DEdgePanel*					_edgePanel;

	/** Le gestionnaire de mise en forme du panneau. */
	QVBoxLayout*						_verticalLayout;
};	// class QtMeshSheetOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtMeshSheetOperationPanel</I> de contrôle d'une exploration de
 * maillage type "feuillet".
 */
class QtMeshSheetOperationAction : public QtMgx3DMeshOperationAction
{
	public :

	/**
	 * Constructeur 1 : commande Magix 3D, avec panneau intégré aux barres
	 * d'opérations.
	 * Créé et s'associe une instance de la classe
	 * <I>QtMeshSheetOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et la barre d'icônes
	 *				d'opérations Magix 3D.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 * \param		<I>true<I> s'il s'agit d'une commande <I>Magix 3D</I>,
	 *				à savoir soumis au gestionnaire de <I>undo/redo</I>,
	 *				scriptable, ..., <I>false</I> dans le cas contraire (par
	 *				exemple dans le cadre d'une exploration de feuillets).
	 * \param		<I>true<I> si l'instance doit créer son panneau de
	 *				paramétrage associé, <I>false</I> dans le cas contraire
	 *				(cas par exemple de l'explorateur de feuillets dérivé de
	 *				cette classe).
	 */
	QtMeshSheetOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy,
		bool isCommand = true, bool createPanel = true);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMeshSheetOperationAction ( );

	/**
	 * \return		Le panneau d'édition de l'explorateur de maillage associé.
	 */
	virtual QtMeshSheetOperationPanel* getMeshSheetPanel ( );

	/**
	 * Créé/modifie le feuillet avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DMeshOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );

	/**
	 * \return		<I>true</I> si l'opération est une commande <I>Magix 3D</I>,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool isCommand ( ) const;


	protected :

	/**
	 * \return		Un pointeur sur l'éventuelle instance de
	 *				<I>CommandMeshExplorer</I> en cours.
	 * \see			releaseMeshExplorer
	 * \see			setMeshExplorer
	 */
	virtual Mgx3D::Mesh::CommandMeshExplorer* getMeshExplorer ( );

	/**
	 * \return		Un pointeur sur l'éventuelle instance de
	 *				<I>CommandMeshExplorer</I> en cours, à adopter.
	 * \see			getMeshExplorer
	 * \see			setMeshExplorer
	 */
	virtual Mgx3D::Mesh::CommandMeshExplorer* releaseMeshExplorer ( );

	/**
	 * \param		Nouveau pointeur sur une instance de
	 *				<I>CommandMeshExplorer</I> à prendre en charge, ou 0.
	 *				Adopte cette instance, l'éventuelle instance en cours
	 *				étant détruite.
	 * \see			releaseMeshExplorer
	 */
	virtual void setMeshExplorer (Mgx3D::Mesh::CommandMeshExplorer*);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMeshSheetOperationAction (const QtMeshSheetOperationAction&);
	QtMeshSheetOperationAction& operator = (
									const QtMeshSheetOperationAction&);

	/** L'éventuel explorateur de feuillet créé. */
	std::unique_ptr<Mgx3D::Mesh::CommandMeshExplorer>		_meshExplorer;

	/** <I>true</I> si l'opération est une commande <I>Magix 3D</I>. */
	bool												_isCommand;
};  // class QtMeshSheetOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MESH_SHEET_OPERATION_ACTION_H
