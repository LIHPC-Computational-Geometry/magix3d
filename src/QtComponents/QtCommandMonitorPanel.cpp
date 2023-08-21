/**
 * \file		QtCommandMonitorPanel.cpp
 * \author		Charles PIGNEROL
 * \date		18/01/2011
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Utils/ErrorManagement.h"
#include "Utils/Magix3DEvents.h"
#include "QtComponents/QtCommandMonitorPanel.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtMgx3DApplication.h"
#include "Internal/M3DCommandResult.h"
#include "Internal/CommandInternal.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/ReferencedMutex.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtHelpWindow.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <QLinearGradient>
#include <QHeaderView>
#include <QLayout>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QWhatsThis>
#include <QCursor>

#include <assert.h>
#include <iostream>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//           LA CLASSE QtCmdListWidget
// ===========================================================================

QtCmdListWidget::QtCmdListWidget (QWidget* parent)
	: QListWidget (parent), _mutex ( )
{
	connect (this, SIGNAL (modified (const QModelIndex&)), this,
	         SLOT (update (const QModelIndex&)));
}	// QtCmdListWidget::QtCmdListWidget


QtCmdListWidget::QtCmdListWidget (const QtCmdListWidget&)
	: QListWidget (0), _mutex ( )
{
	MGX_FORBIDDEN ("QtCmdListWidget operator = is not allowed.");
}	// QtCmdListWidget::QtCmdListWidget


QtCmdListWidget& QtCmdListWidget::operator = (const QtCmdListWidget&)
{
	MGX_FORBIDDEN ("QtCmdListWidget operator = is not allowed.");
	return *this;
}	// QtCmdListWidget::operator =


QtCmdListWidget::~QtCmdListWidget ( )
{
}	// QtCmdListWidget::~QtCmdListWidget


void QtCmdListWidget::addCommandItem (QListWidgetItem* item)
{
	if (0 == item)
		return;

	AutoMutex	autoMutex (&_mutex);

	addItem (item);
	scrollToBottom ( );
}	// QtCmdListWidget::addCommandItem


void QtCmdListWidget::deleteCommandItem (Command* command)
{
	if (0 == command)
		return;

	AutoMutex	autoMutex (&_mutex);

	for (size_t i = 0; i < count ( ); i++)
	{
		QtCommandItem*	commandItem	= dynamic_cast<QtCommandItem*>(item (i));
		if ((0 != commandItem) && (commandItem->getCommandPtr ( ) == command))
		{
			takeItem (i);
			delete commandItem;
			break;
		}	// if ((0 != commandItem) && (&(commandItem->getCommand ( )) == ...
	}	// for (size_t i = 0; i < count ( ); i++)
}	// QtCmdListWidget::deleteCommandItem


QModelIndex QtCmdListWidget::index (QListWidgetItem* item) const
{
	AutoMutex	autoMutex (&_mutex);

	return indexFromItem (item);
}	// QtCmdListWidget::index


bool QtCmdListWidget::hasCommand (Utils::Command* command) const
{
	if (0 == command)
		return false;

	AutoMutex	autoMutex (&_mutex);

	for (size_t i = 0; i < count ( ); i++)
	{
		QtCommandItem*	commandItem	= dynamic_cast<QtCommandItem*>(item (i));
		if ((0 != commandItem) && (commandItem->getCommandPtr ( ) == command))
			return true;
	}	// for (size_t i = 0; i < count ( ); i++)

	return false;
}	// QtCmdListWidget::hasCommand


void QtCmdListWidget::emitModified (const QModelIndex& index)
{
	AutoMutex	autoMutex (&_mutex);

	// Provoque l'actualisation de l'item d'index <I>index</I> dans le thread
	// de ce widget.
	emit modified (index);
}	// QtCmdListWidget::emitModified


// ===========================================================================
//           LA CLASSE QtCommandMonitorPanel::QtCommandItemDelegate
// ===========================================================================

QtCommandMonitorPanel::QtCommandItemDelegate::QtCommandItemDelegate (
															QListWidget* parent)
	: QItemDelegate (parent), _listWidget (parent)
{
}	// QtCommandItemDelegate::QtCommandItemDelegate


QtCommandMonitorPanel::QtCommandItemDelegate::QtCommandItemDelegate (
							const QtCommandMonitorPanel::QtCommandItemDelegate&)
	: QItemDelegate (0), _listWidget (0)
{
	MGX_FORBIDDEN ("QtCommandItemDelegate operator = is not allowed.");
}	// QtCommandItemDelegate::QtCommandItemDelegate


QtCommandMonitorPanel::QtCommandItemDelegate&
				QtCommandMonitorPanel::QtCommandItemDelegate::operator = (
							const QtCommandMonitorPanel::QtCommandItemDelegate&)
{
	MGX_FORBIDDEN ("QtCommandItemDelegate operator = is not allowed.");
	return *this;
}	// QtCommandItemDelegate::operator =


QtCommandMonitorPanel::QtCommandItemDelegate::~QtCommandItemDelegate ( )
{
}	// QtCommandItemDelegate::~QtCommandItemDelegate


void QtCommandMonitorPanel::QtCommandItemDelegate::paint (
				QPainter* painter, const QStyleOptionViewItem& option,
				const QModelIndex& index) const
{
	if ((0 == painter) || (0 == _listWidget))
		return;

	QtCommandItem*	item	=
			dynamic_cast<QtCommandItem*>(_listWidget->item (index.row ( )));
	if (0 == item)
	{
		QItemDelegate::paint (painter, option, index);
		return;
	}	// if (0 == item)
	// Si getCommand lève une exception (en cours de destruction), on quitte, 
	// car sinon on boucle sur une boite de dialogue d'erreur.
	try	
	{
		if (0 != item)
			item->getCommand ( );
	}
	catch (...)
	{
		return;
	}
	QBrush				brush		= painter->background ( );
	double				progress	= 0;
	Command::status		status		= Command::FAIL;
	const UTF8String	cmdText		= item->getCommand ( ).getStrProgression( );
	if (0 != item)
	{
		BEGIN_QT_TRY_CATCH_BLOCK

		status		= item->getCommand ( ).getStatus ( );
		progress	= item->getCommand ( ).getProgression ( );
#ifdef _DEBUG2
		std::cout <<"QtCommandItemDelegate::paint avec status="<<item->getCommand ( ).getStrStatus()
				  <<", cmdText = "<<cmdText
				  <<", progress = "<<progress<<std::endl;
#endif
		switch (status)
		{
			case Command::INITED	:
			case Command::STARTING  : brush	= QBrush (Qt::yellow);	break;
			case Command::PROCESSING:
				brush	= QBrush (QColor (36, 182, 208));
				break;
			case Command::DONE		: brush	= QBrush (Qt::green);	break;
				break;
			case Command::CANCELED	: brush	= QBrush (QColor (255, 0, 125));
				break;
			default					: brush	= QBrush (Qt::red);	break;
		}	// switch (status)

		COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), item->listWidget ( ), "Magix 3D - Moniteur des commandes")
	}	// if (0 != item)
	// Rem : en Qt 4.6.2 QtItemDelegate::drawBackground n'est pas virtuelle.
	// Dommage, c'était suffisant ...

	// Code issu de QItemDelegate::paint de Qt 4.6.2 :
	QStyleOptionViewItemV4	opt	= setOptions (index, option);
	painter->save ( );
	painter->setClipRect (opt.rect);
	QVariant	value	= index.data (Qt::DecorationRole);
	QPixmap		pixmap;
	QRect		decorationRect;
	if (value.isValid ( ))
	{
		pixmap	= decoration(opt, value);
		decorationRect = QRect(QPoint(0, 0), pixmap.size());
	}	// if (value.isValid ( ))
	else
		decorationRect	= QRect();
	QString	text	= UTF8TOQSTRING (cmdText);
	value	= index.data(Qt::DisplayRole);
	QRect	checkRect;
	Qt::CheckState	checkState	= Qt::Unchecked;
	int	width	= painter->fontMetrics ( ).width (text);
	int	height	= painter->fontMetrics ( ).height ( );
	QRect	displayRect (0, 0, width, height);
	doLayout(opt, &checkRect, &decorationRect, &displayRect, false);
	QBrush	oldBrush	= painter->background ( );
	painter->setBackground (brush);
	// drawBackground(painter, opt, index);
	if (Command::PROCESSING != status)
		painter->fillRect (option.rect, brush);
	else
	{
		QRect	fillRect (option.rect);
		fillRect.setWidth ((int)(progress * option.rect.width ( )));
		painter->fillRect (fillRect, brush);
	}
//	drawCheck(painter, opt, checkRect, checkState);
	drawDecoration(painter, opt, decorationRect, pixmap);
	drawDisplay(painter, opt, displayRect, text);
// CP : 2 FOLLOWING LINES TO UNCOMMENT
//	drawFocus(painter, opt, displayRect);
//	painter->setBackground (oldBrush);

	painter->restore ( );
}	// QtCommandItemDelegate::paint


// ===========================================================================
//                      LA CLASSE QtCommandMonitorPanel
// ===========================================================================

QtCommandMonitorPanel::QtCommandMonitorPanel (
									QWidget* parent, CommandManager* manager)
	: QtGroupBox ("", parent), ObjectBase ( ),
	  _commandListWidget (0),
	  _processingCheckBox (0), _cancelButton (0), _killButton (0), _selectButton(0), _selectMenu(0),
	  _commandManager (manager)
{
	assert (0 != manager);
	registerObservable (manager, false);

	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	setMargin (QtConfiguration::margin);
	setSpacing (QtConfiguration::spacing);

	_commandListWidget	= new QtCmdListWidget (this);
	_commandListWidget->setModelColumn (1);
	_commandListWidget->setSelectionMode (QAbstractItemView::SingleSelection);
	layout->addWidget (_commandListWidget);
	_commandListWidget->setWhatsThis ("Commandes.");
	_commandListWidget->setItemDelegate (
								new QtCommandItemDelegate (_commandListWidget));
	connect (_commandListWidget, SIGNAL (itemSelectionChanged ( )), this,
	         SLOT (updateGuiCallback ( )));

	// Les boutons :
	QWidget*		buttonsBox	= new QWidget (this);
	QHBoxLayout*	buttonsLayout	= new QHBoxLayout ( );
	buttonsBox->setLayout (buttonsLayout);
	layout->addWidget (buttonsBox);

	QLabel*	label0	= new QLabel (" ", buttonsBox);
	_processingCheckBox	= new QCheckBox ("Commandes en cours", buttonsBox);
	_processingCheckBox->setChecked (false);
	_processingCheckBox->setWhatsThis (QString::fromUtf8("Coché, ce panneau n'affiche que les commandes en cours d'exécution. Non coché, ce panneau affiche toutes les commandes."));
	connect (_processingCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (processingCallback ( )));

	QLabel*	label1	= new QLabel (" ", buttonsBox);
	_cancelButton	= new QPushButton ("Annuler", buttonsBox);
	connect (_cancelButton, SIGNAL(clicked ( )), this, 
	         SLOT (cancelCallback ( )));
	_cancelButton->setWhatsThis (QString::fromUtf8("Annule la commande en cours d'exécution."));
	QLabel*	label2	= new QLabel (" ", buttonsBox);
#ifdef ALLOW_KILLING_COMMAND
	_killButton	= new QPushButton ("Tuer", buttonsBox);
	_killButton->setWhatsThis (QString::fromUtf8("Tue la commande en cours d'exécution MAIS risque de laisser l'application dans un état instable. SAUVEGARDEZ votre travail dans un autre fichier avant de tuer la commande. A l'issue de la commande, on peut observer des fuites mémoires, plantages ou des deadlocks."));
	connect (_killButton, SIGNAL(clicked ( )), this, 
	         SLOT (killCallback ( )));
#endif	// ALLOW_KILLING_COMMAND
	QLabel*	label3	= new QLabel (" ", buttonsBox);

	_selectButton	= new QPushButton (QString::fromUtf8("Sélection ..."), buttonsBox);
	connect (_selectButton, SIGNAL(clicked ( )), this,
	         SLOT (selectCallback ( )));
	_selectButton->setWhatsThis (QString::fromUtf8("Sélection d'entités créées par la commande."));
	QLabel*	label4	= new QLabel (" ", buttonsBox);

	QPushButton* helpButton	= new QPushButton ("Aide ...", buttonsBox);
	connect (helpButton, SIGNAL(clicked ( )), this,
	         SLOT (helpCallback ( )));
	helpButton->setWhatsThis (QString::fromUtf8("Ouverture de la page web décrivant ce panneau."));
	QLabel*	label5	= new QLabel (" ", buttonsBox);

	_processingCheckBox->setFixedSize (_processingCheckBox->sizeHint ( ));
#ifdef ALLOW_KILLING_COMMAND
	QSize	sizeHints	=
		_cancelButton->sizeHint( ).width( ) > _killButton->sizeHint( ).width( )?
		_cancelButton->sizeHint ( ) : _killButton->sizeHint ( );
	_cancelButton->setFixedSize (sizeHints);
	_killButton->setFixedSize (sizeHints);
#endif	// ALLOW_KILLING_COMMAND
	buttonsLayout->addWidget (label0);
	buttonsLayout->addWidget (_processingCheckBox);
	buttonsLayout->addWidget (label1);
	buttonsLayout->addWidget (_cancelButton);
	buttonsLayout->addWidget (label2);
#ifdef ALLOW_KILLING_COMMAND
	buttonsLayout->addWidget (_killButton);
	buttonsLayout->addWidget (label3);
#endif	// ALLOW_KILLING_COMMAND
	buttonsLayout->addWidget (_selectButton);
	buttonsLayout->addWidget (label4);
	buttonsLayout->addWidget (helpButton);
	buttonsLayout->addWidget (label5);

	// menu pour bouton Sélection ...
	_selectMenu = new QMenu(_selectButton);

	QAction* selectGeomVolumesAction = new QAction(QString::fromUtf8("Sélectionne les volumes géométriques créés par la commande sélectionnée"), _selectMenu);
	_selectMenu->addAction(selectGeomVolumesAction);
	QObject::connect (selectGeomVolumesAction, SIGNAL (triggered ( )), this, SLOT (selectGeomVolumesCallback ( )));

	QAction* selectGeomSurfacesAction = new QAction(QString::fromUtf8("Sélectionne les surfaces géométriques créées par la commande sélectionnée"), _selectMenu);
	_selectMenu->addAction(selectGeomSurfacesAction);
	QObject::connect (selectGeomSurfacesAction, SIGNAL (triggered ( )), this, SLOT (selectGeomSurfacesCallback ( )));

	QAction* selectGeomCurvesAction = new QAction(QString::fromUtf8("Sélectionne les courbes géométriques créées par la commande sélectionnée"), _selectMenu);
	_selectMenu->addAction(selectGeomCurvesAction);
	QObject::connect (selectGeomCurvesAction, SIGNAL (triggered ( )), this, SLOT (selectGeomCurvesCallback ( )));

	QAction* selectGeomVerticesAction = new QAction(QString::fromUtf8("Sélectionne les sommets géométriques créés par la commande sélectionnée"), _selectMenu);
	_selectMenu->addAction(selectGeomVerticesAction);
	QObject::connect (selectGeomVerticesAction, SIGNAL (triggered ( )), this, SLOT (selectGeomVerticesCallback ( )));

	QAction* selectTopoBlocksAction = new QAction(QString::fromUtf8("Sélectionne les blocs topologiques créés par la commande sélectionnée"), _selectMenu);
	_selectMenu->addAction(selectTopoBlocksAction);
	QObject::connect (selectTopoBlocksAction, SIGNAL (triggered ( )), this, SLOT (selectTopoBlocksCallback ( )));

	QAction* selectTopoCoFacesAction = new QAction(QString::fromUtf8("Sélectionne les faces topologiques créées par la commande sélectionnée"), _selectMenu);
	_selectMenu->addAction(selectTopoCoFacesAction);
	QObject::connect (selectTopoCoFacesAction, SIGNAL (triggered ( )), this, SLOT (selectTopoCoFacesCallback ( )));

	QAction* selectTopoCoEdgesAction = new QAction(QString::fromUtf8("Sélectionne les arêtes topologiques créées par la commande sélectionnée"), _selectMenu);
	_selectMenu->addAction(selectTopoCoEdgesAction);
	QObject::connect (selectTopoCoEdgesAction, SIGNAL (triggered ( )), this, SLOT (selectTopoCoEdgesCallback ( )));

	QAction* selectTopoVerticesAction = new QAction(QString::fromUtf8("Sélectionne les sommets topologiques créés par la commande sélectionnée"), _selectMenu);
	_selectMenu->addAction(selectTopoVerticesAction);
	QObject::connect (selectTopoVerticesAction, SIGNAL (triggered ( )), this, SLOT (selectTopoVerticesCallback ( )));

	QAction* selectMeshVolumesAction = new QAction(QString::fromUtf8("Sélectionne les volumes de mailles créés par la commande sélectionnée"), _selectMenu);
	_selectMenu->addAction(selectMeshVolumesAction);
	QObject::connect (selectMeshVolumesAction, SIGNAL (triggered ( )), this, SLOT (selectMeshVolumesCallback ( )));

	QAction* selectMeshSurfacesAction = new QAction(QString::fromUtf8("Sélectionne les surfaces de mailles créées par la commande sélectionnée"), _selectMenu);
	_selectMenu->addAction(selectMeshSurfacesAction);
	QObject::connect (selectMeshSurfacesAction, SIGNAL (triggered ( )), this, SLOT (selectMeshSurfacesCallback ( )));

	QAction* selectMeshCloudsAction = new QAction(QString::fromUtf8("Sélectionne les nuages de noeuds créés par la commande sélectionnée"), _selectMenu);
	_selectMenu->addAction(selectMeshCloudsAction);
	QObject::connect (selectMeshCloudsAction, SIGNAL (triggered ( )), this, SLOT (selectMeshCloudsCallback ( )));


	// On affiche les commandes :
	processingCallback ( );
	updateGuiCallback ( );

	connect (this, SIGNAL (commandModified ( )), this,
	         SLOT (updateGuiCallback ( )));
	connect (this, SIGNAL (commandDeleted (Mgx3D::Utils::Command*)), this,
	         SLOT (commandDeletedCallback (Mgx3D::Utils::Command*)));
}	// QtCommandMonitorPanel::QtCommandMonitorPanel


QtCommandMonitorPanel::QtCommandMonitorPanel (
			const QtCommandMonitorPanel&)
	: QtGroupBox ("", 0), ObjectBase ( ),
	  _commandListWidget (0),
	  _processingCheckBox (0), _cancelButton (0), _killButton (0), _selectButton(0), _selectMenu(0),
	  _commandManager (0)
{
	MGX_FORBIDDEN ("QtCommandMonitorPanel::QtCommandMonitorPanel is not alllowed.")
}	// QtCommandMonitorPanel::QtCommandMonitorPanel


QtCommandMonitorPanel& QtCommandMonitorPanel::operator = (
		const QtCommandMonitorPanel&)
{
	MGX_FORBIDDEN ("QtCommandMonitorPanel::operator = is not alllowed.")
	return *this;
}	// QtCommandMonitorPanel::operator =


QtCommandMonitorPanel::~QtCommandMonitorPanel ( )
{
	_commandListWidget->setItemDelegate (0);
	release ( );
/*
	unregisterReferences ( );
	if (0 != _commandManager)
	{
		AutoReferencedMutex	arm (_commandManager->getMutex ( ));

		unregisterObservable (_commandManager, false);
		_commandManager	= 0;
	}	// if (0 != _commandManager)
*/
}	// QtCommandMonitorPanel::~QtCommandMonitorPanel


void QtCommandMonitorPanel::release ( )
{
	unregisterReferences ( );

	if (0 != _commandListWidget)
		_commandListWidget->clear ( );

	if (0 != _commandManager)
	{
		AutoReferencedMutex	arm (_commandManager->getMutex ( ));

		unregisterObservable (_commandManager, false);
		_commandManager	= 0;
	}	// if (0 != _commandManager)
}	// QtCommandMonitorPanel::release


Utils::Command* QtCommandMonitorPanel::getSelectedCommand ( )
{
	assert ((0 != _commandListWidget) && "QtCommandMonitorPanel::getSelectedCommand : null command list view.");

	QList<QListWidgetItem*>	selection	= _commandListWidget->selectedItems( );
	int						i	= 0;
	if (selection.count ( ) > 1)
		throw Exception (UTF8String ("Moniteur de commandes : plus d'une commande est sélectionnée.", Charset::UTF_8));
	for (QList<QListWidgetItem*>::iterator it = selection.begin ( );
	     it != selection.end ( ); it++, i++)
	{
		QListWidgetItem*	item	= *it;
		QtCommandItem*	commandItem	= dynamic_cast<QtCommandItem*>(*it);
		CHECK_NULL_PTR_ERROR (commandItem)
		return &commandItem->getCommand ( );
	}

	return 0;
}	// QtCommandMonitorPanel::getSelectedCommand


const Utils::Command* QtCommandMonitorPanel::getSelectedCommand ( ) const
{
	assert ((0 != _commandListWidget) && "QtCommandMonitorPanel::getSelectedCommand : null command list view.");

	QList<QListWidgetItem*>	selection	= _commandListWidget->selectedItems( );
	if (selection.count ( ) > 1)
		throw Exception (UTF8String ("Moniteur de commandes : plus d'une commande est sélectionnée.", Charset::UTF_8));

	// Code suite passage coverity :
	QtCommandItem*		commandItem	= dynamic_cast<QtCommandItem*>(*selection.begin ( ));
	CHECK_NULL_PTR_ERROR (commandItem)
	return &commandItem->getCommand ( );
/*
	int						i	= 0;
	for (QList<QListWidgetItem*>::iterator it = selection.begin ( );
	     it != selection.end ( ); it++, i++)
	{
		QListWidgetItem*	item	= *it;
		QtCommandItem*	commandItem	= dynamic_cast<QtCommandItem*>(*it);
		CHECK_NULL_PTR_ERROR (commandItem)
		return &commandItem->getCommand ( );
	}

	return 0;
*/
}	// QtCommandMonitorPanel::getSelectedCommand


void QtCommandMonitorPanel::observableModified (
								ReferencedObject* object, unsigned long event)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	LOCK_INSTANCE

	ReferencedNamedObject*	rno	= dynamic_cast<ReferencedNamedObject*>(object);
	CommandManager*	manager	= dynamic_cast<CommandManager*>(object);
	if (0 != manager)
	{
		const vector<Command*>	commands	= _commandManager->getCommands ( );
		switch (event)
		{
			case NEW_COMMAND	:
				Command*	command	= commands [commands.size ( ) - 1];
				registerObservable (command, false);
				if (true == shouldDisplayCommand (*command))
					addCommand (command);
				break;
		}	// switch (event)

		return;
	}	// if (0 != manager)

	CHECK_NULL_PTR_ERROR (_commandListWidget)
	Command*	command	= dynamic_cast<Command*>(object);
	if ((0 != command) && (COMMAND_STATE == event))
	{	// Faut-il ajouter/enlever la commande de la liste ?
		if ((Command::DONE == command->getStatus ( )) &&
		    (false == shouldDisplayCommand (*command)))
			emit commandDeleted (command);
//			_commandListWidget->deleteCommandItem (command);
		else if ((Command::PROCESSING == command->getStatus ( )) &&
		         (false == shouldDisplayAll ( )) &&
		         (false == _commandListWidget->hasCommand (command)))
			addCommand (command);

		emit commandModified ( );	// => updateGuiCallback dans le bon thread
	}	// if ((0 != command) && (COMMAND_STATE == event))
	else if ((0 != command) && (UTIL_UNAVAILABLE_EVENT == event))
	{
		observableDeleted (command);
	}	// else if ((0 != command) && (UTIL_UNAVAILABLE_EVENT == event))

//	updateGuiCallback ( );	// NON, mauvais thread

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")
}	// QtCommandMonitorPanel::observableModified


void QtCommandMonitorPanel::observableDeleted (ReferencedObject* object)
{
	ReferencedNamedObject*	rno	= dynamic_cast<ReferencedNamedObject*>(object);
#ifdef _DEBUG
	if (0 != rno)
		cout << "QtCommandMonitorPanel::observableDeleted for " << rno->getName ( ) << endl;
#endif
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_commandListWidget)

	LOCK_INSTANCE

	Command*	command	= dynamic_cast<Command*>(object);
	if (0 != command)
	{
		unregisterObservable (command, false);
		_commandListWidget->deleteCommandItem (command);
	}	// if (0 != command)

	emit commandDeleted (command);	// => updateGuiCallback dans le bon thread
//	updateGuiCallback ( );

	COMPLETE_QT_TRY_CATCH_BLOCK  (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")
}	// QtCommandMonitorPanel::observableDeleted


void QtCommandMonitorPanel::addCommand (Command* command)
{
	CHECK_NULL_PTR_ERROR (command)
	CHECK_NULL_PTR_ERROR (_commandListWidget)

	LOCK_INSTANCE

	AutoReferencedMutex	arm (command->getMutex ( ));
	QListWidgetItem*	item	=
							new QtCommandItem (*_commandListWidget, command);
//	_commandListWidget->addCommandItem (item);
}	// QtCommandMonitorPanel::addCommand


bool QtCommandMonitorPanel::shouldDisplayCommand (const Command& command) const
{
	CHECK_NULL_PTR_ERROR (_processingCheckBox)
	if (false == _processingCheckBox->checkState ( ))
		return true;

	switch (command.getStatus ( ))
	{
		case Command::PROCESSING	:
		case Command::CANCELED		:
		case Command::FAIL			:
			break;
		case Command::INITED		:
        case Command::STARTING      :
		case Command::DONE			:
		default						:
			return false;
	}	// switch (command.getStatus ( ))

	return true;
}	// QtCommandMonitorPanel::shouldDisplayCommand


bool QtCommandMonitorPanel::shouldDisplayAll ( ) const
{
	CHECK_NULL_PTR_ERROR (_processingCheckBox)
	return !_processingCheckBox->checkState ( );
}	// QtCommandMonitorPanel::shouldDisplayAll


void QtCommandMonitorPanel::updateGuiCallback ( )
{
	assert (0 != _cancelButton);
	assert (0 != _selectButton);
#ifdef ALLOW_KILLING_COMMAND
	assert (0 != _killButton);
#endif	// ALLOW_KILLING_COMMAND

	BEGIN_QT_TRY_CATCH_BLOCK

	Command*	command			= getSelectedCommand ( );
	bool		enableCancel	= false, enableKill	= false, enableSelect = false;
	if (0 != command)
	{
		switch (command->getStatus ( ))
		{
			case Command::DONE			:
				enableSelect = true;
				break;
			case Command::CANCELED		:
			case Command::FAIL			:
				break;
			default						: // => INITED ou PROCESSING
				enableCancel	= command->cancelable ( );
				enableKill		= 0==command->getCommandRunner ( ) ? false:true;
		}	// switch (command->getStatus ( ))
	}	// if (0 != command)

	// CP : COMMENTER LES LIGNES CI-DESSOUS, MAUVAIS THREAD (03/02/17) ?
	_cancelButton->setEnabled (enableCancel);	// CP : A CONFIRMER
	_selectButton->setEnabled (enableSelect);
#ifdef ALLOW_KILLING_COMMAND
	_killButton->setEnabled (enableKill);		// CP : A CONFIRMER
#endif	// ALLOW_KILLING_COMMAND

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")
}	// QtCommandMonitorPanel::updateGuiCallback


void QtCommandMonitorPanel::commandDeletedCallback (Command* command)
{
	assert (0 != _commandListWidget);

	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != command)
		_commandListWidget->deleteCommandItem (command);
	updateGuiCallback ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")
}	// QtCommandMonitorPanel::commandDeletedCallback


void QtCommandMonitorPanel::cancelCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Command*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)
	if (false == command->cancelable ( ))	// Eviter un dead-lock
		throw Exception (UTF8String ("La commande n'est pas annulable.", Charset::UTF_8));

	command->cancel ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")
}	// QtCommandMonitorPanel::cancelCallback


void QtCommandMonitorPanel::killCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

#ifdef ALLOW_KILLING_COMMAND
	Command	*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)

	if (Command::PROCESSING != command->getStatus ( ))
	{
		UTF8String	error (Charset::UTF_8);
		error << "La commande \"" << command->getName ( )
		      << "\" n'est pas en cours d'exécution (état courant : "
		      << command->getStrStatus ( )
		      << "), et ne peut pas de ce fait être tuée.";
		throw Exception (error);
	}	// if (Command::PROCESSING != command->getStatus( ))

	CommandRunner*	runner	= command->getCommandRunner ( );
	if (0 == runner)
	{
		UTF8String	error (Charset::UTF_8);
		error << "La commande\" " << command->getName ( )
		      << "\" n'est pas exécutée dans un thread spécifique, et un "
		      << " tel thread ne peut de ce fait être détruit.";
		throw Exception (error);
	}	// if (0 == runner)

	runner->cancel ( );
	// Le thread est mort ! Lors de son agonie il est supposé avoir informé
	// ses utilisateurs qu'il ne fallait plus compter sur lui. Le menu, au
	// niveau de l'IHM, est a priori réactivé par ce biais.
	// A priori le thread, au sens instance c++ du terme, n'est pas détruit,
	// (il y a des risques selon la façon dont il a été annulé), mais plus
	// personne ne pointe sur lui. De même la commande n'a pas eu le temps
	// d'être enregistrée car elle n'a pas été terminée avec succès.
#endif	// ALLOW_KILLING_COMMAND

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")
}	// QtCommandMonitorPanel::killCallback


void QtCommandMonitorPanel::selectCallback ( )
{
	assert ( 0 != _selectMenu );

	BEGIN_QT_TRY_CATCH_BLOCK

	_selectMenu->popup(QCursor::pos());

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")
}	// QtCommandMonitorPanel::selectCallback


void QtCommandMonitorPanel::selectGeomVolumesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Command	*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)

	Internal::CommandInternal* cmdInt = dynamic_cast<Internal::CommandInternal*>(command);
	CHECK_NULL_PTR_ERROR(cmdInt)

	Internal::M3DCommandResult cmdResult(*cmdInt);
	std::vector<Geom::Volume*> objs = cmdResult.getVolumesObj();

	vector<Entity*>	entities;
	entities.insert(entities.end(), objs.begin(), objs.end());
	getStdContext ( )->getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")

} // QtCommandMonitorPanel::selectGeomVolumesCallback


void QtCommandMonitorPanel::selectGeomSurfacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Command	*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)

	Internal::CommandInternal* cmdInt = dynamic_cast<Internal::CommandInternal*>(command);
	CHECK_NULL_PTR_ERROR(cmdInt)

	Internal::M3DCommandResult cmdResult(*cmdInt);
	std::vector<Geom::Surface*> objs = cmdResult.getSurfacesObj();

	vector<Entity*>	entities;
	entities.insert(entities.end(), objs.begin(), objs.end());
	getStdContext ( )->getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")

} // QtCommandMonitorPanel::selectGeomSurfacesCallback


void QtCommandMonitorPanel::selectGeomCurvesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Command	*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)

	Internal::CommandInternal* cmdInt = dynamic_cast<Internal::CommandInternal*>(command);
	CHECK_NULL_PTR_ERROR(cmdInt)

	Internal::M3DCommandResult cmdResult(*cmdInt);
	std::vector<Geom::Curve*> objs = cmdResult.getCurvesObj();

	vector<Entity*>	entities;
	entities.insert(entities.end(), objs.begin(), objs.end());
	getStdContext ( )->getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")

} // QtCommandMonitorPanel::selectGeomCurvesCallback


void QtCommandMonitorPanel::selectGeomVerticesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Command	*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)

	Internal::CommandInternal* cmdInt = dynamic_cast<Internal::CommandInternal*>(command);
	CHECK_NULL_PTR_ERROR(cmdInt)

	Internal::M3DCommandResult cmdResult(*cmdInt);
	std::vector<Geom::Vertex*> objs = cmdResult.getVerticesObj();

	vector<Entity*>	entities;
	entities.insert(entities.end(), objs.begin(), objs.end());
	getStdContext ( )->getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")

} // QtCommandMonitorPanel::selectGeomVerticesCallback


void QtCommandMonitorPanel::selectTopoBlocksCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Command	*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)

	Internal::CommandInternal* cmdInt = dynamic_cast<Internal::CommandInternal*>(command);
	CHECK_NULL_PTR_ERROR(cmdInt)

	Internal::M3DCommandResult cmdResult(*cmdInt);
	std::vector<Topo::Block*> objs = cmdResult.getBlocksObj();

	vector<Entity*>	entities;
	entities.insert(entities.end(), objs.begin(), objs.end());
	getStdContext ( )->getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")

} // QtCommandMonitorPanel::selectTopoBlocksCallback


void QtCommandMonitorPanel::selectTopoCoFacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Command	*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)

	Internal::CommandInternal* cmdInt = dynamic_cast<Internal::CommandInternal*>(command);
	CHECK_NULL_PTR_ERROR(cmdInt)

	Internal::M3DCommandResult cmdResult(*cmdInt);
	std::vector<Topo::CoFace*> objs = cmdResult.getFacesObj();

	vector<Entity*>	entities;
	entities.insert(entities.end(), objs.begin(), objs.end());
	getStdContext ( )->getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")

} // QtCommandMonitorPanel::selectTopoCoFacesCallback


void QtCommandMonitorPanel::selectTopoCoEdgesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Command	*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)

	Internal::CommandInternal* cmdInt = dynamic_cast<Internal::CommandInternal*>(command);
	CHECK_NULL_PTR_ERROR(cmdInt)

	Internal::M3DCommandResult cmdResult(*cmdInt);
	std::vector<Topo::CoEdge*> objs = cmdResult.getEdgesObj();

	vector<Entity*>	entities;
	entities.insert(entities.end(), objs.begin(), objs.end());
	getStdContext ( )->getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")

} // QtCommandMonitorPanel::selectTopoCoEdgesCallback


void QtCommandMonitorPanel::selectTopoVerticesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Command	*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)

	Internal::CommandInternal* cmdInt = dynamic_cast<Internal::CommandInternal*>(command);
	CHECK_NULL_PTR_ERROR(cmdInt)

	Internal::M3DCommandResult cmdResult(*cmdInt);
	std::vector<Topo::Vertex*> objs = cmdResult.getTopoVerticesObj();

	vector<Entity*>	entities;
	entities.insert(entities.end(), objs.begin(), objs.end());
	getStdContext ( )->getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")

} // QtCommandMonitorPanel::selectTopoVerticesCallback


void QtCommandMonitorPanel::selectMeshVolumesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Command	*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)

	Internal::CommandInternal* cmdInt = dynamic_cast<Internal::CommandInternal*>(command);
	CHECK_NULL_PTR_ERROR(cmdInt)

	Internal::M3DCommandResult cmdResult(*cmdInt);
	std::vector<Mesh::Volume*> objs = cmdResult.getMeshVolumesObj();

	vector<Entity*>	entities;
	entities.insert(entities.end(), objs.begin(), objs.end());
	getStdContext ( )->getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")

} // QtCommandMonitorPanel::selectMeshVolumesCallback


void QtCommandMonitorPanel::selectMeshSurfacesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Command	*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)

	Internal::CommandInternal* cmdInt = dynamic_cast<Internal::CommandInternal*>(command);
	CHECK_NULL_PTR_ERROR(cmdInt)

	Internal::M3DCommandResult cmdResult(*cmdInt);
	std::vector<Mesh::Surface*> objs = cmdResult.getMeshSurfacesObj();

	vector<Entity*>	entities;
	entities.insert(entities.end(), objs.begin(), objs.end());
	getStdContext ( )->getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")

} // QtCommandMonitorPanel::selectMeshSurfacesCallback


void QtCommandMonitorPanel::selectMeshCloudsCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	Command	*	command	= getSelectedCommand ( );
	CHECK_NULL_PTR_ERROR (command)

	Internal::CommandInternal* cmdInt = dynamic_cast<Internal::CommandInternal*>(command);
	CHECK_NULL_PTR_ERROR(cmdInt)

	Internal::M3DCommandResult cmdResult(*cmdInt);
	std::vector<Mesh::Cloud*> objs = cmdResult.getMeshCloudsObj();

	vector<Entity*>	entities;
	entities.insert(entities.end(), objs.begin(), objs.end());
	getStdContext ( )->getSelectionManager ( ).addToSelection (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")

} // QtCommandMonitorPanel::selectMeshCloudsCallback


void QtCommandMonitorPanel::processingCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	LOCK_INSTANCE

	unregisterReferences ( );
	registerObservable (_commandManager, false);

	// On enlève les commandes affichées :
	CHECK_NULL_PTR_ERROR (_commandListWidget)
	_commandListWidget->clear ( );

	// On affiche les commandes souhaitées :
	const vector<Command*>	commands	= _commandManager->getCommands ( );
	for (vector<Command*>::const_iterator itc = commands.begin ( );
	     commands.end ( ) != itc; itc++)
		if (true == shouldDisplayCommand (**itc))
		{
			addCommand (*itc);
			registerObservable (*itc, false);
		}

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, "Magix 3D - Moniteur de commandes")
}	// QtCommandMonitorPanel::processingCallback


void QtCommandMonitorPanel::helpCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtHelpWindow::displayURL (QtMgx3DApplication::HelpSystem::instance ( ).monitorPanelURL,
			QtMgx3DApplication::HelpSystem::instance ( ).monitorPanelTag );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, "Magix 3D : affichage de l'aide.")

} // QtCommandMonitorPanel::helpCallback



// ===========================================================================
//                          LA CLASSE QtCommandItem
// ===========================================================================

QtCommandItem::QtCommandItem (QtCmdListWidget& parent, Command* command)
	: QListWidgetItem ( ), ObjectBase ( ), _command (command),
	  _commandPtr (command)
{
	LOCK_INSTANCE

	parent.addCommandItem (this);
	if (0 != _command)
	{
		registerObservable (_command, false);
		observableModified (_command, UTIL_UNDEFINED_EVENT);
	}	// if (0 != _command)
}	// QtCommandItem::QtCommandItem


QtCommandItem::QtCommandItem (const QtCommandItem&)
	: QListWidgetItem ( ), ObjectBase ( ), _command (0), _commandPtr (0)
{
	assert (0 && "QtCommandItem copy constructor is not allowed.");
}	// QtCommandItem::QtCommandItem


QtCommandItem& QtCommandItem::operator = (const QtCommandItem&)
{
	assert (0 && "QtCommandItem assignment operator is not allowed.");
	return *this;
}	// QtCommandItem::operator =


QtCommandItem::~QtCommandItem ( )
{
	unregisterReferences ( );
}	// QtCommandItem::~QtCommandItem


void QtCommandItem::observableModified (
								ReferencedObject* object, unsigned long event)
{
	if (0 == object)
		return;

	LOCK_INSTANCE

	Command*	command	= dynamic_cast<Command*>(object);
	if ((UTIL_UNAVAILABLE_EVENT == event) && (command == _command))
	{	// thread annulé ?
		observableDeleted (object);
		return;
	}	// if ((UTIL_UNAVAILABLE_EVENT == event) && (command == _command))

	// On ne peut pas mettre à jour l'item ici car on est peut être dans un
	// thread autre que celui du Widget, ce qui pourrait provoquer un plantage
	// => emission d'un signal en vue de provoquer l'actualisation dans le
	// thread du widget :
	QtCmdListWidget*	parent	= dynamic_cast<QtCmdListWidget*>(listWidget( ));
	if (0 != parent)
	{
		parent->emitModified (parent->index (this));
		return;
	}	// if (0 != parent)
}	// QtCommandItem::observableModified


void QtCommandItem::observableDeleted (ReferencedObject* object)
{
	if (0 == object)
		return;

	LOCK_INSTANCE

	Command*	command	= dynamic_cast<Command*>(object);
	if (command == _command)
	{
		_command	= 0;
		setBackground (QBrush (Qt::white));
		setText ("");
	}	// if (command == object)
}	// QtCommandItem::observableDeleted


Command& QtCommandItem::getCommand ( )
{
	LOCK_INSTANCE

	if (0 == _command)
		throw Exception (UTF8String ("QtCommandItem::getCommand : null command.", Charset::UTF_8));

	return *_command;
}	// QtCommandItem::getCommand


const Command& QtCommandItem::getCommand ( ) const
{
	LOCK_INSTANCE

	if (0 == _command)
		throw Exception (UTF8String ("QtCommandItem::getCommand : null command.", Charset::UTF_8));

	return *_command;
}	// QtCommandItem::getCommand

}	// namespace QtComponents

}	// namespace Mgx3D
