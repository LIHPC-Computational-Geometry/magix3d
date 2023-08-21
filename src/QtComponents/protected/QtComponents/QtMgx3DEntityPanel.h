/**
 * \file		QtMgx3DEntityPanel.h
 * \author		Charles PIGNEROL
 * \date		09/09/2013
 */

#ifndef QT_MGX3D_ENTITY_PANEL_H
#define QT_MGX3D_ENTITY_PANEL_H

#include "Internal/ContextIfc.h"

#include <TkUtil/util_config.h>

#include <QtComponents/QtEntityIDTextField.h>

#include <QLabel>

#include <string>
#include <vector>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * Panneau Qt permettant la saisie d'une entité dans Magix 3D.
 */
class QtMgx3DEntityPanel : public QWidget
{
	Q_OBJECT

	public :

	/** Constructeur
	 * \param		widget parent. Ne doit pas être nul.
	 * \param		titre du panneau
	 * \param		true si le panneau est éditable, sinon false.
	 * \param		Libellé du champ de texte.
	 * \param		Eventuel identifiant unique initial.
	 * \param		Fenêtre principale associée, pour la saisie interactive de
	 *				l'entité.
	 * \param		Dimensions possibles des entités à sélectionner
	 * \param		Types d'"entités" sélectionnables
	 */
	QtMgx3DEntityPanel (
					QWidget* parent, const std::string& title, bool editable,
					const std::string& label, const std::string& uniqueName,
					Mgx3D::QtComponents::QtMgx3DMainWindow* mainWindow,
					Mgx3D::Utils::SelectionManagerIfc::DIM dimensions,
					Mgx3D::Utils::FilterEntity::objectType type);

	/** Destructeur. */
	virtual ~QtMgx3DEntityPanel ( );

	/**
	 * \param		Nouveau libellé du champ de texte.
	 */
	virtual void setLabel (const std::string& label);

	/**
	 * \return			true si l'utilisateur a modifié le nom unique.
	 */
	virtual bool isModified ( ) const;

	/**
	 * \return		<I>true</I> si le panneau permet la saisie multiple,
	 *				<I>false</I> dans le cas contraire.
	 * \see			setMultiSelectMode
	 */
	virtual bool isMultiSelectMode ( ) const;

	/**
	 * \param		<I>true</I> si le panneau doit permettre la saisie
	 *				multiple, <I>false</I> dans le cas contraire (saisie
	 *				d'un seul nom).
	 * \see			isMultiSelectMode
	 */
	virtual void setMultiSelectMode (bool multi);

	/**
	 * \param		Nouveau nom unique.
	 */
	virtual void setUniqueName (const std::string& name);

	/**
	 * \return		Le nom unique.
	 * \see			getUniqueNames
	 */
	virtual std::string getUniqueName ( ) const;

	/**
	 * \return		Les noms uniques (cas de la sélection multiple activée).
	 * \see			getUniqueName
	 */
	virtual std::vector<std::string> getUniqueNames ( ) const;

	/**
	 * \param		Les noms uniques (cas de la sélection multiple activée).
	 * \see			getUniqueNames
	 */
	virtual void setUniqueNames (const std::vector<std::string>& names);

	/**
	 * \return		le widget utilisé pour la saisie du nom unique.
	 */
	virtual Mgx3D::QtComponents::QtEntityIDTextField* getNameTextField ( );
	virtual const Mgx3D::QtComponents::QtEntityIDTextField* getNameTextField ( ) const;

	/**
	 * \param		Nouvelles dimensions possibles.
	 * \see			getFilteredDimensions
	 */
	virtual void setFilteredDimensions (Mgx3D::Utils::SelectionManagerIfc::DIM dimensions);

	/**
	 * \return		Les dimensions possibles.
	 * \see			setFilteredDimensions
	 * \see			getFilteredTypes
	 */
	virtual Mgx3D::Utils::SelectionManagerIfc::DIM getFilteredDimensions ( ) const;

	/**
	 * \return		Les types filtrés.
	 * \see			getFilteredDimensions
	 */
	virtual Mgx3D::Utils::FilterEntity::objectType getFilteredTypes ( ) const;

	/**
	 * \param		Les types filtrés.
	 * \see			getFilteredTypes
	 */
	virtual void setFilteredTypes(Mgx3D::Utils::FilterEntity::objectType types);

	/**
	 * Réinitialise le champ de saisie.
	 */
	virtual void reset ( );

	/**
	 * Annule la sélection en cours, même en mode de sélection interactive.
	 */
	virtual void clearSelection ( );

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );

	/**
	 * Actualisation de la représentation de la sélection.
	 * \return	<I>true</I> en cas de modification, <I>false</I> dans le cas
	 * 			contraire.
	 */
	virtual bool actualizeGui (bool removeDestroyed);


	signals :

	/**
	 * Signal émis lorsque la sélection est modifiée par l'utilisateur.
	 * \param	Chaine décrivant la nouvelle sélection.
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
	 * Invoqué lorsque la sélection est modifiée par l'utilisateur.
	 * Emet le signal selectionModified.
	 */
	virtual void selectionModifiedCallback (QString selection);

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


	protected :


	private :

	/** Constructeur de copie. Interdit. */
	QtMgx3DEntityPanel (const QtMgx3DEntityPanel&);

	/** Opérateur de copie. Interdit. */
	QtMgx3DEntityPanel& operator = (const QtMgx3DEntityPanel&);

	/** Le champ de saisie du nom unique.  */
	Mgx3D::QtComponents::QtEntityIDTextField*	_nameTextField;

	/** Le nom unique initial. */
	std::string					_initialUniqueName;

	/** Le libellé du champ de texte. */
	QLabel*						_label;

	/** Le titre du panneau. */
	const std::string				_title;
};	// class QtMgx3DEntityPanel


}	// namespace QtComponents

}	// namespace Mgx3D



#endif	// QT_MGX3D_ENTITY_PANEL_H
