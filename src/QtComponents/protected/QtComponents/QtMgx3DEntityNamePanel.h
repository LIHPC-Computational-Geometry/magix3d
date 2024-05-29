/*
 * QtMgx3DEntityNamePanel.h
 *
 *  Created on: 20 mars 2013
 *      Author: ledouxf
 */

#ifndef QTMGX3DENTITYNAMEPANEL_H_
#define QTMGX3DENTITYNAMEPANEL_H_

#include "Internal/ContextIfc.h"

#include "Utils/ValidatedField.h"
#include "Group/GroupManagerIfc.h"
#include "QtComponents/QtMgx3DMainWindow.h"

#include "QtComponents/QtEntityIDTextField.h"

#include <vector>
#include <QLabel>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * Classe permettant la saisie d'un ensemble de noms d'entités <I>Magix 3D</I>
 * et offrant une <I>API</I> de validation <I>ValidatedField</I>.
 * \see     ValidatedField
 */
class QtMgx3DEntityNamePanel :
            public QWidget, public Mgx3D::Utils::ValidatedField
{
	Q_OBJECT

    public :

    /**
     * \param   Widget parent.
     * \param   Nom du panneau.
     * \param   Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
     *          notamment pour récupérer le contexte.
     * \param   Dimensions possibles des entités saisies dans le panneau.
     * \param   Types d'entités sélectionnées
     *
     * \see     getContext
     */
    QtMgx3DEntityNamePanel (
        QWidget* parent, const std::string& name,
        Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
        Mgx3D::Utils::SelectionManagerIfc::DIM dimensions,
		Utils::FilterEntity::objectType types);

    /**
     * RAS.
     */
    virtual ~QtMgx3DEntityNamePanel ( );

    /**
     * \return      Contexte <I>Magix 3D</I> courant.
     */
     virtual Mgx3D::Internal::ContextIfc& getContext ( );

    /**
     * \return      La liste des noms saisies
     * \see			setNames
     */
    virtual std::vector<std::string> getNames( ) const;

    /**
     * \param      La liste des noms saisies
     * \see			getNames
     */
    virtual void setNames (const std::vector<std::string>& names);

	/**
	 * Réinitialise le champ de saisie.
	 */
	virtual void reset ( );


	/**
	 * Vide la liste des noms saisis.
	 */
	virtual void clear ( );

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );

    /**
     * Lève une exception de type <I>TkUtil::Exception</I> si le nom saisi par
     * l'utilisateur est invalide compte-tenu de la politique appliquée.
     * \see         getPolicy
     */
    virtual void validate ( );

    /**
     * Actualise le panneau en fonction du contexte.
     */
    virtual void autoUpdate ( );

    /**
     * Change le label définissant ce que l'on attend dans le champ de sélection
     */
    virtual void setLabel (const std::string& label);

	/**
	 * Change les dimensions possibles des entités à sélectionner
	 * \see			getFilteredDimensions
	 */
	virtual void setDimensions (
							Mgx3D::Utils::SelectionManagerIfc::DIM dimensions);

    /**
     * Change le type des entités attendues
     */
    virtual void setFilteredTypes (Utils::FilterEntity::objectType types);

    /**
     * Accepte ou non la sélection multiple
     */
    virtual void setMultiSelectMode (bool enable);

	/**
	 * \return	Le champ de saisie des noms d'entité.
	 */
	Mgx3D::QtComponents::QtEntityIDTextField* getNamesTextField ( );


	signals :

	/**
	 * Signal émis lorsque la sélection est modifiée.
	 * \param		Noms des entités sélectionnées.
	 */
	void selectionModified (QString);

	/**
	 * Signal émis lorsque des entités sont ajoutées à la sélection.
	 * \param		Nom des entités ajoutées.
	 * \see			entitiesRemovedFromSelection
	 * \see			selectionModified
	 */
	void entitiesAddedToSelection (QString);

	/**
	 * Signal émis lorsque des entités sont enlevées de la sélection.
	 * \param		Nom des entités enlevées.
	 * \see			entitiesAddedToSelection
	 * \see			selectionModified
	 */
	void entitiesRemovedFromSelection (QString);


	protected slots :

	/**
	 * Invoqué lorsque la sélection est modifiée.
	 * Emet le signal selectionModified.
	 */
	virtual void selectionModifiedCallback (QString entityName);

	/**
	 * Appelé lorsque l'utilisateur ajoute des entités à la sélection.
	 * \param		Entités ajoutées à la sélection
	 * \see			removeFromSelection
	 * \see			setSelectedEntity
	 * \see			setSelectedEntities
	 */
	virtual void entitiesAddedToSelectionCallback (QString entitiesNames);

	/**
	 * Appelé lorsque l'utilisateur enlève des entités de la sélection.
	 * \param		Entités enlevées de la sélection
	 * \see			addToSelection
	 * \see			setSelectedEntity
	 * \see			setSelectedEntities
	 */
	virtual void entitiesRemovedFromSelectionCallback (QString entitiesNames);


    private :

    /**
     * Constructeur de copie/opérateur = : Opérations interdites :
     */
    QtMgx3DEntityNamePanel (const QtMgx3DEntityNamePanel&);
    QtMgx3DEntityNamePanel& operator = (const QtMgx3DEntityNamePanel&);

    /** Le widget de saisie du ou des noms. */
    QtEntityIDTextField*                        _names;

    /** La fenêtre <I>Magix 3D</I> de rattachement. */
    Mgx3D::QtComponents::QtMgx3DMainWindow*     _mainWindow;

    /** Label définissant ce que l'on attend dans le champ de sélection */
    QLabel*                                     _label;


};  // class QtMgx3DGroupEntityPanel


}   // namespace QtComponents

}   // namespace Mgx3D


#endif /* QTMGX3DENTITYNAMEPANEL_H_ */
