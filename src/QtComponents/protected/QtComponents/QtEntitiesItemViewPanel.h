/**
 * \file		QtEntitiesItemViewPanel.h
 * \author		Charles PIGNEROL
 * \date		07/02/2012
 */
#ifndef QT_ENTITIES_ITEM_VIEW_PANEL_H
#define QT_ENTITIES_ITEM_VIEW_PANEL_H


#include <QtUtil/QtGroupBox.h>

#include "Utils/Entity.h"
#include "QtComponents/Qt3DGraphicalWidget.h"
#include "QtComponents/QtRepresentationTypesPanel.h"
#include "Internal/Context.h"


namespace Mgx3D 
{

namespace QtComponents
{

class QtMgx3DMainWindow;


/**
 * \brief		Panneau abstrait de l'IHM <I>Magix 3D</I> affichant des entités 
 *				d'une session <I>Magix 3D</I>.
 *				L'affichage est ici a priori textuel et couplé à un affichage
 *				dans une fenêtre graphique 3D.
 *
 *				<P>L'objectif de cette classe non instanciable est de factoriser
 *				certains services des vues textuelles type <I>QTreeWidget</I>
 *				d'entités.
 *				</P>
 */
class QtEntitiesItemViewPanel : public QtGroupBox
{
	protected :

	/**
	 * \param	Widget parent.
	 * \param	Fenêtre principale de rattachement du panneau
	 * \param	Nom du panneau.
	 * \param	Contexte Magix 3D du panneau
	 */
	QtEntitiesItemViewPanel (
			QWidget* parent, QtMgx3DMainWindow* mainWindow,
			const std::string& name, Internal::ContextIfc& context);

	/**
	 * RAS.
	 */
	virtual ~QtEntitiesItemViewPanel ( );


	public :

	/**
	 * @return		Le titre de l'application.
	 */
	virtual const std::string& getAppTitle ( ) const;

	/**
	 * \return		La fenêtre principale à laquelle est rattaché le panneau.
	 */
	virtual const QtMgx3DMainWindow& getMainWindow ( ) const;
	virtual QtMgx3DMainWindow& getMainWindow ( );

	/**
	 * \param		La fenêtre principale à laquelle est rattaché le panneau.
	 */
	virtual void setMainWindow (QtMgx3DMainWindow*);

	/**
	 * La gestion de la sélection.
	 */
	//@{

	/**
	 * \return		Contexte <I>Magix 3D</I> courant.
	 * \see			setContext
	 */
	virtual Mgx3D::Internal::ContextIfc& getContext ( );

	/**
	 * \param		Nouveau contexte <I>Magix 3D</I>.
	 * \see			getContext
	 */
	virtual void setContext (Mgx3D::Internal::ContextIfc*);

	//@}	// La gestion de la sélection.


	/**
	 * Les affichages graphiques 3D.
	 */
	//@{

	/**
	 * \param		La fenêtre graphique à utiliser pour les affichages 3D,
	 *				ou 0.
	 * \exception	Une exception est levée si la fenêtre transmise en
	 *				argument est non nulle et qu'une autre fenêtre est déjà
	 *				affectée.
	 * \see			getGraphicalWidget
	 */
	virtual void setGraphicalWidget (Qt3DGraphicalWidget* widget3D);

	/**
	 * \return		La fenêtre graphique 3D utilisée.
	 * \see			setGraphicalWidget
	 */
	virtual Qt3DGraphicalWidget* getGraphicalWidget ( );
	virtual const Qt3DGraphicalWidget* getGraphicalWidget ( ) const;

	//@}	// Les affichages graphiques 3D.

	/**
	 * La gestion des informations à remonter à l'utilisateur.
	 */
	//@{

	/**
	 * \param		Flux d'informations à utiliser.
	 * \see			log
	 */
	virtual void setLogStream (TkUtil::LogOutputStream* stream);

	/**
	 * \param		Ecrit le log transmis en argument dans le flux
	 *				d'informations associé.
	 */
	virtual void log (const TkUtil::Log& log);

	//@}	// La gestion des informations à remonter à l'utilisateur.

	/**
	 * Appelé pour modifier les paramètres d'affichage
	 * (points/filaire/surfacique) des entités données en argument. Affiche une
	 * boite de dialogue de choix des types de représentation à utiliser,
	 * initialisé selon les entités transmises en argument, et effectue les
	 * modifications demandées.
	 */
	virtual void changeRepresentationTypes (
						const std::vector<Mgx3D::Utils::Entity*>& entities);


	protected :

	// Opérations interdites :
	QtEntitiesItemViewPanel (const QtEntitiesItemViewPanel&);


	private :

	QtEntitiesItemViewPanel& operator = (const QtEntitiesItemViewPanel&);

	/** Le context Magix 3D. */
	Mgx3D::Internal::ContextIfc*	_context;

	/** La fenêtre principale à laquelle ce panneau est rattaché. */
	QtMgx3DMainWindow*				_mainWindow;

	/** Le widget graphique représentant les entités en 3D. */
	Qt3DGraphicalWidget*			_graphicalWidget;

	/** L'éventuel flux sortant d'informations. */
	TkUtil::LogOutputStream*		_logStream;
};	// class QtEntitiesItemViewPanel


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_ENTITIES_ITEM_VIEW_PANEL_H
