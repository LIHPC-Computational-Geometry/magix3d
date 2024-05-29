/**
 * \file		QtMgx3DEdgePanel.h
 * \author		Charles PIGNEROL
 * \date		30/01/2013
 */

#ifndef QT_MGX3D_EDGE_PANEL_H
#define QT_MGX3D_EDGE_PANEL_H

#include "Internal/ContextIfc.h"

#include <TkUtil/util_config.h>

#include "QtComponents/QtMgx3DEntityPanel.h"
#include "Internal/Context.h"

#include <string>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * Panneau Qt permettant la saisie d'une arête dans Magix 3D.
 */
class QtMgx3DEdgePanel : public QWidget
{
	Q_OBJECT

	public :

	/** Constructeur
	 * \param		widget parent. Ne doit pas être nul.
	 * \param		titre du panneau
	 * \param		true si le panneau est éditable, sinon false.
	 * \param		Eventuel identifiant unique initial.
	 * \param		Fenêtre principale associée, pour la saisie interactive de
	 *				l'arête.
	 * \param		Types d'"arêtes" sélectionnables
	 */
	QtMgx3DEdgePanel (QWidget* parent, const std::string& title, bool editable,
					const std::string& uniqueName,
					Mgx3D::QtComponents::QtMgx3DMainWindow* mainWindow,
					Mgx3D::Utils::FilterEntity::objectType type);

	/** Destructeur. */
	virtual ~QtMgx3DEdgePanel ( );

	/**
	 * \return			true si l'utilisateur a modifié le nom unique.
	 */
	virtual bool isModified ( ) const;

	/**
	 * \return			<I>true</I> si le panneau permet la saisie multiple,
	 * 					<I>false</I> dans le cas contraire.
	 * \see				setMultiSelectMode
	 */
	virtual bool isMultiSelectMode ( ) const;

	/**
	 * \param			<I>true</I> si le panneau doit permettre la saisie
	 * 					multiple, <I>false</I> dans le cas contraire (saisie
	 * 					d'un seul nom).
	 * 					isMultiSelectMode
	 */
	 virtual void setMultiSelectMode (bool multi);

	/**
	 * \param			Nouveau nom unique.
	 */
	virtual void setUniqueName (const std::string& name);

	/**
	 * \return			Le nom unique.
	 */
	virtual std::string getUniqueName ( ) const;

	/**
	 * \param			Nouveaux noms uniques.
	 */
	virtual void setUniqueNames (const std::vector<std::string>& names);

	/**
	 * \return			Le noms uniques.
	 */
	virtual std::vector<std::string> getUniqueNames ( ) const;

	/**
	 * \return			Une référence sur le panneau de saisie de l'arête.
	 */
	virtual QtMgx3DEntityPanel& getEntityPanel ( );
	virtual const QtMgx3DEntityPanel& getEntityPanel ( ) const;

	/**
	 * \return			Une référence sur le champ de texte de saisie de
	 *					l'arête.
	 */
	virtual QtEntityIDTextField& getEntityTextField ( );
	virtual const QtEntityIDTextField& getEntityTextField ( ) const;

	/**
	 * \param			En retour, les points définissant l'arête.
	 * \exception		Une exception est levée en cas d'absence d'arête.
	 */
	virtual void getPoints (Utils::Math::Point&, Utils::Math::Point&) const;

	/**
	 * Réinitialise le champ de saisie.
	 */
	virtual void reset ( );

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

	/**
	 * \return	Une référence sur le contexte <I>Magix 3D</I> associé à
	 *			l'opération.
	 */
	virtual const Mgx3D::Internal::ContextIfc& getContext ( ) const;


	signals :

	/**
	 * Signal émis lorsque la sélection est modifiée par l'utilisateur.
	 * \param	Chaine décrivant la nouvelle sélection.
	 */
	void selectionModified (QString);

	/**
	 * Signal émis lorsqu'une arête est ajoutée à la sélection.
	 * \param		Nom de l'arête ajoutée.
	 * \see			edgeRemovedFromSelection
	 * \see			selectionModified
	 */
	void edgeAddedToSelection (QString);

	/**
	 * Signal émis lorsqu'une arête est enlevée de la sélection.
	 * \param		Nom de l'arête enlevée.
	 * \see			edgeAddedToSelection
	 * \see			selectionModified
	 */
	void edgeRemovedFromSelection (QString);

	/**
	 * Signal émis lorsque des arêtes sont ajoutées à la sélection.
	 * \param		Nom des arêtes ajoutées.
	 * \see			edgesRemovedFromSelection
	 * \see			selectionModified
	 */
	void edgesAddedToSelection (QString);

	/**
	 * Signal émis lorsque des arêtes sont enlevées de la sélection.
	 * \param		Nom des arêtes enlevées.
	 * \see			edgesAddedToSelection
	 * \see			selectionModified
	 */
	void edgesRemovedFromSelection (QString);


	protected slots :

	/**
	 * Invoqué lorsque la sélection est modifiée par l'utilisateur.
	 * Emet le signal selectionModified.
	 */
	virtual void selectionModifiedCallback (QString selection);

	/**
	 * Invoqué lorsqu'une arête est ajoutée à la sélection.
	 * Emet le signal edgeAddedToSelection.
	 */
	virtual void edgeAddedToSelectionCallback (QString edgeName);

	/**
	 * Invoqué lorsqu'une arête est enlevée de la sélection.
	 * Emet le signal edgeRemovedFromSelection.
	 */
	virtual void edgeRemovedFromSelectionCallback (QString edgeName);

	/**
	 * Invoqué lorsque des arêtes sont ajoutées à la sélection.
	 * Emet le signal edgesAddedToSelection.
	 */
	virtual void edgesAddedToSelectionCallback (QString edgesNames);

	/**
	 * Invoqué lorsque des arêtes sont enlevées de la sélection.
	 * Emet le signal edgesRemovedFromSelection.
	 */
	virtual void edgesRemovedFromSelectionCallback (QString edgesNames);


	private :

	/** Constructeur de copie. Interdit. */
	QtMgx3DEdgePanel (const QtMgx3DEdgePanel&);

	/** Opérateur de copie. Interdit. */
	QtMgx3DEdgePanel& operator = (const QtMgx3DEdgePanel&);

	/** Le contexte <I>Magix 3D</I> de l'opération. */
	Mgx3D::QtComponents::QtMgx3DMainWindow*			_mainWindow;

	/** Le champ de saisie du nom unique.  */
	QtMgx3DEntityPanel*					_entityTextField;

	/** Le nom unique initial. */
	std::string							_initialUniqueName;

	/** Le titre du panneau. */
	const std::string					_title;
};	// class QtMgx3DEdgePanel


}	// namespace QtComponents

}	// namespace Mgx3D



#endif	// QT_MGX3D_EDGE_PANEL_H
