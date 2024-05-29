/**
 * \file		QtEntityIDTextField.cpp
 * \author		Charles PIGNEROL
 * \date		11/09/2013
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/QtEntityIDTextField.h"
#include "QtComponents/QtMgx3DMainWindow.h"

#include <Utils/Common.h>

#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/WarningLog.h>

#include <QtUtil/QtObjectSignalBlocker.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <QKeyEvent>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


QtEntityIDTextField::QtEntityIDTextField (
	QWidget* parent, QtMgx3DMainWindow* mainWindow,
	SelectionManagerIfc::DIM dimensions,
	FilterEntity::objectType types, char* name)
	: QtTextField (parent, name),
	  EntitySeizureManager (mainWindow, dimensions, types),
	  _mainWindow (mainWindow), _addGraphicalSelectionAction (0), _addGroupsEntitiesAction (0),
	  _backgroundColor (Qt::white), _updating (false)
{
	QPalette	p		= palette ( );
	_backgroundColor	= p.color (QPalette::Active, QPalette::Base);
	// 1er callback à exécuter : filterText, le texte est peut-être issu de
	// copier/coller avec la console python, il faut alors enlever des ,, ', ",
	// ...
	connect (this, SIGNAL (editingFinished ( )), this,
	         SLOT (filterText ( )));
	connect (this, SIGNAL (returnPressed ( )), this,
	         SLOT (returnPressedCallback ( )));
	connect (this, SIGNAL (editingFinished ( )), this,
	         SLOT (textModifiedCallback ( )));

	// Le menu contextuel :
	_addGraphicalSelectionAction	= new QAction (QString::fromUtf8 ("Ajouter la sélection"), this);
	_addGraphicalSelectionAction->setToolTip (QString::fromUtf8 ("Ajoute à cette liste les entités sélectionnées de la fenêtre graphique"));
	connect (_addGraphicalSelectionAction, SIGNAL (triggered ( )), this, SLOT (addGraphicalSelectionCallback ( )));
// [EB] ne fonctionne pas correctement car on pourrait s'attendre à avoir les blocs également alors que ces derniers ne sont pas dans les groupes en général.
// Rappel: on a en général des volumes dans les groupes et des blocs sont liés à des volumes.
//	_addGroupsEntitiesAction	= new QAction (QString::fromUtf8 ("Ajouter les groupes sélectionnés"), this);
//	_addGroupsEntitiesAction->setToolTip (QString::fromUtf8 ("Ajoute à cette liste les entités des groupes sélectionnées dans le panneau \"Groupes\""));
//	connect (_addGroupsEntitiesAction, SIGNAL (triggered ( )), this, SLOT (addSelectedGroupsEntitiesCallback ( )));
}	// QtEntityIDTextField::QtEntityIDTextField


QtEntityIDTextField::QtEntityIDTextField (const QtEntityIDTextField&)
	: QtTextField (0, 0),
	  EntitySeizureManager (
				0, SelectionManagerIfc::NO_DIM, FilterEntity::NoneEntity),
	_mainWindow (0), _addGraphicalSelectionAction (0), _addGroupsEntitiesAction (0),
	_backgroundColor (Qt::white), _updating (false)
{
	MGX_FORBIDDEN ("QtEntityIDTextField copy constructor is forbidden.")
}	// QtEntityIDTextField::QtEntityIDTextField


QtEntityIDTextField& QtEntityIDTextField::operator = (
													const QtEntityIDTextField&)
{
	MGX_FORBIDDEN ("QtEntityIDTextField assignment operator is forbidden.")
	return *this;
}	// QtEntityIDTextField::operator =


QtEntityIDTextField::~QtEntityIDTextField ( )
{
}	// QtEntityIDTextField::~QtEntityIDTextField


bool QtEntityIDTextField::actualizeGui (bool removeDestroyed)
{
	// Actualisation du texte affiché :
	bool			modified	= false;
	bool			first		= true;
	vector<string>	namesList	= getUniqueNames ( );
	UTF8String		names (Charset::UTF_8);
	for (vector<string>::iterator it = namesList.begin ( );
	     namesList.end ( ) != it; it++)
	{
		try
		{
			Entity&	entity = getContext ( ).nameToEntity (*it);
			if ((true == entity.isDestroyed ( )) && (true== removeDestroyed))
			{
				modified	= true;
			}	// if ((true == entity.isDestroyed ( )) && ...
			else
			{	// L'entité peut avoir été détruite, et son ID réaffecté à une
				// entité d'un autre type.
				if (true == criteriaMatch (
								entity.getDim ( ), entity.getFilteredType ( )))
				{
					if (false == first)
						names << ' ';
					else
						first	= false;
					names << *it;
				}	// if (true == criteriaMatch ( ...
				else
					modified	= true;
			}	// if ((true == entity.isDestroyed ( )) ...
		}
		catch (...)
		{
			modified	= true;	// Realy destroyed !
		}
	}	// for (vector<string>::iterator it = namesList.begin ( ); ...
	if (true == modified)
	{
		QtObjectSignalBlocker	blocker (this);
		setText (UTF8TOQSTRING (names));
	}	// if (true == modified)

	names.clear ( );		first	= true;
	modified	= EntitySeizureManager::actualizeGui (removeDestroyed);
	if (true == modified)
	{
		const vector<Entity*>	entities	= getSelectedEntities ( );
		for (vector<Entity*>::const_iterator it = entities.begin ( );
		     entities.end ( ) != it; it++)
		{
			if (false == first)
				names << ' ';
			else
				first	= false;
			names << (*it)->getName ( );
		}	// for (vector<Entity*>::const_iterator it = entities.begin ( ); ...

		QtObjectSignalBlocker	blocker (this);
		setText (UTF8TOQSTRING (names));
	}	// if (true == modified)

	return modified;
}	// QtEntityIDTextField::actualizeGui


void QtEntityIDTextField::setInteractiveMode (bool enable)
{
	try
	{
		// EntitySeizureManager::setInteractiveMode va appeler addToSelection et autres => ne pas rajouter le nom des
		// entités pré-sélectionnées à la liste en cours.
		_updating	= true;
		EntitySeizureManager::setInteractiveMode (enable);
		_updating	= false;
	}
	catch (...)
	{
		_updating	= false;
		throw;
	}

	try
	{

/* Même en VTK 7 ce signal ne semble pas émis par le QVTKWidget. A confirmer.
	if (true == enable)
		connect (getMainWindow ( ).getGraphicalWidget ( ).getRenderingWidget( ),
		         SIGNAL (keyRelease (QKeyEvent*)), this, SLOT (keyReleaseCallback (QKeyEvent*)));
	else
		disconnect (
			getMainWindow ( ).getGraphicalWidget ( ).getRenderingWidget( ),
		         SIGNAL (keyRelease (QKeyEvent*)), this, SLOT (keyReleaseCallback (QKeyEvent*))); */

	}
	catch (...)
	{	// En phase de construction getGraphicalWidget ( ) est nul.
	}
}	// QtEntityIDTextField::setInteractiveMode


vector<string> QtEntityIDTextField::getUniqueNames ( ) const
{
	static const string	space (" ");
	vector<string>		namesList;
	const string		names	= getText ( );
	string::size_type	current	= 0, end	= names.find (' ');

	while ((string::size_type)-1 != end)
	{
		const string	name	= names.substr (current, end - current);
		// Il arrive que des espaces soient doublés lorsqu'une entité est
		// enlevée de la sélection. La suppression de l'espace pourrait être
		// fait à ce moment, mais n'écarte pas une erreur de saisie manuelle de
		// l'utilisateur qui lèvera une exception pas forcément comprise. Donc
		// on teste ici ce cas :
		if (false == name.empty ( ))
			namesList.push_back (name);//names.substr (current, end - current));
		current	= end + 1;
		end		= names.find (' ', current);
	}   // while ((string::size_type)-1 != end)
	if (current < names.length ( ))
		namesList.push_back (names.substr (current));

	return namesList;
}	// QtEntityIDTextField::getUniqueNames


void QtEntityIDTextField::setUniqueNames (const vector<string>& names)
{
	if (true == inInteractiveMode ( ))
		return;

	UTF8String		text (Charset::UTF_8);
	for (vector<string>::const_iterator it = names.begin ( );
	     names.end ( ) != it; it++)
	{
		try
		{
			Entity*	entity	= &getContext().nameToEntity (*it);
			if (0 != text.length ( ))
				text << ' ';
			if (0 != (*it).length ( ))
				text << *it;
		}
		catch (...)
		{
		}
	}	// for (vector<string>::const_iterator it = names.begin ( ); ...

	setText (UTF8TOQSTRING (text.trim ( )));
}	// QtEntityIDTextField::setUniqueNames


void QtEntityIDTextField::reset ( )
{
	try
	{
		setText (QString (""));
		EntitySeizureManager::reset ( );
	}
	catch (...)
	{
	}
}	// QtEntityIDTextField::reset


void QtEntityIDTextField::entitiesAddedToSelection (const vector<string>& names)
{
	bool	modified	= false;
	QStringList	list	= text ( ).split (' ', QString::SkipEmptyParts, Qt::CaseSensitive);
	for (vector<string>::const_iterator itun = names.begin ( ); names.end ( ) != itun; itun++)
	{
		QString	name ((*itun).c_str ( ));
		if (true == list.contains (name, Qt::CaseSensitive))
			continue;

		list.append (name);
		modified	= true;
	}	// for (vector<string>::const_iterator itun = names.begin ( ); ...

	if (true == modified)	// => Au moins un nom
	{
		QStringList::const_iterator itn	= list.begin ( );
		UTF8String	keptNames ((*itn).toStdString ( ), Charset::UTF_8);
		while (list.end ( ) != ++itn)
			keptNames << ' ' << (*itn).toStdString ( );

		setText (keptNames.utf8 ( ));
	}	// if (true == modified)
}	// QtEntityIDTextField::entitiesAddedToSelection


void QtEntityIDTextField::entitiesRemovedFromSelection (const vector<string>& names)
{
	bool	modified	= false;
	QStringList	list	= text ( ).split (' ', QString::SkipEmptyParts, Qt::CaseSensitive);
	for (vector<string>::const_iterator itun = names.begin ( ); names.end ( ) != itun; itun++)
	{
		const int	index	= list.indexOf (QString	((*itun).c_str ( )));
		if (-1 == index)
			continue;

		list.removeAt (index);
		modified	= true;
	}	// for (vector<string>::const_iterator itun = names.begin ( ); ...

	if (true == modified)	// => Au moins un nom
	{
		QStringList::const_iterator itn	= list.begin ( );
		UTF8String	keptNames ((*itn).toStdString ( ), Charset::UTF_8);
		while (list.end ( ) != ++itn)
			keptNames << ' ' << (*itn).toStdString ( );

		setText (keptNames.utf8 ( ));
	}	// if (true == modified)
}	// QtEntityIDTextField::entitiesRemovedFromSelection


void QtEntityIDTextField::clearSelection ( )
{
	const QString	selection (text ( ).trimmed ( ));

	setText (QString (""));
	clearSeizuredEntities ( );

	if (false == selection.isEmpty ( ))
	{
		emit entitiesRemovedFromSelection (selection);
	}	// if (false == selection.empty ( ))
}	// QtEntityIDTextField::clearSelection


void QtEntityIDTextField::addGraphicalSelectionCallback ( )
{
	try
	{
		if (0 == getMainWindow ( ).getSelectionManager ( ))
			return;

		const vector<Entity*>	selectedEntities	= getMainWindow ( ).getSelectionManager ( )->getEntities ( );
		vector<Entity*>		keptEntities;
		for (vector<Entity*>::const_iterator ite = selectedEntities.begin ( );
		     selectedEntities.end ( ) != ite; ite++)
			if (true == criteriaMatch ((*ite)->getDim ( ), (*ite)->getFilteredType ( )))
				keptEntities.push_back (*ite);
		if (0 != keptEntities.size ( ))
			addToSelection (keptEntities);
	}
	catch (...)
	{
	}
}	// QtEntityIDTextField::addGraphicalSelectionCallback


void QtEntityIDTextField::addSelectedGroupsEntitiesCallback ( )
{
cout << "QtEntityIDTextField::addSelectedGroupsEntitiesCallback called" << endl;
	try
	{
		vector<Group::GroupEntity*>	groups	= getMainWindow ( ).getGroupsPanel ( ).getSelectedGroups ( );
cout << "CURRENT SELECTION HAS " << groups.size ( ) << " GROUPS" << endl;
		vector<Entity*>	keptEntities;
		for (vector<Group::GroupEntity*>::const_iterator itg = groups.begin ( );
		     groups.end ( ) != itg; itg++)
		{
			vector<Utils::Entity*>	entities	= (*itg)->getEntities ( );
			for (vector<Entity*>::const_iterator ite = entities.begin ( );
		     		entities.end ( ) != ite; ite++)
				if (true == criteriaMatch ((*ite)->getDim ( ), (*ite)->getFilteredType ( )))
					keptEntities.push_back (*ite);
		}	// for (vector<Group::GroupEntity*>::const_iterator itg = groups.begin ( );

		if (0 != keptEntities.size ( ))
			addToSelection (keptEntities);
cout << "KEPT ENTIES COUNT IS " << keptEntities.size ( ) << endl;
	}
	catch (...)
	{
	}

}


void QtEntityIDTextField::hideEvent (QHideEvent* event)
{
	QtTextField::hideEvent (event);

	try
	{
		setInteractiveMode (false);
	}
	catch (...)
	{
	}
}	// QtEntityIDTextField::hide


void QtEntityIDTextField::keyPressEvent (QKeyEvent* event)
{
	try
	{
		if (Qt::Key_Escape == event->key ( ))
			setInteractiveMode (false);
		else
			QtTextField::keyPressEvent (event);
	}
	catch (...)
	{
	}
}	// QtEntityIDTextField::keyPressEvent


void QtEntityIDTextField::keyReleaseCallback (QKeyEvent* event)
{
	try
	{
		if (Qt::Key_Escape == event->key ( ))
			setInteractiveMode (false);
	}
	catch (...)
	{
	}
}	// QtEntityIDTextField::keyReleaseCallback


void QtEntityIDTextField::mousePressEvent (QMouseEvent* event)
{
	if (0 == event)	// Restructuré pour coverity
		return;

	// MGXDDD-262 : on ne retient que l'évènement bouton gauche seul => on
	// oublie par exemple "bouton du milieu' == presse-papier, ...
	if (Qt::LeftButton != event->buttons ( ))
	{
		QtTextField::mousePressEvent (event);
		return;
	}	// if ((0 != event) && (Qt::Qt::LeftButton != event->buttons ( )))

	try
	{
		bool	interactive	= !inInteractiveMode ( );
		QtTextField::mousePressEvent (event);	// Calls focusInEvent
		setInteractiveMode (interactive);
		event->accept ( );
	}
	catch (...)
	{
	}
}	// QtEntityIDTextField::mousePressEvent


void QtEntityIDTextField::contextMenuEvent (QContextMenuEvent* event)
{
	try
	{
		unique_ptr<QMenu> menu (createStandardContextMenu  ( ));
		QList<QAction*>	actions	= menu->actions ();
		QAction*	action	= 0 == actions.size ( ) ? 0 : actions [0];
		if ((true == inInteractiveMode ( )) && (true == isMultiSelectMode ( )))
		{
			if (0 != _addGraphicalSelectionAction)
			{
				if (0 == action)
					menu->addAction (_addGraphicalSelectionAction);
				else
					menu->insertAction (action, _addGraphicalSelectionAction);
			}	// if (0 != _addGraphicalSelectionAction)
			if (0 != _addGroupsEntitiesAction)
			{
				if (0 == action)
					menu->addAction (_addGroupsEntitiesAction);
				else
					menu->insertAction (action, _addGroupsEntitiesAction);
			}	// if (0 != _addGroupsEntitiesAction)

			if (0 != action)
				menu->insertSeparator (action);
		}	// if ((true == inInteractiveMode ( )) && (true == isMultiSelectMode ( )))

		menu->exec (event->globalPos ( ));
	}
	catch (...)
	{
	}
}	// QtEntityIDTextField::contextMenuEvent


void QtEntityIDTextField::updateGui (bool enable)
{
	QPalette	p	= palette ( );
	p.setColor (QPalette::Active, QPalette::Base,
		true == enable ?
			EntitySeizureManager::interactiveModeBackground : _backgroundColor);
	setPalette (p);
}	// QtEntityIDTextField::updateGui


void QtEntityIDTextField::setSelectedEntity (const Entity& entity)
{
	try
	{
		const vector<string>	names	= getUniqueNames ( );
		UTF8String				selectedEntities (Charset::UTF_8);
		bool			first	= true;
		for (vector<string>::const_iterator it = names.begin ( );
		     names.end ( ) != it; it++)
		{
			if (false == first)
				selectedEntities << ' ';
			else
				first	= false;
			selectedEntities << *it;
		}
		emit entitiesRemovedFromSelection (
				 UTF8TOQSTRING (selectedEntities));

		setText (entity.getName ( ));
		setModified (false);

		const QString	name	= getQText ( );
		emit entitiesAddedToSelection (name);
		emit selectionModified (name);
	}
	catch (...)
	{
	}
}	// QtEntityIDTextField::setSelectedEntity


void QtEntityIDTextField::setSelectedEntities (
							const std::vector<Mgx3D::Utils::Entity*> entities)
{
	try
	{
		const vector<string>	names	= getUniqueNames ( );
		UTF8String				selectedEntities (Charset::UTF_8);
		bool			first	= true;
		for (vector<Entity*>::const_iterator it = entities.begin ( );
		     entities.end ( ) != it; it++)
		{
			if (false == first)
				selectedEntities << ' ';
			else
				first	= false;
			selectedEntities << (*it)->getName ( );
		}
		emit entitiesRemovedFromSelection (
					UTF8TOQSTRING (selectedEntities));

		UTF8String   entityNames (Charset::UTF_8);
		for (uint i=0; i<entities.size ( ); i++)
			entityNames << entities[i]->getName ( ) <<" ";
		setText (entityNames.trim ( ).ascii());

		setModified (false);
		emit entitiesAddedToSelection (getQText ( ));
		emit selectionModified (getQText ( ));
	}
	catch (...)
	{
	}
}  // QtEntityIDTextField::setSelectedEntities


void QtEntityIDTextField::addToSelection (const vector<Entity*>& entities)
{
// CP : correctif MGXDDD-262 :
//	if ((true == _updating)	|| (0 == entities.size ( )))
//		return;	// cf. setInteractiveMode

	QString	selection	= getQText ( );

	UTF8String		selectedEntities (Charset::UTF_8);
	bool			first	= true;
	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		selection	= selection + ' ' + (*it)->getName ( ).c_str ( );
		if (false == first)
			selectedEntities << ' ';
		else
			first	= false;
		selectedEntities << (*it)->getName ( );
	}
	if ((false == _updating) && (0 != entities.size ( )))	// CP : correctif MGXDDD-262
		setText (selection.trimmed ( ));

	emit entitiesAddedToSelection (UTF8TOQSTRING (selectedEntities));
}	// QtEntityIDTextField::addToSelection


void QtEntityIDTextField::removeFromSelection (const vector<Entity*>& entities)
{
	if ((true == _updating)	|| (0 == entities.size ( )))
		return;	// cf. setInteractiveMode

	UTF8String		unselectedEntities (Charset::UTF_8);
	bool			first		= true;
	QString			selection	= getQText ( );
	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		selection.remove ((*it)->getName ( ).c_str ( ), Qt::CaseSensitive);
		if (false == first)
			unselectedEntities << ' ';
		else
			first	= false;
		unselectedEntities << (*it)->getName ( );
	}	// for (vector<Entity*>::const_iterator it = entities.begin ( ); ...

	setText (selection.trimmed ( ));

	emit entitiesRemovedFromSelection (UTF8TOQSTRING (unselectedEntities));
	emit selectionModified (getQText ( ));
}	// QtEntityIDTextField::removeFromSelection


void QtEntityIDTextField::returnPressedCallback ( )
{
//	emit selectionModified (getQText ( ));
}	// QtEntityIDTextField::returnPressedCallback


void QtEntityIDTextField::filterText ( )
{
	if (false == isModified ( ))
		return;

	const string	text		= getText ( );
	bool			modified	= false;
	UTF8String		newtext (Charset::UTF_8);
	for (string::const_iterator it = text.begin ( ); text.end ( ) != it; it++)
	{
		if ((0 != isalnum (*it)) || (' ' == *it))
			newtext << *it;
		else
		{
			modified	= true;
			string::const_iterator	itn	= it;
			// S'assurer (au moins) un séparateur si nécessaire :
			bool	addws		= false;
			bool	lastisws	= false;
			while (text.end ( ) != itn)
			{
				if(text.begin ( ) == itn)
					break;
				if(' ' == *itn)
					break;
				if (0 != isalnum (*itn))
				{
					addws	= true;
					break;
				}	// if (0 != isalnum (*itn))

				itn++;
			}	// while (text.end ( ) != itn)
			if (true == addws)
			{
				const size_t	len	= newtext.length ( );
//				if ((0 != len) && (' ' != newtext [len - 1]))
				if ((0 != len) && (' ' != *newtext [len - 1]))
				newtext << ' ';
			}	// if (true == addws)
		}	// else if ((0 != isalnum (*it)) || (' ' == *it))
	}	// for (string::iterator it = text.begin ( ): text.end ( ) != it; it++)

	if (true == modified)
	{
		QtObjectSignalBlocker	blocker (this);
		setText (UTF8TOQSTRING (newtext));
	}	// if (true == modified)
}	// QtEntityIDTextField::filterText


void QtEntityIDTextField::textModifiedCallback ( )
{
	if (false == isModified ( ))
		return;

	try
	{	// CP MGXDDD-262 : on se met en mode interactif si ça n'est pas le cas.
		// On peut arriver à ce stade sans avoir pour autant activé le champ de
		// texte par clic souris bouton gauche, par exemple en faisant juste un
		// "paste" sur le champ de texte au bouton du milieu.
		if (false == inInteractiveMode ( ))
		{
			setInteractiveMode (true);
		}	// if (false == inInteractiveMode ( ))
	}
	catch (...)
	{
	}

	try
	{
		// On enlève la sélection dûe à cette instance du gestionnaire de
		// sélection et on y ajoute, si possible, celle saisie par
		// l'utilisateur :
		const vector<Entity*>	selectedEntities	= getSelectedEntities ( );
		clearSeizuredEntities ( );
		if ((0 != getSelectionManager ( )) && (0 != selectedEntities.size ( )))
			getSelectionManager ( )->removeFromSelection (selectedEntities);

		UTF8String		selection (Charset::UTF_8);
		vector<string>	names	= getUniqueNames ( );
		for (vector<string>::iterator it = names.begin ( );
		     names.end ( ) != it; it++)
		{
			try
			{
				Entity&	entity = getContext().nameToEntity (*it);
				if (0 != getSelectionManager ( ))
				{
					// EntitySeizureManager va appeler addToSelection et autres
					// => ne pas rajouter le nom des entités pré-sélectionnées
					// à la liste en cours.
					_updating	= true;
					vector<Entity*>	entities;
					entities.push_back (&entity);
					getSelectionManager ( )->addToSelection (entities);
					_updating	= false;
				}
				if (false == selection.empty ( ))
					selection << ' ';
				selection << *it;
			}
			catch (...)
			{
				_updating	= false;
				UTF8String	message (Charset::UTF_8);
				message << "Erreur : il n'y a pas d'entité du nom de \"" << *it
				        << "\". Entité enlevée de la sélection.";
				getContext ( ).getLogDispatcher ( ).log (WarningLog (message));
			}

			setText (UTF8TOQSTRING (selection.trim ( )));
		}	// for (vector<string>::iterator it = names.begin ( );

// MGXDDD-262 : Faut-il ajouter ici un
// emit entitiesAddedToSelection (getQText ( )) pour des cas tordus si on a
// false == inInteractiveMode ( ) en entrée de fonction ???
		emit selectionModified (getQText ( ));
	}
	catch (...)
	{
	}

	setModified (false);
}	// QtEntityIDTextField::textModifiedCallback


const ContextIfc& QtEntityIDTextField::getContext ( ) const
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return _mainWindow->getContext ( );
}	// QtEntityIDTextField::getContext


ContextIfc& QtEntityIDTextField::getContext ( )
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return _mainWindow->getContext ( );
}	// QtEntityIDTextField::getContext

}	// namespace QtComponents

}	// namespace Mgx3D
