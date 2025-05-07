/**
 * \file        QtMgx3DGroupNamePanel.cpp
 * \author      Charles PIGNEROL
 * \date        11/12/2012
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"

#include "Group/Group0D.h"
#include "Group/Group1D.h"
#include "Group/Group2D.h"
#include "Group/Group3D.h"
#include "Group/GroupManager.h"

#include <TkUtil/MemoryError.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>

#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Group;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;
using namespace Mgx3D::QtComponents;


namespace Mgx3D
{

namespace QtComponents
{

// ===========================================================================
//                        LA CLASSE QtMgx3DGroupNamePanel
// ===========================================================================

QtMgx3DGroupNamePanel::QtMgx3DGroupNamePanel (
	QWidget* parent, const string& name, QtMgx3DMainWindow& mainWindow, int dimension, QtMgx3DGroupNamePanel::POLICY policy, const string& defaultName)
	: QWidget (parent), ValidatedField ( ),
	  _nameComboBox (0), _dimension (dimension), _mainWindow (&mainWindow), _policy (QtMgx3DGroupNamePanel::INITIALIZATION), _defaultName (defaultName)
{
	QHBoxLayout*	layout	= new QHBoxLayout (this);
	layout->setMargin (QtConfiguration::margin);
	layout->setSpacing (QtConfiguration::spacing);
	setLayout (layout);
	QLabel*	label	= new QLabel ("Groupe", this);
	layout->addWidget (label);
	_nameComboBox	= new QComboBox (this);
	_nameComboBox->setInsertPolicy (QComboBox::InsertAlphabetically);	// Les saisies utilisateurs seront triées alphabétiquement
	layout->addWidget (_nameComboBox, 1000);
	setPolicy (policy, defaultName);
}	// QtMgx3DGroupNamePanel::QtMgx3DGroupNamePanel


QtMgx3DGroupNamePanel::QtMgx3DGroupNamePanel (const QtMgx3DGroupNamePanel&)
	: QWidget (0), ValidatedField ( ),
	  _nameComboBox (0), _dimension (-1), _mainWindow (0), _policy (QtMgx3DGroupNamePanel::INITIALIZATION), _defaultName ( )
{
	MGX_FORBIDDEN ("QtMgx3DGroupNamePanel copy constructor is not allowed.");
}	// QtMgx3DGroupNamePanel::QtMgx3DGroupNamePanel (const QtMgx3DGroupNamePanel&)


QtMgx3DGroupNamePanel& QtMgx3DGroupNamePanel::operator = (const QtMgx3DGroupNamePanel&)
{
	MGX_FORBIDDEN ("QtMgx3DGroupNamePanel assignment operator is not allowed.");
	return *this;
}	// QtMgx3DGroupNamePanel::QtMgx3DGroupNamePanel (const QtMgx3DGroupNamePanel&)


QtMgx3DGroupNamePanel::~QtMgx3DGroupNamePanel ( )
{
}	// QtMgx3DGroupNamePanel::~QtMgx3DGroupNamePanel


ContextIfc& QtMgx3DGroupNamePanel::getContext ( )
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	return _mainWindow->getContext ( );
}	// QtMgx3DGroupNamePanel::getContext


QtMgx3DGroupNamePanel::POLICY QtMgx3DGroupNamePanel::getPolicy ( ) const
{
	return _policy;
}	// QtMgx3DGroupNamePanel::getPolicy


void QtMgx3DGroupNamePanel::setPolicy (QtMgx3DGroupNamePanel::POLICY policy, const string& defaultName)
{
	CHECK_NULL_PTR_ERROR (_nameComboBox)
	CHECK_NULL_PTR_ERROR (_mainWindow)
	if (getPolicy ( ) == policy)
		return;

	_policy			= policy;
	_defaultName	= defaultName;
	_nameComboBox->clear ( );

	switch (policy)
	{
		case QtMgx3DGroupNamePanel::INITIALIZATION	:
			break;
		case QtMgx3DGroupNamePanel::CREATION		:
		case QtMgx3DGroupNamePanel::CONSULTATION		:
		{
			vector<string>	groupNames;
			getGroupNames (groupNames);
			for (vector<string>::iterator it = groupNames.begin ( ); groupNames.end ( ) != it; it++)
				_nameComboBox->addItem ((*it).c_str ( ));
			if ((false == defaultName.empty ( )) && (-1 != _nameComboBox->findText (defaultName.c_str ( ))))
				_nameComboBox->addItem (defaultName.c_str ( ));
			_nameComboBox->setEditable (QtMgx3DGroupNamePanel::CREATION ==  policy);
			if (false == defaultName.empty ())
				_nameComboBox->setCurrentIndex (_nameComboBox->findText (defaultName.c_str ( )));
		}	// case QtMgx3DGroupNamePanel::CREATION
		break;
		default	:
			cerr << __FILE__ << ' ' << __LINE__ << " QtMgx3DGroupNamePanel::QtMgx3DGroupNamePanel : Politique inconnue (" << (unsigned long)policy << ")." << endl;
	}	// switch (policy)
	if (0 != _nameComboBox->model ( ))
		_nameComboBox->model ( )->sort (0);	// => tri alphabétique
	// Rem : le 0 est pour la 1ère colonne, donc le texte, et en 2nd argument on peut transmettre le sens à utiliser, par défaut Qt::AscendingOrder, mais ce
	// pourrait être Qt::DescendingOrder.
	// On pourrait utiliser un autre modèle "personnalisé", avec par exemple les Hors_groupe_* en premier, puis un tri alphabétique.
}	// QtMgx3DGroupNamePanel::setPolicy


int QtMgx3DGroupNamePanel::getDimension ( ) const
{
	return _dimension;
}	// QtMgx3DGroupNamePanel::getDimension


void QtMgx3DGroupNamePanel::setDimension (int dim)
{
	if (dim != _dimension)
	{
		_dimension	= dim;
		autoUpdate ( );
	}	// if (dim != _dimension)
}	// QtMgx3DGroupNamePanel::setDimension


string QtMgx3DGroupNamePanel::getGroupName ( ) const
{
	CHECK_NULL_PTR_ERROR (_nameComboBox)
	return _nameComboBox->currentText ( ).toStdString ( );
}	// QtMgx3DGroupNamePanel::getGroupName


string QtMgx3DGroupNamePanel::getDefaultName ( ) const
{
	return _defaultName;
}	// QtMgx3DGroupNamePanel::getDefaultName


void QtMgx3DGroupNamePanel::validate ( )
{
	switch (getPolicy ( ))
	{
		case QtMgx3DGroupNamePanel::CREATION		:
//			if (0 == getGroupName ( ).length ( ))
//				throw Exception ("Nom du groupe nul en mode création de groupe.");
			break;
		case QtMgx3DGroupNamePanel::CONSULTATION	:
/*			if (getGroupName ( ) != getDefaultName ( ))
			{
				UTF8String	error (Charset::UTF_8);
				error << "Nom de groupe (" << getGroupName ( )
				      << ") différent du nom proposé par défaut ("
				      << getDefaultName ( )
				      << ") en mode consultation de groupe.";
				throw Exception (error);
			}*/
			break;
		default	:
		{
			UTF8String	error (Charset::UTF_8);
			error << "QtMgx3DGroupNamePanel::validate. Politique inconnue (" << (unsigned long)getPolicy ( ) << ")";
			throw Exception (error);
		}
	}	// switch (getPolicy ( ))
}	// QtMgx3DGroupNamePanel::validate


void QtMgx3DGroupNamePanel::autoUpdate ( )
{
	CHECK_NULL_PTR_ERROR (_nameComboBox)
	const string	name		= getGroupName ( );
	const string&	defaultName	= getDefaultName ( );
	const POLICY	policy		= getPolicy ( );
	setPolicy (INITIALIZATION, "");
	setPolicy (policy, defaultName);

#ifdef AUTO_UPDATE_OLD_SCHEME
	// On sélectionne l'éventuel groupe sélectionné avant cet appel :
	if (0 != name.length ( ))
	{
		int	index	= _nameComboBox->findText (name.c_str ( ));
		if (-1 != index)
			_nameComboBox->setCurrentIndex (index);
	}	// if (0 != name.length ( ))
	else
		_nameComboBox->setCurrentIndex (-1);
#else	// AUTO_UPDATE_OLD_SCHEME
	_nameComboBox->setCurrentIndex (-1);
#endif	// AUTO_UPDATE_OLD_SCHEME
}	// QtMgx3DGroupNamePanel::autoUpdate


void QtMgx3DGroupNamePanel::getGroupNames (vector< string >& names)
{
	CHECK_NULL_PTR_ERROR (_mainWindow)
	names.clear ( );

	GroupManager&	groupManager	= _mainWindow->getContext ( ).getGroupManager ( );

	switch (getDimension ( ))
	{
		case 0	:
		{
			vector<Group0D*>	groups;
			groupManager.getGroup0D (groups, true);
			for (vector<Group0D*>::const_iterator it = groups.begin ( ); groups.end ( ) != it; it++)
				names.push_back ((*it)->getName ( ));
		}
		break;
		case 1	:
		{
			vector<Group1D*>	groups;
			groupManager.getGroup1D (groups, true);
			for (vector<Group1D*>::const_iterator it = groups.begin ( ); groups.end ( ) != it; it++)
				names.push_back ((*it)->getName ( ));
		}
		break;
		case 2	:
		{
			vector<Group2D*>	groups;
			groupManager.getGroup2D (groups, true);
			for (vector<Group2D*>::const_iterator it = groups.begin ( ); groups.end ( ) != it; it++)
				names.push_back ((*it)->getName ( ));
		}
		break;
		case 3	:
		{
			vector<Group3D*>	groups;
			groupManager.getGroup3D (groups, true);
			for (vector<Group3D*>::const_iterator it = groups.begin ( ); groups.end ( ) != it; it++)
				names.push_back ((*it)->getName ( ));
		}
		break;
		default	:
		{
			UTF8String	error (Charset::UTF_8);
			error << "QtMgx3DGroupNamePanel::getGroupNames : " << "dimension invalide (" << (long)getDimension ( ) << ").";
			throw Exception (error);
		}
	}	// switch (getDimension ( ))
}	// QtMgx3DGroupNamePanel::getGroupNames


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
