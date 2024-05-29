/**
 * \file		QtCommandMonitorPanel.h
 * \author		Charles PIGNEROL
 * \date		18/01/2011
 */

#ifndef QT_COMMAND_MONITOR_PANEL_H
#define QT_COMMAND_MONITOR_PANEL_H

#include <TkUtil/util_config.h>
#include "Utils/CommandManager.h"
#include <QtUtil/QtGroupBox.h>

#include <QCheckBox>
#include <QItemDelegate>
#include <QListWidget>
#include <QPushButton>
#include <QMenu>

#include <string>

namespace Mgx3D
{

namespace QtComponents
{
/**
 * <P>Widget de type liste spécialisé pour les commandes :<BR>
 * - Provoque, par emission de signal, un retour au thread principal pour
 * actualiser la représentation d'une commande (éventuellement en cours
 * d'exécution dans un thread dédié),<BR>
 * - Accès à un item représentant une commande à partir de son index
 * </P>
 */
class QtCmdListWidget : public QListWidget
{
	Q_OBJECT

	public :

	QtCmdListWidget (QWidget* parent);
	virtual ~QtCmdListWidget ( );
	virtual void addCommandItem (QListWidgetItem* item);
	virtual void deleteCommandItem (Utils::Command* command);
	virtual QModelIndex index (QListWidgetItem*) const;
	virtual bool hasCommand (Utils::Command* command) const;

	/**
	 * Emet le signal <I>modified</I>. Ugly code. Pour des besoins internes.
	 * @warning		Ne pas utiliser cette méthode.
	 */
	virtual void emitModified (const QModelIndex& index);


	signals	:

	/**
	 * Signal émis lorsqu'un item est modifié.
	 * Objectif : retour au thread principal pour mise à jour de l'item associé.
	 */
	void modified (const QModelIndex& index);


	private :

	QtCmdListWidget (const QtCmdListWidget&);
	QtCmdListWidget& operator = (const QtCmdListWidget&);

	mutable TkUtil::Mutex		_mutex;
};	// class QtCmdListWidget 


/**
 * \brief	Panneau Qt permettant de gérer des commandes Magix 3D. Représente à
 * 			cet effet une instance de la classe CommandManager.
 */
class QtCommandMonitorPanel : public QtGroupBox , public TkUtil::ObjectBase
{
	Q_OBJECT

	public :

	/** Constructeur
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		gestionnaire de commandes représenté. Ne doit pas être nul.
	 */
	QtCommandMonitorPanel (
					QWidget* parent, Mgx3D::Utils::CommandManager* manager);

	/** Destructeur. */
	virtual ~QtCommandMonitorPanel ( );

	/**
	 * Appelé lors de la modification d'une entité observée. S'actualise.
	 */
	virtual void observableModified (
					TkUtil::ReferencedObject* object, unsigned long event);

	/**
	 * Appelé lors de la destruction d'une entité observée. S'actualise.
	 */
	virtual void observableDeleted (TkUtil::ReferencedObject* object);

	/**
	 * Libère de toute contrainte vis à vis d'objets référencés.
	 */
	virtual void release ( );

	/**
	 * @return		La commande sélectionnée.
	 */
	virtual Utils::Command* getSelectedCommand ( );
	virtual const Utils::Command* getSelectedCommand ( ) const;


	public :

	/**
	 * Classe dédiée à la représentation des commandes (et de leurs états) dans
	 * des items de type <I>QtCommandItem</I>.
	 * La motivation première de cette classe est de représenter les commandes
	 * en cours d'exécution avec une barre de progression.
	 */
	class QtCommandItemDelegate : public QItemDelegate
	{
		public :
	
		/**
		 * Constructeur. RAS.
		 */
		QtCommandItemDelegate (QListWidget* parent);

		/**
		 * Destructeur. RAS.
		 */
		~QtCommandItemDelegate ( );

		/**
		 * Représente la commande dont l'index de l'item est transmis en
		 * argument.
		 */
		virtual void paint (
			QPainter* painter, const QStyleOptionViewItem& option,
			const QModelIndex& index ) const;


		private :

		/**
		 * Constructeur de copie et opérateur = : interdits.
		 */
		QtCommandItemDelegate (const QtCommandItemDelegate&);
		QtCommandItemDelegate& operator = (const QtCommandItemDelegate&);

		/** Liste contenant les items à représenter. */
		QListWidget*		_listWidget;
	};	// class QtCommandItemDelegate


	protected :

	/**
	 * Ajoute la commande transmise en argument à l'IHM. Ne l'affecte pas
	 * au manager de commandes représenté.
	 */
	virtual void addCommand (Mgx3D::Utils::Command* command);

	/**
	 * \return		<I>true</I> si la commande transmise en argument doit
	 *				figurer dans la liste, <I>false</I> dans le cas contraire.
	 * \see			processingCallback
	 * \see			shouldDisplayAll
	 */
	virtual bool shouldDisplayCommand (
									const Mgx3D::Utils::Command& command) const;

	/**
	 * \return		<I>true</I> si toutes les commandes doivent être affichées,
	 *				<I>false</I> dans le cas contraire.
	 * \see			shouldDisplayCommand
	 */
	 virtual bool shouldDisplayAll ( ) const;


	signals :

	/**
	 * Emis lorsqu'une commande observée est modifiée.
	 * Pas d'argument : la commande considérée peut avoir été détruite le
	 * temps que le signal arrive (mécanismes Qt requiérant un changement de
	 * thread).
	 */
	void commandModified ( );

	/**
	 * Emis lorsqu'une commande observée est détruite.
	 * @param	pointeur sur la commande détruite.
	 * @warning	<B>ATTENTION :</B> commande détruite.
	 */
	void commandDeleted (Mgx3D::Utils::Command* command);


	protected slots :

	/**
	 * Mise à jour de l'IHM selon l'état de la sélection.
	 */
	virtual void updateGuiCallback ( );

	/**
	 * Mise à jour de l'IHM suite à la destruction d'un commande.
	 * @parama	Commande <U>détruite</U>.
	 */
	virtual void commandDeletedCallback (Mgx3D::Utils::Command* command);

	/**
	 * Annule la commande sélectionnée et en cours d'exécution.
	 */
	virtual void cancelCallback ( );

	/**
	 * Tue la commande sélectionnée et en cours d'exécution.
	 * @warning		<B>Selon la commande, est fortement susceptible de
	 *				<U>laisser l'application dans un état instable (fuites de
	 *				mémoire, risque de plantage et de deadlocks dus à des
	 *				mutex non dévérouillés)</U></B>.
	 */
	virtual void killCallback ( );

	/**
	 *  Ouvre un menu pour sélectionner un type d'entités créées par la commande sélectionnée
	 */
	virtual void selectCallback ( );

	/** Sélectionne les volumes géométriques créés par la commande sélectionnée */
	virtual void selectGeomVolumesCallback ( );

	/** Sélectionne les surfaces géométriques créées par la commande sélectionnée */
	virtual void selectGeomSurfacesCallback ( );

	/** Sélectionne les courbes géométriques créées par la commande sélectionnée */
	virtual void selectGeomCurvesCallback ( );

	/** Sélectionne les sommets géométriques créés par la commande sélectionnée */
	virtual void selectGeomVerticesCallback ( );

	/** Sélectionne les blocs topologiques créés par la commande sélectionnée */
	virtual void selectTopoBlocksCallback ( );

	/** Sélectionne les faces communes topologiques créées par la commande sélectionnée */
	virtual void selectTopoCoFacesCallback ( );

	/** Sélectionne les arêtes communes topologiques créées par la commande sélectionnée */
	virtual void selectTopoCoEdgesCallback ( );

	/** Sélectionne les sommets topologiques créés par la commande sélectionnée */
	virtual void selectTopoVerticesCallback ( );

	/** Sélectionne les volumes de mailles créés par la commande sélectionnée */
	virtual void selectMeshVolumesCallback ( );

	/** Sélectionne les surfaces de mailles créées par la commande sélectionnée */
	virtual void selectMeshSurfacesCallback ( );

	/** Sélectionne les nuages de noeuds créés par la commande sélectionnée */
	virtual void selectMeshCloudsCallback ( );

	/** Ouverture de la page web d'aide */
	virtual void helpCallback ( );

	/**
	 * Selon la case à cocher prévue à cet effet affiche toutes les commandes ou
	 * uniquement celles en cours d'exécution.
	 * \see			shouldDisplayCommand
	 */
	virtual void processingCallback ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtCommandMonitorPanel (const QtCommandMonitorPanel&);

	/** Opérateur de copie. Interdit. */
	QtCommandMonitorPanel& operator = (const QtCommandMonitorPanel&);

	/** La liste des commandes gérées. */
	QtCmdListWidget*					_commandListWidget;

	/** L'affichage des commandes en cours d'exécution (coché) ou de toutes
	 * les commandes (non cochée).
	 * \see			shouldDisplayCommand
	 */
	QCheckBox*							_processingCheckBox;

	/** Les boutons "Annuler" et "Tuer". */
	QPushButton							*_cancelButton, *_killButton;

	/** Le bouton de "Sélection ..." */
	QPushButton							*_selectButton;

	/** Menu associé au bouton "Sélection ..." */
	QMenu                               *_selectMenu;

	/** Le gestionnaire de commandes représenté. */
	Mgx3D::Utils::CommandManager*		_commandManager;
};	// class QtCommandMonitorPanel


/**
 * Item représentant une commande Magix 3D.
 * 
 * @warning		Peut être amené à être réactualisé dans un thread autre que 
 *				de son parent, ce qui peut conduire au plantage => changement de
 *				thread pour les aspects graphiques (<I>Qt</I>).
 */
class QtCommandItem : public QListWidgetItem, public TkUtil::ObjectBase
{
	public :

	/**
	 * \param	Widget parent. Cet item sera recensé auprès du parent par
	 *			<I>parent->addCommandItem (this)</I>.
	 * \param	Commande représentée.
	 */
	QtCommandItem (QtCmdListWidget& parent, Mgx3D::Utils::Command*);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtCommandItem ( );

	/**
	 * Appelé lors de la modification de la commande représentée. 
	 * Provoque l'émission par son parent du signal <I>modified</I> en vue de
	 * s'actualiser dans le thread du parent et d'éviter ainsi un plantage si le
	 * thread courant est un thread dédié à l'exécution d'une commande.
	 */
	virtual void observableModified (
					TkUtil::ReferencedObject* object, unsigned long event);

	/**
	 * Appelé lors de la destruction de la commande représentée. S'actualise.
	 */
	virtual void observableDeleted (TkUtil::ReferencedObject* object);

	/**
	 * \return		La commande représentée.
	 * \exception	Une exception est levée s'il n'y a pas de commande
	 *				représentée.
	 */
	Mgx3D::Utils::Command& getCommand ( );
	const Mgx3D::Utils::Command& getCommand ( ) const;

	/**
	 * \return		Pointeur sur la commande représentée.
	 * \warning		Pour des besoins internes : la commande peut être détruite.
	 */
	const Mgx3D::Utils::Command* getCommandPtr ( ) const
	{ return _commandPtr; }


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtCommandItem (const QtCommandItem&);
	QtCommandItem& operator = (const QtCommandItem&);

	/** La commande représentée. */
	Mgx3D::Utils::Command*			_command;
	Mgx3D::Utils::Command*			_commandPtr;
};	// class QtCommandItem


}	// namespace QtComponents

}	// namespace Mgx3D


#endif	// QT_COMMAND_MONITOR_PANEL_H
