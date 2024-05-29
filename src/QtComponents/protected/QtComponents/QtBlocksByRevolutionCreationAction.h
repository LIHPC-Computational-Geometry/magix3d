/**
 * \file		QtBlocksByRevolutionCreationAction.h
 * \author		Charles PIGNEROL
 * \date		28/10/2012
 */
#ifndef QT_BLOCKS_BY_REVOLUTION_CREATION_ACTION_H
#define QT_BLOCKS_BY_REVOLUTION_CREATION_ACTION_H


#include "QtComponents/QtAnglePanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DEdgePanel.h"



namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau de création de bloc topologique par révolution d'arêtes.
 */
class QtBlocksByRevolutionCreationPanel : public QtMgx3DOperationPanel
{
	public :

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtBlocksByRevolutionCreationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtBlocksByRevolutionCreationPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return		Les arêtes à partir desquelles est effectuée la création de
	 * 				bloc par rotation.
	 */
	virtual std::vector<std::string> getEdges ( ) const;

	/**
	 * La portion de cylindre.
	 */
//	virtual unsigned short getCylinderFraction ( ) const;
	Utils::Portion::Type getPortion ( ) const;
	virtual double getAngle ( ) const;
	virtual double getOGridRatio ( ) const;

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
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * Invoque <I>preview (false, true).</I>
	 * \see		preview
	 */
	virtual void operationCompleted ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtBlocksByRevolutionCreationPanel (
									const QtBlocksByRevolutionCreationPanel&);
	QtBlocksByRevolutionCreationPanel& operator = (
									const QtBlocksByRevolutionCreationPanel&);

	/** Les arêtes utilisées pour construire le bloc. */
	QtMgx3DEdgePanel*				_edgePanel;

	/** La portion de cylindre à créer/modifier. */
	QtAnglePanel*					_anglePanel;

	/** Le ratio de positionnement de l'o-grid. */
	QtDoubleTextField*			    _oGridRatioTextField;

};	// class QtBlocksByRevolutionCreationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtBlocksByRevolutionCreationPanel</I> de création d'un bloc par rotation
 * d'arêtes.
 */
class QtBlocksByRevolutionCreationAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtBlocksByRevolutionCreationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtBlocksByRevolutionCreationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtBlocksByRevolutionCreationAction ( );

	/**
	 * \return		Le panneau d'édition du bloc associé.
	 */
	virtual QtBlocksByRevolutionCreationPanel* getBlocksByRevolutionPanel ( );

	/**
	 * Créé/modifie le bloc avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DtopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtBlocksByRevolutionCreationAction (
								const QtBlocksByRevolutionCreationAction&);
	QtBlocksByRevolutionCreationAction& operator = (
								const QtBlocksByRevolutionCreationAction&);
};  // class QtBlocksByRevolutionCreationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_BLOCKS_BY_REVOLUTION_CREATION_ACTION_H
