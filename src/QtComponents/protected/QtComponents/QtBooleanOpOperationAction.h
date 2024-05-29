/*
 * QtBooleanOpOperationAction.h
 *
 *  Created on: 20 mars 2013
 *      Author: ledouxf
 */

#ifndef QtBooleanOpOPERATIONACTION_H_
#define QtBooleanOpOPERATIONACTION_H_

#include "Internal/ContextIfc.h"

#include "QtComponents/QtAnglePanel.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DEntityNamePanel.h"
#include "QtComponents/QtTopologyPanel.h"

#include <QtUtil/Qt3DDataPanel.h>
#include <QtUtil/QtDoubleTextField.h>
#include <QtUtil/QtIntTextField.h>

#include <qlineedit.h>
#include <QComboBox>


namespace Mgx3D
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération géométrique touchant
 * un cylindre.
 */
class QtBooleanOpOperationPanel : public QtMgx3DOperationPanel
{

    Q_OBJECT

    public :

    /** Le type d'operations booleen. */
        enum OPERATION_TYPE {GLUE, FUSE, COMMON, CUT, SECTION };

    /**
     * Créé l'ihm.
     * \param   Widget parent.
     * \param   Nom du panneau.
     * \param   Politique de création/modification du groupe
     * \param   Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
     *          notamment pour récupérer le contexte.
     * \param   Eventuelle action associée à ce panneau.
     */
    QtBooleanOpOperationPanel (
            QWidget* parent, const std::string& panelName,
            QtMgx3DGroupNamePanel::POLICY creationPolicy,
            Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
            Mgx3D::QtComponents::QtMgx3DOperationAction* action);

    /**
     * RAS.
     */
    virtual ~QtBooleanOpOperationPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

    /**
     * \return      Le nom des entités sur lesquelles travailler.

     */
    virtual std::vector<std::string> getEntityNames ( ) const;

    /* \return     Le nom des entité pour couper ou retirer
    */
    virtual std::vector<std::string> getCuttingNames ( ) const;

    /**
     * \return      Le type d'opération
     */
    virtual OPERATION_TYPE getOperationType ( ) const;

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
     * \see     autoUpdate
     */
    virtual void cancel ( );

    /**
     * Actualise le panneau en fonction du contexte.
     * \see     cancel
     */
    virtual void autoUpdate ( );


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


    protected slots :

    /**
     * Permet de modifier les préférences utilisateur.
     */
    virtual void newOperationType (int index);


    private :



    /**
     * Constructeur de copie et opérateur = opérations interdites.
     */
    QtBooleanOpOperationPanel (const QtBooleanOpOperationPanel&);
    QtBooleanOpOperationPanel& operator = (const QtBooleanOpOperationPanel&);

    /** L'opération booléenne appliquée. */
    QComboBox*                      _operationTypeComboBox;

    /** Le nom des volumes sur lesquels les opérations booléennes sont
     *  appliquées. */
    QtMgx3DEntityNamePanel*          _entitiesPanel;

    /** Le nom des volumes ou surface que l'on utilisent pour retirer ou couper */
    QtMgx3DEntityNamePanel*          _toolsToCutPanel;

};  // class QtBooleanOpOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtBooleanOpOperationPanel</I> de création/modification d'une coupe.
 */
class QtBooleanOpOperationAction : public QtMgx3DGeomOperationAction
{
    public :

    /**
     * Créé et s'associe une instance de la classe
     * <I>QtBooleanOpOperationPanel</I>.
     * \param       Icône représentant l'action.
     * \param       Texte représentant l'action.
     * \param       Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
     *              notamment pour récupérer le contexte et le panneau contenant
     *              les icônes.
     * \param       Tooltip décrivant l'action.
     * \param       Politique de création/modification du groupe
     */
    QtBooleanOpOperationAction (
        const QIcon& icon, const QString& text,
        Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
        const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

    /**
     * Destructeur. RAS.
     */
    virtual ~QtBooleanOpOperationAction ( );

    /**
     * \return      Le panneau d'édition du cylindre associé.
     */
    virtual QtBooleanOpOperationPanel* getBooleanPanel ( );

    /**
     * Créé/modifie le cylindre avec le paramétrage de son panneau associé.
     * Invoque préalablement
     * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
     */
    virtual void executeOperation ( );


    private :

    /**
     * Constructeur de copie et opérateur = : interdits.
     */
    QtBooleanOpOperationAction (const QtBooleanOpOperationAction&);
    QtBooleanOpOperationAction& operator = (
                                    const QtBooleanOpOperationAction&);
};  // class QtBooleanOpOperationAction



}   // namespace QtComponents

}   // namespace Mgx3D


#endif /* QtBooleanOpOPERATIONACTION_H_ */
