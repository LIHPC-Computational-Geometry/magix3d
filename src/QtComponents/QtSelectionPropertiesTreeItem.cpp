/**
 * \file		QtSelectionPropertiesTreeItem.cpp
 * \author		Charles PIGNEROL
 * \date		07/10/2013
 */


#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "QtComponents/QtSelectionIndividualPropertiesPanel.h"

#include "Utils/Common.h"

#include <TkUtil/MemoryError.h>


using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace TkUtil;
using namespace std;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                    LA CLASSE QtSelectionPropertiesTreeItem
// ===========================================================================

QtSelectionPropertiesTreeItem::QtSelectionPropertiesTreeItem (
		QTreeWidget* parent, SerializedRepresentation* description,
		bool alsoComputed, RenderingManager* rm)
	: QTreeWidgetItem (parent),
	  _description (description), _displayComputedProperties (alsoComputed),
	  _renderingManager (rm)
{
	setChildIndicatorPolicy (QTreeWidgetItem::ShowIndicator);
}	// QtSelectionPropertiesTreeItem::QtSelectionPropertiesTreeItem


QtSelectionPropertiesTreeItem::QtSelectionPropertiesTreeItem (
		QTreeWidgetItem* parent, SerializedRepresentation* description,
		bool alsoComputed, RenderingManager* rm)
	: QTreeWidgetItem (parent),
	  _description (description), _displayComputedProperties (alsoComputed),
	  _renderingManager (rm)
{
	setChildIndicatorPolicy (QTreeWidgetItem::ShowIndicator);
	if (0 != description)
		setText (0, description->getName ( ).c_str ( ));
}	// QtSelectionPropertiesTreeItem::QtSelectionPropertiesTreeItem


QtSelectionPropertiesTreeItem::QtSelectionPropertiesTreeItem (
										const QtSelectionPropertiesTreeItem&)
	: QTreeWidgetItem ((QTreeWidget*)0), _description ( ),
	  _displayComputedProperties(0), _renderingManager (0)
{
	MGX_FORBIDDEN ("QtSelectionPropertiesTreeItem copy constructor is not allowed.");
}	// QtSelectionPropertiesTreeItem::QtSelectionPropertiesTreeItem


QtSelectionPropertiesTreeItem& QtSelectionPropertiesTreeItem::operator = (
										const QtSelectionPropertiesTreeItem&)
{
	MGX_FORBIDDEN ("QtSelectionPropertiesTreeItem assignment operator is not allowed.");
	return *this;
}	// QtSelectionPropertiesTreeItem::operator =


QtSelectionPropertiesTreeItem::~QtSelectionPropertiesTreeItem ()
{
}	// QtSelectionPropertiesTreeItem::~QtSelectionPropertiesTreeItem


const SerializedRepresentation&
						QtSelectionPropertiesTreeItem::getDescription ( ) const
{
	 CHECK_NULL_PTR_ERROR (_description.get ( ))
	return *(_description.get ( ));
}	// QtSelectionPropertiesTreeItem::getDescription


bool QtSelectionPropertiesTreeItem::displayComputedProperties ( ) const
{
	return _displayComputedProperties;
}	// QtSelectionPropertiesTreeItem::displayComputedProperties


void QtSelectionPropertiesTreeItem::initialize ( )
{
//	setChildIndicatorPolicy (QTreeWidgetItem::ShowIndicator);
	int				i	= 0;
	const string	summary	= getDescription ( ).getSummary ( );
	if (0 != summary.length ( ))
		for (i = 0; i < columnCount ( ); i++)
			setToolTip (i, summary.c_str ( ));	
}	// QtSelectionPropertiesTreeItem::initialize


void QtSelectionPropertiesTreeItem::displayContents ( )
{
	if (0 != childCount ( ))
		return;

	// Les propriétés :
	const vector<SerializedRepresentation::Property>&	properties	=
										getDescription ( ).getProperties ( );
	for (vector<SerializedRepresentation::Property>::const_iterator
				it1 = properties.begin ( ); properties.end ( ) != it1; it1++)
	{
		QTreeWidgetItem*	propItem	= 0;
		if (SerializedRepresentation::Property::ENTITY != (*it1).getType ( ))
			propItem	= new QTreeWidgetItem (this);
		else
		{
			try
			{
				Entity&		entity	=
					getContext().uniqueIdToEntity ((*it1).getEntityUniqueId( ));
				QtSelectionPropertiesTreeItem*	pi	=
					new QtEntityPropertiesItem (this, entity,
						displayComputedProperties ( ), getRenderingManager ( ));
				pi->initialize ( );
				propItem	= pi;
			}
			catch (...)
			{
				propItem	= new QTreeWidgetItem (this);
			}
		}
		CHECK_NULL_PTR_ERROR (propItem)
		propItem->setText (0, (*it1).getName ( ).c_str ( ));
		propItem->setText (1, (*it1).getValue ( ).c_str ( ));
		// On veut pouvoir faire du copier/coller (sans modifier, mais le
		// fait est qu'éditable => modifiable, et qu'on ne peut faire du
		// copier/coller sans que ça ne soit éditable) :
		propItem->setFlags (
				Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	}	// for (vector<SerializedRepresentation::Property>::const_iterator ...

	// Les sous-rubriques :
	const vector<SerializedRepresentation*>&	propertiesSets	=
									getDescription ( ).getPropertiesSets ( );
	for (vector<SerializedRepresentation*>::const_iterator it2 =
			propertiesSets.begin ( ); propertiesSets.end ( ) != it2; it2++)
	{
		QtSelectionPropertiesTreeItem*	propItem	=
				new QtSelectionPropertiesTreeItem (this,
						new SerializedRepresentation (**it2),
						displayComputedProperties ( ), getRenderingManager ( ));
		propItem->initialize ( );
	}	// for (vector<SerializedRepresentation>::const_iterator ...
}	// QtSelectionPropertiesTreeItem::displayContents


RenderingManager* QtSelectionPropertiesTreeItem::getRenderingManager ( )
{
	return _renderingManager;
}	// QtSelectionPropertiesTreeItem::getRenderingManager


const ContextIfc& QtSelectionPropertiesTreeItem::getContext ( ) const
{
	CHECK_NULL_PTR_ERROR (_renderingManager)
	return _renderingManager->getContext ( );
}	// QtSelectionPropertiesTreeItem::getContext


void QtSelectionPropertiesTreeItem::setDescription (
							Utils::SerializedRepresentation* description)
{
	_description.reset (description);
}	// tSelectionPropertiesTreeItem::setDescription


}	// namespace QtComponents

}	// namespace Mgx3D

