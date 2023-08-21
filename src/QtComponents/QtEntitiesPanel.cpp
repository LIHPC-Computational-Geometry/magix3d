/**
 * \file        QtEntitiesPanel.cpp
 * \author      Charles PIGNEROL
 * \date        29/11/2010
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "Internal/EntitiesHelper.h"
#include "Internal/InternalPreferences.h"
#include "QtComponents/QtEntitiesPanel.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/RenderedEntityRepresentation.h"

#include <TkUtil/ErrorLog.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/LogOutputStream.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/ThreeStates.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtObjectSignalBlocker.h>

#include <QComboBox>
#include <QThread>

#include <assert.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Internal;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Group;



namespace Mgx3D
{

namespace QtComponents
{

static QAction* createAction (
			QMenu& parent, const QString& label, const QString& whatsThis,
			const char* method, QObject* receiver,
			bool checkable, THREESTATES state)
{
	QAction*	action	= new QAction (label, &parent);
	action->setWhatsThis (whatsThis);
	if (true == checkable)
	{
		action->setCheckable (true);
		switch (state)
		{
			case ON		: action->setChecked (true);	break;
			// Pb : Qt 4.6.2 ne prévoit plus les checkbox dans les menus avec
			// le setTristate (true) et le setCheckState (Qt::PartiallyChecked).
			case OFF	:
			default		: action->setChecked (false);
		}	// switch (state)
	}	// if (true == checkable)
	if ((0 != method) && (0 != receiver))
		QObject::connect (action, SIGNAL (triggered ( )), receiver, method);
	parent.addAction (action);

	return action;
}	// createAction


// ===========================================================================
//                      LA CLASSE QtEntitiesGroupTreeWidgetItem
// ===========================================================================

/**
 * <P>Classe représentant un ensemble d'items de type <I>Entity</I> dans un
 * arbre <I>Qt</I> de type <I>QTreeWidget</I>.</P>
 *
 * <P>Cette classe gère des transitions d'état (ex : affiché ou non, type
 * d'affichage 3D) pour l'ensemble des items qui lui sont rattachés.</P>
 */
class QtEntitiesGroupTreeWidgetItem : public QTreeWidgetItem
{
	public :

	/**
	 * Constructeur.
	 * \param		Parent
	 * \param		Nom
	 * \param		Panneau associé
	 * \param		Type d'entités du groupe.
	 */
	QtEntitiesGroupTreeWidgetItem (
		QTreeWidgetItem* parent, const string& name, QtEntitiesPanel* panel,
		DisplayRepresentation::display_type type);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtEntitiesGroupTreeWidgetItem ( );

	/**
	 * \return		Le masque de représentation 3D des entités associées.
	 *				Vaut une valeur possible définie par la classe
	 *				<I>GraphicalEntityRepresentation</I> (ex : <I>CURVES</I>,
	 *				<I>SURFACES</I>), ou <I>-1</I> en cas d'état indéterminé
	 *				(les items enfants n'ont pas tous la même représentation
	 *				3D).
	 * \see			setRepresentationMask
	 * \see			GraphicalEntityRepresentation::CURVES
	 * \see			GraphicalEntityRepresentation::SURFACES
	 */
	virtual unsigned long getRepresentationMask ( ) const
	{ return _representationMask; }

	/**
	 * \param		Nouveau masque de représentation 3D des entités associées,
	 *				ou <I>-1</I> si indéterminé. Actualise l'item et ses
	 *				enfants.
	 * \see			getRepresentationMask
	 */
	virtual void setRepresentationMask (unsigned long mask);

	/**
	 * Méthode appelé lorsque l'état de l'item change.
	 * \param		Indice de la colonne dont l'état vient de changer.
	 */
	virtual void stateChange (int col);

	/**
	 * \return		Le type d'entités du groupe.
	 */
	virtual DisplayRepresentation::display_type getEntitiesType ( ) const;


	private :

	/**
	 * Constructeur de copie, opérateur = : interdits.
	 */
	QtEntitiesGroupTreeWidgetItem (const QtEntitiesGroupTreeWidgetItem&);
	QtEntitiesGroupTreeWidgetItem& operator = (
										const QtEntitiesGroupTreeWidgetItem&);

	/** Le masque de représentation 3D des items enfants, ou -1 si état
	 * indéterminé. */
	unsigned long						_representationMask;

	/** L'éventuel panneau associé. */
	QtEntitiesPanel*					_panel;

	/** Le type d'entités. */
	DisplayRepresentation::display_type	_entitiesType;
};	// class QtEntitiesGroupTreeWidgetItem


QtEntitiesGroupTreeWidgetItem::QtEntitiesGroupTreeWidgetItem (
			QTreeWidgetItem* parent, const string& name, QtEntitiesPanel* panel,
			DisplayRepresentation::display_type type)
	: QTreeWidgetItem (parent), _representationMask (0), _panel (panel),
	  _entitiesType (type)
{
#ifdef REPRESENTATION_TYPE_BY_POPUP
	setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
#else	// REPRESENTATION_TYPE_BY_POPUP
	setFlags (Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
#endif	// REPRESENTATION_TYPE_BY_POPUP
	setCheckState (0, Qt::Unchecked);
	setText (0, name.c_str ( ));
	setRepresentationMask (GraphicalEntityRepresentation::CURVES);

	CHECK_NULL_PTR_ERROR (_panel)
}	// QtEntitiesGroupTreeWidgetItem::QtEntitiesGroupTreeWidgetItem


QtEntitiesGroupTreeWidgetItem::QtEntitiesGroupTreeWidgetItem (
									const QtEntitiesGroupTreeWidgetItem& item)
	: QTreeWidgetItem (0), _representationMask (item._representationMask), _panel (item._panel),
	  _entitiesType (item._entitiesType)
{
	MGX_FORBIDDEN ("QtEntitiesGroupTreeWidgetItem copy constructor is not allowed.");
}	// QtEntitiesGroupTreeWidgetItem::QtEntitiesGroupTreeWidgetItem


QtEntitiesGroupTreeWidgetItem& QtEntitiesGroupTreeWidgetItem::operator = (
										const QtEntitiesGroupTreeWidgetItem&)
{
	MGX_FORBIDDEN ("QtEntitiesGroupTreeWidgetItem assignment operator is not allowed.");
	return *this;
}	// QtEntitiesGroupTreeWidgetItem::operator =


QtEntitiesGroupTreeWidgetItem::~QtEntitiesGroupTreeWidgetItem ( )
{
}	// QtEntitiesGroupTreeWidgetItem::~QtEntitiesGroupTreeWidgetItem


void QtEntitiesGroupTreeWidgetItem::setRepresentationMask (unsigned long mask)
{
	if (mask != _representationMask)
	{
		_representationMask	= mask;
#ifndef REPRESENTATION_TYPE_BY_POPUP
		string	rep	= GraphicalEntityRepresentation::rep3DMaskToString (mask);
		setText (1, rep.c_str ( ));
#endif	// REPRESENTATION_TYPE_BY_POPUP

		if ((unsigned long)-1 != mask)
		{
			for (int i = 0; i < childCount ( ); i++)
			{
				QTreeWidgetItem*		item		= child (i);
				QtEntityTreeWidgetItem*	entityItem	= 
						dynamic_cast<QtEntityTreeWidgetItem*>(item);
				CHECK_NULL_PTR_ERROR (entityItem)
				entityItem->setRepresentationMask (mask);
			}	// for (int i = 0; i < childCount ( ); i++)
		}	// if ((unsigned long)-1 != mask)
	}	// if (mask != _representationMask)
}	// QtEntitiesGroupTreeWidgetItem::setRepresentationMask


DisplayRepresentation::display_type
					QtEntitiesGroupTreeWidgetItem::getEntitiesType ( ) const
{
	return _entitiesType;
}	// QtEntitiesGroupTreeWidgetItem::getEntitiesType


void QtEntitiesGroupTreeWidgetItem::stateChange (int col)
{
	switch (col)
	{
		case	0	:	// Affichage/Masquage
		{
			const Qt::CheckState	currentState	= checkState (0);
			if (Qt::PartiallyChecked == currentState)
				return;

			// Verrouiller les opérations d'affichage (optimisation) :
			CHECK_NULL_PTR_ERROR (_panel)
			Qt3DGraphicalWidget*	graphicalWidget	=
												_panel->getGraphicalWidget ( );
			CHECK_NULL_PTR_ERROR (graphicalWidget)
			RenderingManager::DisplayLocker	displayLocker(
									graphicalWidget->getRenderingManager ( ));
			vector<Entity*>			entities;
			unsigned long			mask	= 0;
			const bool	checked	= Qt::Checked == currentState ? true : false;
			for (int i = 0; i < childCount ( ); i++)
			{
				QTreeWidgetItem*	item	= child (i);
				CHECK_NULL_PTR_ERROR (item)
				bool	changed	= !(item->checkState (0) == currentState);
				item->setCheckState (0, currentState);
				QtEntityTreeWidgetItem*	entityItem	=
					dynamic_cast<QtEntityTreeWidgetItem*>(item);
				CHECK_NULL_PTR_ERROR (entityItem)
				if (false == _panel->groupEntitiesDisplayModifications ( ))
					graphicalWidget->getRenderingManager (
													).displayRepresentation (
							*(entityItem->getEntity ( )), checked,
							entityItem->getRepresentationMask ( ));
				else if (true == changed)
				{
					entities.push_back (entityItem->getEntity ( ));
					mask	= entityItem->getRepresentationMask ( );
				}	// else if (true == changed)
			}	// for (int i = 0; i < childCount ( ); i++)
			if ((true == _panel->groupEntitiesDisplayModifications ( )) &&
			    (0 != entities.size ( )))
				graphicalWidget->getRenderingManager (
						).displayRepresentations (entities, currentState, mask,
						                          getEntitiesType ( ));
		}	// case 0
		break;
		case	1	:	// Mode d'affichage 3D
		{
		}	// case 1
	}	// switch (col)
}	// QtEntitiesGroupTreeWidgetItem::stateChange


// ===========================================================================
//                      LA CLASSE QtEntityTreeWidgetItem
// ===========================================================================

QtEntityTreeWidgetItem::QtEntityTreeWidgetItem (
				QTreeWidgetItem* parent, Entity& entity, unsigned long rep3d)
	: QTreeWidgetItem (parent), DisplayProperties::EntityView (&entity),
	  _entity (&entity), _3dRep (rep3d)
{
	setFlags (Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable |
	          Qt::ItemIsUserCheckable);
// update (false) est appelé par les constructeurs des classes dérivées
// => on ne le fait plus, mais il est important que ce soit fait par les
// constructeurs des classes d'implémentation car sinon l'entité ne sera pas
// visible si son affichage est requis lors de l'instanciation.
//	update (false);
	entity.getDisplayProperties ( ).registerEntityView (*this);
}	// QtEntityTreeWidgetItem::QtEntityTreeWidgetItem


QtEntityTreeWidgetItem::QtEntityTreeWidgetItem (const QtEntityTreeWidgetItem&)
	: QTreeWidgetItem (0), DisplayProperties::EntityView (0),
	  _entity (0), _3dRep (0)
{
	MGX_FORBIDDEN ("QtEntityTreeWidgetItem copy constructor is not allowed.");
}	// QtEntityTreeWidgetItem::QtEntityTreeWidgetItem


QtEntityTreeWidgetItem& QtEntityTreeWidgetItem::operator = (const QtEntityTreeWidgetItem&)
{
	MGX_FORBIDDEN ("QtEntityTreeWidgetItem assignment operator is not allowed.");
	return *this;
}	// QtEntityTreeWidgetItem::operator =


QtEntityTreeWidgetItem::~QtEntityTreeWidgetItem ( )
{
	if (0 != _entity)
		_entity->getDisplayProperties ( ).unregisterEntityView (*this);
}	// QtEntityTreeWidgetItem::~QtEntityTreeWidgetItem


Entity* QtEntityTreeWidgetItem::getEntity ( )
{
	return _entity;
}	// QtEntityTreeWidgetItem::getEntity


const Entity* QtEntityTreeWidgetItem::getEntity ( ) const
{
	return _entity;
}	// QtEntityTreeWidgetItem::getEntity


unsigned long QtEntityTreeWidgetItem::getNativeRepresentationMask ( ) const
{
	return _3dRep;
}	// QtEntityTreeWidgetItem::getNativeRepresentationMask


unsigned long QtEntityTreeWidgetItem::getRepresentationMask ( ) const
{
	Entity*	entity	= (Entity*)getEntity ( );	// Ugly code
	CHECK_NULL_PTR_ERROR (entity)
	CHECK_NULL_PTR_ERROR (entity->getDisplayProperties ( ).getGraphicalRepresentation ( ))
//	return _3dRep;
	return entity->getDisplayProperties ( ).getGraphicalRepresentation ( )->getRepresentationMask ( );
}	// QtEntityTreeWidgetItem::getRepresentationMask


void QtEntityTreeWidgetItem::setRepresentationMask (unsigned long representation)
{
assert (0);
	if (representation != _3dRep)
	{
		_3dRep	= representation;
		update (true);
	}	// if (representation != _3dRep)
}	// QtEntityTreeWidgetItem::setRepresentationMask


void QtEntityTreeWidgetItem::update (bool updateRepresentation)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	setText (0, 0 == getEntity ( ) ? "" : getEntity ( )->getName ( ).c_str ( ));

	const unsigned long	mask	= getRepresentationMask ( );
#ifndef REPRESENTATION_TYPE_BY_POPUP
	string	rep3d = GraphicalEntityRepresentation::rep3DMaskToString (mask);
	setText (1, rep3d.c_str ( ));
#endif	// REPRESENTATION_TYPE_BY_POPUP

	// La vue 3D :
	if ((0 != getEntity ( )) &&
	    (true == getEntity ( )->getDisplayProperties ( ).isDisplayed ( )))
	{
		QtEntitiesTreeWidget*			parent	=
				dynamic_cast<QtEntitiesTreeWidget*>(treeWidget ( ));	
		QtEntitiesPanel*		panel	=
							0 == parent ? 0 : parent->getEntitiesPanel ( );
		Qt3DGraphicalWidget*	graphicalWidget	= 
							0 == panel ? 0 : panel->getGraphicalWidget ( );
		GraphicalEntityRepresentation*	entity3DRep	= 
							0 == graphicalWidget ?
				0 : graphicalWidget->getRenderingManager ( ).getRepresentation (*getEntity ( ));
		if (0 != entity3DRep)
		{
			// CP : l'entité vient peut être d'être tout juste créée, et n'a pas
			// encore de gestionnaire de rendu associé => lui en donner un car
			// la représentation qui va être éventuellement créée dépend du mode
			// d'affichage courant (global ou non, MGXDDD-208) :
			RenderedEntityRepresentation*	rep	=
					dynamic_cast<RenderedEntityRepresentation*>(entity3DRep);
			if ((0 != rep) && (0 != graphicalWidget) &&
			    (0 == rep->getRenderingManager ( )))
				rep->setRenderingManager (&graphicalWidget->getRenderingManager ( ));
			entity3DRep->updateRepresentation (mask, true);
			if ((0 != graphicalWidget) && (true == updateRepresentation))
				graphicalWidget->getRenderingManager ( ).forceRender ( );
		}	// if (0 != entity3DRep)
	}	// if (0 != getEntity ( ))

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), treeWidget ( ), "Magix 3D")
}	// QtEntityTreeWidgetItem::update


void QtEntityTreeWidgetItem::entityDisplayed (bool displayed)
{
	setCheckState (0, true == displayed ? Qt::Checked : Qt::Unchecked);
}	// QtEntityTreeWidgetItem::entityDisplayed


// ===========================================================================
//                     LA CLASSE QtCADEntityTreeWidgetItem
// ===========================================================================

QtCADEntityTreeWidgetItem::QtCADEntityTreeWidgetItem (
		QTreeWidgetItem* parent, Geom::GeomEntity& entity, unsigned long rep3d)
	: QtEntityTreeWidgetItem (parent, entity, rep3d), _geomEntity (&entity)
{
	update (false);
}	// QtCADEntityTreeWidgetItem::QtCADEntityTreeWidgetItem


QtCADEntityTreeWidgetItem::QtCADEntityTreeWidgetItem (const QtCADEntityTreeWidgetItem& wi)
	: QtEntityTreeWidgetItem (wi.parent ( ), (Geom::GeomEntity&)*wi.getGeomEntity ( ), 0),	// beurk
	  _geomEntity (0)
{
	MGX_FORBIDDEN ("QtCADEntityTreeWidgetItem copy constructor is not allowed.");
}	// QtCADEntityTreeWidgetItem::QtCADEntityTreeWidgetItem


QtCADEntityTreeWidgetItem& QtCADEntityTreeWidgetItem::operator = (const QtCADEntityTreeWidgetItem&)
{
	MGX_FORBIDDEN ("QtCADEntityTreeWidgetItem assignment operator is not allowed.");
	return *this;
}	// QtCADEntityTreeWidgetItem::operator =


QtCADEntityTreeWidgetItem::~QtCADEntityTreeWidgetItem ( )
{
}	// QtCADEntityTreeWidgetItem::~QtCADEntityTreeWidgetItem


void QtCADEntityTreeWidgetItem::update (bool updateRepresentation)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtEntityTreeWidgetItem::update (updateRepresentation);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), treeWidget ( ), "Magix 3D")
}	// QtCADEntityTreeWidgetItem::update


// ===========================================================================
//                     LA CLASSE QtTopologicEntityTreeWidgetItem
// ===========================================================================

QtTopologicEntityTreeWidgetItem::QtTopologicEntityTreeWidgetItem (
		QTreeWidgetItem* parent, Topo::TopoEntity& entity, unsigned long rep3d)
	: QtEntityTreeWidgetItem (parent, entity, rep3d), _topoEntity (&entity)
{
	update (false);
}	// QtTopologicEntityTreeWidgetItem::QtTopologicEntityTreeWidgetItem


QtTopologicEntityTreeWidgetItem::QtTopologicEntityTreeWidgetItem (const QtTopologicEntityTreeWidgetItem& wi)
	: QtEntityTreeWidgetItem (wi.parent ( ), (Topo::TopoEntity&)*wi.getTopologicEntity ( ), 0),	// beurk
	  _topoEntity (0)
{
	MGX_FORBIDDEN ("QtTopologicEntityTreeWidgetItem copy constructor is not allowed.");
}	// QtTopologicEntityTreeWidgetItem::QtTopologicEntityTreeWidgetItem


QtTopologicEntityTreeWidgetItem& QtTopologicEntityTreeWidgetItem::operator = (const QtTopologicEntityTreeWidgetItem&)
{
	MGX_FORBIDDEN ("QtTopologicEntityTreeWidgetItem assignment operator is not allowed.");
	return *this;
}	// QtTopologicEntityTreeWidgetItem::operator =


QtTopologicEntityTreeWidgetItem::~QtTopologicEntityTreeWidgetItem ( )
{
}	// QtTopologicEntityTreeWidgetItem::~QtTopologicEntityTreeWidgetItem


void QtTopologicEntityTreeWidgetItem::update (bool updateRepresentation)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtEntityTreeWidgetItem::update (updateRepresentation);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), treeWidget ( ), "Magix 3D")
}	// QtTopologicEntityTreeWidgetItem::update


// ===========================================================================
//                     LA CLASSE QtMeshEntityTreeWidgetItem
// ===========================================================================

QtMeshEntityTreeWidgetItem::QtMeshEntityTreeWidgetItem (
		QTreeWidgetItem* parent, Mesh::MeshEntity& entity, unsigned long rep3d)
	: QtEntityTreeWidgetItem (parent, entity, rep3d), _meshEntity (&entity)
{
	update (false);
}	// QtMeshEntityTreeWidgetItem::QtMeshEntityTreeWidgetItem


QtMeshEntityTreeWidgetItem::QtMeshEntityTreeWidgetItem (const QtMeshEntityTreeWidgetItem& wi)
	: QtEntityTreeWidgetItem (wi.parent ( ),
					(Mesh::MeshEntity&)*wi.getMeshEntity ( ), 0),	// beurk
	  _meshEntity (0)
{
	MGX_FORBIDDEN ("QtMeshEntityTreeWidgetItem copy constructor is not allowed.");
}	// QtMeshEntityTreeWidgetItem::QtMeshEntityTreeWidgetItem


QtMeshEntityTreeWidgetItem& QtMeshEntityTreeWidgetItem::operator = (const QtMeshEntityTreeWidgetItem&)
{
	MGX_FORBIDDEN ("QtMeshEntityTreeWidgetItem assignment operator is not allowed.");
	return *this;
}	// QtMeshEntityTreeWidgetItem::operator =


QtMeshEntityTreeWidgetItem::~QtMeshEntityTreeWidgetItem ( )
{
}	// QtMeshEntityTreeWidgetItem::~QtMeshEntityTreeWidgetItem


void QtMeshEntityTreeWidgetItem::update (bool updateRepresentation)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtEntityTreeWidgetItem::update (updateRepresentation);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), treeWidget ( ), "Magix 3D")
}	// QtMeshEntityTreeWidgetItem::update


// ===========================================================================
//                     LA CLASSE QtSysCoordEntityTreeWidgetItem
// ===========================================================================

QtSysCoordEntityTreeWidgetItem::QtSysCoordEntityTreeWidgetItem (
		QTreeWidgetItem* parent, CoordinateSystem::SysCoord& entity, unsigned long rep3d)
	: QtEntityTreeWidgetItem (parent, entity, rep3d), _sysCoordEntity (&entity)
{
	update (false);
}	// QtSysCoordEntityTreeWidgetItem::QtSysCoordEntityTreeWidgetItem


QtSysCoordEntityTreeWidgetItem::QtSysCoordEntityTreeWidgetItem (const QtSysCoordEntityTreeWidgetItem& wi)
	: QtEntityTreeWidgetItem (wi.parent ( ),
					(CoordinateSystem::SysCoord&)*wi.getSysCoordEntity ( ), 0),	// beurk
	  _sysCoordEntity (0)
{
	MGX_FORBIDDEN ("QtSysCoordEntityTreeWidgetItem copy constructor is not allowed.");
}	// QtSysCoordEntityTreeWidgetItem::QtSysCoordEntityTreeWidgetItem


QtSysCoordEntityTreeWidgetItem& QtSysCoordEntityTreeWidgetItem::operator = (const QtSysCoordEntityTreeWidgetItem&)
{
	MGX_FORBIDDEN ("QtSysCoordEntityTreeWidgetItem assignment operator is not allowed.");
	return *this;
}	// QtSysCoordEntityTreeWidgetItem::operator =


QtSysCoordEntityTreeWidgetItem::~QtSysCoordEntityTreeWidgetItem ( )
{
}	// QtSysCoordEntityTreeWidgetItem::~QtSysCoordEntityTreeWidgetItem


void QtSysCoordEntityTreeWidgetItem::update (bool updateRepresentation)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtEntityTreeWidgetItem::update (updateRepresentation);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), treeWidget ( ), "Magix 3D")
}	// QtSysCoordEntityTreeWidgetItem::update


// ===========================================================================
//                LA CLASSE QtStructuredMeshEntityTreeWidgetItem
// ===========================================================================

QtStructuredMeshEntityTreeWidgetItem::QtStructuredMeshEntityTreeWidgetItem (
		QTreeWidgetItem* parent, Structured::StructuredMeshEntity& entity, unsigned long rep3d)
	: QtEntityTreeWidgetItem (parent, entity, rep3d), _structuredMeshEntity (&entity)
{
	update (false);
}	// QtStructuredMeshEntityTreeWidgetItem::QtStructuredMeshEntityTreeWidgetItem


QtStructuredMeshEntityTreeWidgetItem::QtStructuredMeshEntityTreeWidgetItem (const QtStructuredMeshEntityTreeWidgetItem& wi)
	: QtEntityTreeWidgetItem (wi.parent ( ), (Structured::StructuredMeshEntity&)*wi.getStructuredMeshEntity ( ), 0),	// beurk
	  _structuredMeshEntity (0)
{
	MGX_FORBIDDEN ("QtStructuredMeshEntityTreeWidgetItem copy constructor is not allowed.");
}	// QtStructuredMeshEntityTreeWidgetItem::QtStructuredMeshEntityTreeWidgetItem


QtStructuredMeshEntityTreeWidgetItem& QtStructuredMeshEntityTreeWidgetItem::operator = (const QtStructuredMeshEntityTreeWidgetItem&)
{
	MGX_FORBIDDEN ("QtStructuredMeshEntityTreeWidgetItem assignment operator is not allowed.");
	return *this;
}	// QtStructuredMeshEntityTreeWidgetItem::operator =


QtStructuredMeshEntityTreeWidgetItem::~QtStructuredMeshEntityTreeWidgetItem ( )
{
}	// QtStructuredMeshEntityTreeWidgetItem::~QtStructuredMeshEntityTreeWidgetItem


void QtStructuredMeshEntityTreeWidgetItem::update (bool updateRepresentation)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtEntityTreeWidgetItem::update (updateRepresentation);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), treeWidget ( ), "Magix 3D")
}	// QtStructuredMeshEntityTreeWidgetItem::update


#ifndef REPRESENTATION_TYPE_BY_POPUP

// ===========================================================================
//      LA CLASSE QtEntitiesTreeWidget::QtEntityTreeWidgetItemDelegate
// ===========================================================================

QtEntitiesTreeWidget::QtEntityTreeWidgetItemDelegate::QtEntityTreeWidgetItemDelegate (QtEntitiesPanel* panel)
	: QItemDelegate (panel->getTreeWidget ( )), _entitiesPanel (panel)
{
}	// QtEntityTreeWidgetItemDelegate::QtEntityTreeWidgetItemDelegate


QtEntitiesTreeWidget::QtEntityTreeWidgetItemDelegate::QtEntityTreeWidgetItemDelegate (const QtEntitiesTreeWidget::QtEntityTreeWidgetItemDelegate&)
	: QItemDelegate (0), _entitiesPanel (0)
{
	MGX_FORBIDDEN ("QtEntityTreeWidgetItemDelegate copy constructor is not allowed.");
}	// QtEntityTreeWidgetItemDelegate::QtEntityTreeWidgetItemDelegate


QtEntitiesTreeWidget::QtEntityTreeWidgetItemDelegate&
	QtEntitiesTreeWidget::QtEntityTreeWidgetItemDelegate::operator = (
		const QtEntitiesTreeWidget::QtEntityTreeWidgetItemDelegate&)
{
	MGX_FORBIDDEN ("QtEntityTreeWidgetItemDelegate assignment operator is not allowed.");
	return *this;
}	// QtEntityTreeWidgetItemDelegate::QtEntityTreeWidgetItemDelegate


QtEntitiesTreeWidget::QtEntityTreeWidgetItemDelegate::~QtEntityTreeWidgetItemDelegate ( )
{
}	// QtEntityTreeWidgetItemDelegate::~QtEntityTreeWidgetItemDelegate


QWidget* QtEntitiesTreeWidget::QtEntityTreeWidgetItemDelegate::createEditor (
				QWidget* parent, const QStyleOptionViewItem& option,
				const QModelIndex& index) const
{
	assert (0 != _entitiesPanel);
	QtEntityTreeWidgetItem*	entityItem	=
				dynamic_cast<QtEntityTreeWidgetItem*>(
							_entitiesPanel->getTreeWidget ( )->currentItem ( ));
	QtEntitiesGroupTreeWidgetItem*	groupItem	=
				dynamic_cast<QtEntitiesGroupTreeWidgetItem*>(
							_entitiesPanel->getTreeWidget ( )->currentItem ( ));
	if (1 == index.column ( ))
	{
		QComboBox*	editor	= new QComboBox (parent);
		unsigned long	mask	= GraphicalEntityRepresentation::CLOUDS;
		editor->addItem (
			GraphicalEntityRepresentation::rep3DMaskToString (mask).c_str ( ),
			QVariant ((qulonglong)mask));
		mask	= GraphicalEntityRepresentation::CURVES;
		editor->addItem (
			GraphicalEntityRepresentation::rep3DMaskToString (mask).c_str ( ),
			QVariant ((qulonglong)mask));
		mask	= GraphicalEntityRepresentation::CURVES |
				  GraphicalEntityRepresentation::ISOCURVES;
		editor->addItem (
			GraphicalEntityRepresentation::rep3DMaskToString (mask).c_str ( ),
			QVariant ((qulonglong)mask));
		mask	= GraphicalEntityRepresentation::CLOUDS |
				  GraphicalEntityRepresentation::CURVES;
		editor->addItem (
			GraphicalEntityRepresentation::rep3DMaskToString (mask).c_str ( ),
			QVariant ((qulonglong)mask));
		mask	= GraphicalEntityRepresentation::SURFACES;
		editor->addItem (
			GraphicalEntityRepresentation::rep3DMaskToString (mask).c_str ( ),
			QVariant ((qulonglong)mask));
		mask	= GraphicalEntityRepresentation::CURVES |
				  GraphicalEntityRepresentation::SURFACES;
		editor->addItem (
			GraphicalEntityRepresentation::rep3DMaskToString (mask).c_str ( ),
			QVariant ((qulonglong)(mask)));
		mask	= GraphicalEntityRepresentation::CLOUDS |
				  GraphicalEntityRepresentation::CURVES |
				  GraphicalEntityRepresentation::SURFACES;
		editor->addItem (
			GraphicalEntityRepresentation::rep3DMaskToString (mask).c_str ( ),
			QVariant ((qulonglong)(mask)));
		if (0 != groupItem)
		{
			mask	= (unsigned long)-1;
			editor->addItem (
				GraphicalEntityRepresentation::rep3DMaskToString(mask).c_str( ),
				QVariant ((qulonglong)(mask)));
		}	// if (0 != groupItem)
		if (0 != groupItem)
			mask	= groupItem->getRepresentationMask ( );
		else if (0 != entityItem)
			mask	= entityItem->getRepresentationMask ( );
		for (int i = 0; i < editor->count ( ); i++)
		{
			QVariant	v	= editor->itemData (i);
			bool		ok	= true;
			qulonglong	ul	= v.toULongLong (&ok);
			if ((true == ok) && (ul == mask))
			{
				editor->setCurrentIndex (i);
				break;
			}
		}	// if (0 != entityItem)

		return editor;
	}	// if (1 == index.column ( ))

	return QItemDelegate::createEditor (parent, option, index);
}	// QtEntityTreeWidgetItemDelegate::createEditor


void QtEntitiesTreeWidget::QtEntityTreeWidgetItemDelegate::setModelData (
				QWidget* editor, QAbstractItemModel* model,
				const QModelIndex& index) const
{
	assert (0 != _entitiesPanel);
	QtEntityTreeWidgetItem*	entityItem	=
				dynamic_cast<QtEntityTreeWidgetItem*>(
							_entitiesPanel->getTreeWidget ( )->currentItem ( ));
	QtEntitiesGroupTreeWidgetItem*	groupItem	=
				dynamic_cast<QtEntitiesGroupTreeWidgetItem*>(
							_entitiesPanel->getTreeWidget ( )->currentItem ( ));
	if (1 == index.column ( ))
	{
		QComboBox*	comboBox	= dynamic_cast<QComboBox*>(editor);
		qulonglong	mask		= 0;
		if (0 != editor)
		{
			QVariant	variant	= comboBox->itemData(comboBox->currentIndex( ));
			bool		ok		= false;
			mask				= variant.toULongLong (&ok);
		}	// if (0 != editor)

		if (0 != groupItem)
			groupItem->setRepresentationMask (mask);
		else if (0 != entityItem)
		{
			entityItem->setRepresentationMask (mask);
			// Les enfants ont ils tous le même masque d'affichage 3D ?
			QTreeWidgetItem*	parent	= entityItem->parent ( );
			CHECK_NULL_PTR_ERROR (parent)
			QtEntitiesGroupTreeWidgetItem*	groupItem	=
						dynamic_cast<QtEntitiesGroupTreeWidgetItem*>(parent);
			CHECK_NULL_PTR_ERROR (groupItem)
			unsigned long	mask	= entityItem->getRepresentationMask ( );
			for (int i = 0; i < parent->childCount ( ); i++)
			{
				QtEntityTreeWidgetItem*	ei	=
						dynamic_cast<QtEntityTreeWidgetItem*>(parent->child(i));
				if ((0 != ei) && (ei->getRepresentationMask ( ) != mask))
				{
					groupItem->setRepresentationMask ((unsigned long)-1);
					break;
				}	// if ((0 != ei) && (ei->getRepresentationMask ( ) != mask))
			}	// for (int i = 0; i < parent->childCount ( ); i++)
		}	// else if (0 != entityItem)
	}	// if (1 == index.column ( ))
}	// QtEntityTreeWidgetItemDelegate::setModelData

#endif	// REPRESENTATION_TYPE_BY_POPUP


// ===========================================================================
//                     LA CLASSE QtEntitiesTreeWidget
// ===========================================================================

QtEntitiesTreeWidget::QtEntitiesTreeWidget (QtEntitiesPanel& parent)
	: QTreeWidget (&parent), _entitiesPanel (&parent)
{
}	// QtEntitiesTreeWidget::QtEntitiesTreeWidget


QtEntitiesTreeWidget::QtEntitiesTreeWidget (const QtEntitiesTreeWidget&)
	: QTreeWidget (0), _entitiesPanel (0)
{
	MGX_FORBIDDEN ("QtEntitiesTreeWidget copy constructor is not allowed.");
}	// QtEntitiesTreeWidget::QtEntitiesTreeWidget


QtEntitiesTreeWidget& QtEntitiesTreeWidget::operator = (const QtEntitiesTreeWidget&)
{
	MGX_FORBIDDEN ("QtEntitiesTreeWidget::operator = is not allowed.");
	return *this;
}	// QtEntitiesTreeWidget::operator =


QtEntitiesTreeWidget::~QtEntitiesTreeWidget ( )
{
}	// QtEntitiesTreeWidget::~QtEntitiesTreeWidget


static vector<Entity*> itemListToEntities (vector<QTreeWidgetItem*>& selection)
{
	vector<Entity*>	entities;

	for (vector<QTreeWidgetItem*>::iterator it = selection.begin ( );
	     selection.end ( ) != it; it++)
	{
		QtEntityTreeWidgetItem*	item	=
							dynamic_cast<QtEntityTreeWidgetItem*>(*it);
		if ((0 != item) && (0 != item->getEntity ( )))
			entities.push_back (item->getEntity ( ));
	}	// for (vector<QTreeWidgetItem*>::iterator it = selection.begin ( ); ...

	return entities;
}	// itemListToEntities


void QtEntitiesTreeWidget::selectionChanged (const QItemSelection& selected, const QItemSelection & deselected)
{
	try
	{
		CHECK_NULL_PTR_ERROR (_entitiesPanel)

		QTreeWidget::selectionChanged (selected, deselected);
		vector<QTreeWidgetItem*>	selectedItems	=
											filteredSelectionList (selected);
		vector<QTreeWidgetItem*>	deselectedItems	=
											filteredSelectionList (deselected);
		vector<Entity*>				selectedEntities	=
											itemListToEntities(selectedItems);
		vector<Entity*>				deselectedEntities	=
											itemListToEntities(deselectedItems);

		_entitiesPanel->selectionChanged (selectedEntities, deselectedEntities);
	}
	catch (const Exception& exc)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Erreur lors de la modification de la sélection dans le "
		        << "panneau contenant l'arborescence des entités : "
		        << exc.getFullMessage ( ) << ".";
		_entitiesPanel->log (ErrorLog (message));
	}
	catch (...)
	{
		UTF8String	message (Charset::UTF_8);
		message << "Erreur lors de la modification de la sélection dans le "
		        << "panneau contenant l'arborescence des entités : "
		        << "erreur non documentée" << ".";
		_entitiesPanel->log (ErrorLog (message));
	}
}	// QtEntitiesTreeWidget::selectionChanged


vector<QTreeWidgetItem*> QtEntitiesTreeWidget::filteredSelectionList (const QItemSelection& list) const
{	// From Lem 3.3.1 :
	vector<QTreeWidgetItem*>    items;

	QModelIndexList indexes = list.indexes ( );
	for (size_t i = 0; i < indexes.count ( ); i++)
	{   // On est tenté de faire i += n, où n est le nombre de colonne -1, mais
		// on ne préjugera pas de l'ordre ...
		QTreeWidgetItem*    item    =
				dynamic_cast<QTreeWidgetItem*>(itemFromIndex (indexes [i]));
		if (0 == item)
		continue;

		// Cet item est il déjà recensé :
		bool    founded = false;
		for (vector<QTreeWidgetItem*>::const_iterator it = items.begin ( );
		items.end ( ) != it; it++)
		{
			if (*it == item)
			{
				founded = true;
				break;
			}   // if (*it == item)
		}   // for (vector<QTreeWidgetItem*>::const_iterator it = ..

		if (false == founded)
			items.push_back (item);
	}	// for (i = 0; i < indexes.count ( ); i++)

	return items;
}	// QtEntitiesTreeWidget::filteredSelectionList


// ===========================================================================
//                        LA CLASSE QTreeViewPopupActions
// ===========================================================================

QTreeViewPopupActions::QTreeViewPopupActions ( )
	: _representationMaskAction (0), _displayAction (0), _hideAction (0)
{
}	// QTreeViewPopupActions::QTreeViewPopupActions


QTreeViewPopupActions::QTreeViewPopupActions (const QTreeViewPopupActions& a)
	: _displayAction (a._displayAction), _hideAction (a._hideAction),
	  _representationMaskAction (a._representationMaskAction)
{
}	// QTreeViewPopupActions::QTreeViewPopupActions


QTreeViewPopupActions& QTreeViewPopupActions::operator = (const QTreeViewPopupActions& a)
{
	if (this != &a)
	{
		_displayAction				= a._displayAction;
		_hideAction					= a._hideAction;
		_representationMaskAction	= a._representationMaskAction;
	}	// if (this != &a)

	return *this;
}	// QTreeViewPopupActions::operator =


QTreeViewPopupActions::~QTreeViewPopupActions ( )
{
}	// QTreeViewPopupActions::~QTreeViewPopupActions


void QTreeViewPopupActions::setEnabled (bool enable)
{
	if (0 != _displayAction)
		_displayAction->setEnabled (enable);
	if (0 != _hideAction)
		_hideAction->setEnabled (enable);
	if (0 != _representationMaskAction)
		_representationMaskAction->setEnabled (enable);
}	// QTreeViewPopupActions::setEnabled


// ===========================================================================
//                        LA CLASSE QtEntitiesPanel
// ===========================================================================

QtEntitiesPanel::QtEntitiesPanel (
	QWidget* parent, QtMgx3DMainWindow* mainWindow, const string& name,
	ContextIfc& context)
	: QtEntitiesItemViewPanel (parent, mainWindow, name, context),
	  SelectionManagerObserver (&context.getSelectionManager ( )),
	  _entitiesWidget (0),
	  _cadItem (0), _topologyItem (0), _meshingItem (0), _coordinateSystemItem(0), _structuredMeshingItem (0),
	  _cadVolumesItem (0), _cadSurfacesItem (0), _cadCurvesItem (0),
	  _cadCloudsItem (0),
	  _topoBlocksItem (0), _topoFacesItem (0), _topoEdgesItem (0),
	  _topoVerticesItem (0),
	  _meshVolumesItem (0), _meshSurfacesItem (0), _meshLinesItem (0),
	  _meshCloudsItem (0), _sysCoordItem(0), _structuredMeshVolumesItem (0),
	  _treeViewPopupMenu (0), _treeViewActions ( )
{
	createGui ( );
	createPopupMenu ( );

	context.getSelectionManager ( ).addSelectionObserver (*this);
}	// QtEntitiesPanel::QtEntitiesPanel


QtEntitiesPanel::QtEntitiesPanel (const QtEntitiesPanel& ep)
	: QtEntitiesItemViewPanel (ep), SelectionManagerObserver (0), 
	  _entitiesWidget (0),
	  _cadItem (0), _topologyItem (0), _meshingItem (0), _coordinateSystemItem(0), _structuredMeshingItem (0),
	  _cadVolumesItem (0), _cadSurfacesItem (0), _cadCurvesItem (0),
	  _cadCloudsItem (0),
	  _topoBlocksItem (0), _topoFacesItem (0), _topoEdgesItem (0),
	  _topoVerticesItem (0),
	  _meshVolumesItem (0), _meshSurfacesItem (0), _meshLinesItem (0),
	  _meshCloudsItem (0), _sysCoordItem(0), _structuredMeshVolumesItem (0),
	  _treeViewPopupMenu (0), _treeViewActions ( )
{
	MGX_FORBIDDEN ("QtEntitiesPanel copy constructor is not allowed.");
}	// QtEntitiesPanel::QtEntitiesPanel (const QtEntitiesPanel&)


QtEntitiesPanel& QtEntitiesPanel::operator = (const QtEntitiesPanel&)
{
	MGX_FORBIDDEN ("QtEntitiesPanel assignment operator is not allowed.");
	return *this;
}	// QtEntitiesPanel::QtEntitiesPanel (const QtEntitiesPanel&)


QtEntitiesPanel::~QtEntitiesPanel ( )
{
}	// QtEntitiesPanel::~QtEntitiesPanel


void QtEntitiesPanel::writeSettings (QSettings& settings)
{
	settings.beginGroup ("EntitiesPanel");
	settings.setValue ("size", size ( ));
	settings.endGroup ( );
}	// QtEntitiesPanel::writeSettings


void QtEntitiesPanel::readSettings (QSettings& settings)
{
	settings.beginGroup ("EntitiesPanel");
	resize (settings.value ("size", size ( )).toSize ( ));
	settings.endGroup ( );
}	// QtEntitiesPanel::readSettings


void QtEntitiesPanel::addCADEntity (Geom::GeomEntity& entity, bool show)
{
assert (0 && "QtEntitiesPanel::addCADEntity is deprecated");
	QTreeWidgetItem*	item	= createCADItem (entity);
	if (0 != getGraphicalWidget ( ))
	{
		QtEntityTreeWidgetItem*	entityItem	=
									dynamic_cast<QtEntityTreeWidgetItem*>(item);
		CHECK_NULL_PTR_ERROR (entityItem)
		getGraphicalWidget ( )->getRenderingManager ( ).addEntity (entity);
		// Avant addEntity entity.getDisplayProperties ( ).getGraphicalRepresentation ( )
		// est nul, et c'est l'item qui décide de la représentation de base
		// de l'entité. On ne fait le setRepresentationMask donc que maintenant:
//		entity.setRepresentationMask (	// TO REMOVE
//								entityItem->getNativeRepresentationMask ( ));
		item->setCheckState (0, true == show ? Qt::Checked : Qt::Unchecked);
		// C'est dommage mais item->setCheckState n'informe pas le tree widget :
		if (true == itemCreationTriggersDisplayCallback ( ))
			stateChangeCallback (item, 0);
	}	// if (0 != getGraphicalWidget ( ))
}	// QtEntitiesPanel::addCADEntity


void QtEntitiesPanel::addCADEntities (
						const vector<Geom::GeomEntity*>& entities, bool show)
{
	vector<Utils::Entity*>		list;
	vector<QTreeWidgetItem*>	itemList;

	for (vector<Geom::GeomEntity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		list.push_back (*it);
		QTreeWidgetItem*	item	= createCADItem (**it);
		item->setCheckState (0, true == show ? Qt::Checked : Qt::Unchecked);
		itemList.push_back (item);
		// C'est dommage mais item->setCheckState n'informe pas le tree widget :
//		if (true == itemCreationTriggersDisplayCallback ( ))
//			stateChangeCallback (item, 0);
//			itemStateChange (item, false);	// CP MODIF du 20/09/12
	}	// for (vector<Geom::GeomEntity*>::const_iterator it = entities.begin ( ); ...
	if (0 != entities.size ( ))
	{
		if (true == itemCreationTriggersDisplayCallback ( ))	// CP 27/09/12
			itemsStateChange (itemList, false);	// CP MODIF du 20/09/12
		updateEntryItems (DisplayRepresentation::DISPLAY_GEOM);
	}

	if (0 != getGraphicalWidget ( ))
		getGraphicalWidget ( )->getRenderingManager ( ).addEntities (
							list, show, DisplayRepresentation::DISPLAY_GEOM);
}	// QtEntitiesPanel::addCADEntities


void QtEntitiesPanel::removeCADEntity (Geom::GeomEntity& entity)
{
assert (0 && "QtEntitiesPanel::removeCADEntity is deprecated");
	QTreeWidgetItem*	parent	= getCADEntryItem (entity.getDim ( ));
	CHECK_NULL_PTR_ERROR (parent)
	for (size_t i = 0; i < parent->childCount ( ); i++)
	{
		QtCADEntityTreeWidgetItem*	item	=
				dynamic_cast<QtCADEntityTreeWidgetItem*>(parent->child (i));	
		if ((0 != item) && (&entity == item->getEntity ( )))
		{
			if (0 != getGraphicalWidget ( ))
				getGraphicalWidget ( )->getRenderingManager ( ).removeEntity (entity);
			parent->takeChild (i);
			delete item;
			break;
		}	// if ((0 != item) && (&entity == item->getEntity ( )))
	}	// for (size_t i = 0; i < parent->childCount ( ); i++)
}	// QtEntitiesPanel::removeCADEntity


void QtEntitiesPanel::removeCADEntities (
									const vector<Geom::GeomEntity*>& entities)
{
	vector<Utils::Entity*>	list;

	for (vector<Geom::GeomEntity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		list.push_back (*it);
		QTreeWidgetItem*	parent	= getCADEntryItem ((*it)->getDim ( ));
		CHECK_NULL_PTR_ERROR (parent)
		for (size_t i = 0; i < parent->childCount ( ); i++)
		{
			QtCADEntityTreeWidgetItem*	item	=
				dynamic_cast<QtCADEntityTreeWidgetItem*>(parent->child (i));	
			if ((0 != item) && (*it == item->getEntity ( )))
			{
				parent->takeChild (i);
				delete item;
				break;
			}	// if ((0 != item) && (&entity == item->getEntity ( )))
		}	// for (size_t i = 0; i < parent->childCount ( ); i++)
	}	// for (vector<Geom::GeomEntity*>::const_iterator it = entities.begin ( );
	if (0 != entities.size ( ))
		updateEntryItems (DisplayRepresentation::DISPLAY_GEOM);

	if (0 != getGraphicalWidget ( ))
		getGraphicalWidget ( )->getRenderingManager ( ).removeEntities (
										list, DisplayRepresentation::DISPLAY_GEOM);
}	// QtEntitiesPanel::removeCADEntities


void QtEntitiesPanel::addTopologicEntity (Topo::TopoEntity& entity, bool show)
{
assert (0 && "QtEntitiesPanel::addTopologicEntity is deprecated");
	QTreeWidgetItem*	item	= createTopologicItem (entity);
	if (0 != getGraphicalWidget ( ))
	{
		QtEntityTreeWidgetItem*	entityItem	=
									dynamic_cast<QtEntityTreeWidgetItem*>(item);
		CHECK_NULL_PTR_ERROR (entityItem)
		getGraphicalWidget ( )->getRenderingManager ( ).addEntity (entity);
		// Avant addEntity entity.getDisplayProperties ( ).getGraphicalRepresentation ( )
		// est nul, et c'est l'item qui décide de la représentation de base
		// de l'entité. On ne fait le setRepresentationMask donc que maintenant:
//		entity.setRepresentationMask (	// TO REMOVE
//								entityItem->getNativeRepresentationMask ( ));
		item->setCheckState (0, true == show ? Qt::Checked : Qt::Unchecked);
		// C'est dommage mais item->setCheckState n'informe pas le tree widget :
		if (true == itemCreationTriggersDisplayCallback ( ))
			stateChangeCallback (item, 0);
	}	// if (0 != getGraphicalWidget ( ))
}	// QtEntitiesPanel::addTopologicEntity


void QtEntitiesPanel::addTopologicEntities (
						const vector<Topo::TopoEntity*>& entities, bool show)
{
	vector<Utils::Entity*>		list;
	vector<QTreeWidgetItem*>	itemList;

	for (vector<Topo::TopoEntity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		list.push_back (*it);
		QTreeWidgetItem*	item	= createTopologicItem (**it);
		//std::cout<<"addTopologicEntities => "<<(*it)->getName()<<(show?" visible":" invisible")<<std::endl;
		item->setCheckState (0, true == show ? Qt::Checked : Qt::Unchecked);
		itemList.push_back (item);
		// C'est dommage mais item->setCheckState n'informe pas le tree widget :
//		if (true == itemCreationTriggersDisplayCallback ( ))
//			stateChangeCallback (item, 0);
//			itemStateChange (item, false);	// CP MODIF du 20/09/12
	}	// for (vector<Topo::TopoEntity*>::const_iterator it = entities.begin ( ); ...
	if (0 != entities.size ( ))
	{
		itemsStateChange (itemList, false);	// // CP MODIF du 20/09/12
		updateEntryItems (DisplayRepresentation::DISPLAY_TOPO);
	}

	if (0 != getGraphicalWidget ( ))
		getGraphicalWidget ( )->getRenderingManager ( ).addEntities (
							list, show, DisplayRepresentation::DISPLAY_TOPO);
}	// QtEntitiesPanel::addTopologicEntities


void QtEntitiesPanel::removeTopologicEntity (Topo::TopoEntity& entity)
{
assert (0 && "QtEntitiesPanel::removeTopologicEntity is deprecated");
	QTreeWidgetItem*	parent	= getTopologicEntryItem (entity.getDim ( ));
	CHECK_NULL_PTR_ERROR (parent)
	for (size_t i = 0; i < parent->childCount ( ); i++)
	{
		QtTopologicEntityTreeWidgetItem*	item	=
			dynamic_cast<QtTopologicEntityTreeWidgetItem*>(parent->child (i));	
		if ((0 != item) && (&entity == item->getEntity ( )))
		{
			if (0 != getGraphicalWidget ( ))
				getGraphicalWidget ( )->getRenderingManager ( ).removeEntity (entity);
			parent->takeChild (i);
			delete item;
			break;
		}	// if ((0 != item) && (&entity == item->getEntity ( )))
	}	// for (size_t i = 0; i < parent->childCount ( ); i++)
}	// QtEntitiesPanel::removeTopologicEntity


void QtEntitiesPanel::removeTopologicEntities (
									const vector<Topo::TopoEntity*>& entities)
{
	vector<Utils::Entity*>	list;

	for (vector<Topo::TopoEntity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		list.push_back (*it);
		QTreeWidgetItem*	parent	= getTopologicEntryItem ((*it)->getDim ( ));
		CHECK_NULL_PTR_ERROR (parent)
		for (size_t i = 0; i < parent->childCount ( ); i++)
		{
			QtTopologicEntityTreeWidgetItem*	item	=
				dynamic_cast<QtTopologicEntityTreeWidgetItem*>(parent->child (i));	
			if ((0 != item) && (*it == item->getEntity ( )))
			{
				parent->takeChild (i);
				delete item;
				break;
			}	// if ((0 != item) && (&entity == item->getEntity ( )))
		}	// for (size_t i = 0; i < parent->childCount ( ); i++)
	}	// for (vector<Topo::TopoEntity*>::const_iterator it = entities.begin ( );
	if (0 != entities.size ( ))
		updateEntryItems (DisplayRepresentation::DISPLAY_TOPO);

	if (0 != getGraphicalWidget ( ))
		getGraphicalWidget ( )->getRenderingManager ( ).removeEntities (
									list, DisplayRepresentation::DISPLAY_TOPO);
}	// QtEntitiesPanel::removeTopologicEntities


void QtEntitiesPanel::addMeshEntity (Mesh::MeshEntity& entity, bool show)
{
assert (0 && "QtEntitiesPanel::addMeshEntity is deprecated");
	QTreeWidgetItem*	item	= createMeshItem (entity);
	if (0 != getGraphicalWidget ( ))
	{
		QtEntityTreeWidgetItem*	entityItem	=
									dynamic_cast<QtEntityTreeWidgetItem*>(item);
		CHECK_NULL_PTR_ERROR (entityItem)
		getGraphicalWidget ( )->getRenderingManager ( ).addEntity (entity);
		// Avant addEntity entity.getDisplayProperties ( ).getGraphicalRepresentation ( )
		// est nul, et c'est l'item qui décide de la représentation de base
		// de l'entité. On ne fait le setRepresentationMask donc que maintenant:
//		entity.setRepresentationMask (	// TO REMOVE
//								entityItem->getNativeRepresentationMask ( ));
		item->setCheckState (0, true == show ? Qt::Checked : Qt::Unchecked);
		// C'est dommage mais item->setCheckState n'informe pas le tree widget :
		if (true == itemCreationTriggersDisplayCallback ( ))
			stateChangeCallback (item, 0);
	}	// if (0 != getGraphicalWidget ( ))
}	// QtEntitiesPanel::addMeshEntity


void QtEntitiesPanel::addMeshEntities (
						const vector<Mesh::MeshEntity*>& entities, bool show)
{
	vector<Utils::Entity*>	list;
	vector<QTreeWidgetItem*>	itemList;

	for (vector<Mesh::MeshEntity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		list.push_back (*it);
		QTreeWidgetItem*	item	= createMeshItem (**it);
		itemList.push_back (item);
		item->setCheckState (0, true == show ? Qt::Checked : Qt::Unchecked);
		// C'est dommage mais item->setCheckState n'informe pas le tree widget :
//		if (true == itemCreationTriggersDisplayCallback ( ))
//			stateChangeCallback (item, 0);
	}	// for (vector<Mesh::MeshEntity*>::const_iterator it = entities.begin ( ); ...
	if (0 != entities.size ( ))
	{
		itemsStateChange (itemList, false);	// // CP MODIF du 20/09/12
		updateEntryItems (DisplayRepresentation::DISPLAY_MESH);
	}

	if (0 != getGraphicalWidget ( ))
		getGraphicalWidget ( )->getRenderingManager ( ).addEntities (
							list, show, DisplayRepresentation::DISPLAY_MESH);
}	// QtEntitiesPanel::addMeshEntities


void QtEntitiesPanel::removeMeshEntity (Mesh::MeshEntity& entity)
{
assert (0 && "QtEntitiesPanel::removeMeshEntity is deprecated");
	QTreeWidgetItem*	parent	= getMeshEntryItem (entity.getDim ( ));
	CHECK_NULL_PTR_ERROR (parent)
	for (size_t i = 0; i < parent->childCount ( ); i++)
	{
		QtMeshEntityTreeWidgetItem*	item	=
			dynamic_cast<QtMeshEntityTreeWidgetItem*>(parent->child (i));	
		if ((0 != item) && (&entity == item->getEntity ( )))
		{
			if (0 != getGraphicalWidget ( ))
				getGraphicalWidget ( )->getRenderingManager ( ).removeEntity (entity);
			parent->takeChild (i);
			delete item;
			break;
		}	// if ((0 != item) && (&entity == item->getEntity ( )))
	}	// for (size_t i = 0; i < parent->childCount ( ); i++)
}	// QtEntitiesPanel::removeMeshEntity


void QtEntitiesPanel::removeMeshEntities (const vector<Mesh::MeshEntity*>& entities)
{
	vector<Utils::Entity*>	list;

	for (vector<Mesh::MeshEntity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		list.push_back (*it);
		QTreeWidgetItem*	parent	= getMeshEntryItem ((*it)->getDim ( ));
		CHECK_NULL_PTR_ERROR (parent)
		for (size_t i = 0; i < parent->childCount ( ); i++)
		{
			QtMeshEntityTreeWidgetItem*	item	=
				dynamic_cast<QtMeshEntityTreeWidgetItem*>(parent->child (i));	
			if ((0 != item) && (*it == item->getEntity ( )))
			{
				parent->takeChild (i);
				delete item;
				break;
			}	// if ((0 != item) && (&entity == item->getEntity ( )))
		}	// for (size_t i = 0; i < parent->childCount ( ); i++)
	}	// for (vector<Geom::MeshEntity*>::const_iterator it = entities.begin ( );
	if (0 != entities.size ( ))
		updateEntryItems (DisplayRepresentation::DISPLAY_MESH);

	if (0 != getGraphicalWidget ( ))
		getGraphicalWidget ( )->getRenderingManager ( ).removeEntities (
										list, DisplayRepresentation::DISPLAY_MESH);
}	// QtEntitiesPanel::removeMeshEntities


void QtEntitiesPanel::addSysCoordEntity (Mgx3D::CoordinateSystem::SysCoord& entity, bool show)
{
	QTreeWidgetItem*	item	= createSysCoordItem (entity);
	if (0 != getGraphicalWidget ( ))
	{
		QtEntityTreeWidgetItem*	entityItem	=
									dynamic_cast<QtEntityTreeWidgetItem*>(item);
		CHECK_NULL_PTR_ERROR (entityItem)
		getGraphicalWidget ( )->getRenderingManager ( ).addEntity (entity);
		item->setCheckState (0, true == show ? Qt::Checked : Qt::Unchecked);
		if (true == itemCreationTriggersDisplayCallback ( ))
			stateChangeCallback (item, 0);
	}	// if (0 != getGraphicalWidget ( ))
}	// QtEntitiesPanel::addSysCoordEntity

void QtEntitiesPanel::removeSysCoordEntity (Mgx3D::CoordinateSystem::SysCoord& entity)
{
	QTreeWidgetItem*	parent	= getSysCoordEntryItem ();
	CHECK_NULL_PTR_ERROR (parent)
	for (size_t i = 0; i < parent->childCount ( ); i++)
	{
		QtSysCoordEntityTreeWidgetItem*	item	=
				dynamic_cast<QtSysCoordEntityTreeWidgetItem*>(parent->child (i));
		if ((0 != item) && (&entity == item->getEntity ( )))
		{
			if (0 != getGraphicalWidget ( ))
				getGraphicalWidget ( )->getRenderingManager ( ).removeEntity (entity);
			parent->takeChild (i);
			delete item;
			break;
		}	// if ((0 != item) && (&entity == item->getEntity ( )))
	}	// for (size_t i = 0; i < parent->childCount ( ); i++)
}	// QtEntitiesPanel::removeSysCoordEntity


void QtEntitiesPanel::addStructuredMeshEntity (Structured::StructuredMeshEntity& entity, bool show)
{
assert (0 && "QtEntitiesPanel::addMeshEntity is deprecated");
	QTreeWidgetItem*	item	= createStructuredMeshItem (entity);
	if (0 != getGraphicalWidget ( ))
	{
		QtEntityTreeWidgetItem*	entityItem	= dynamic_cast<QtEntityTreeWidgetItem*>(item);
		CHECK_NULL_PTR_ERROR (entityItem)
		getGraphicalWidget ( )->getRenderingManager ( ).addEntity (entity);
		// Avant addEntity entity.getDisplayProperties ( ).getGraphicalRepresentation ( )
		// est nul, et c'est l'item qui décide de la représentation de base
		// de l'entité. On ne fait le setRepresentationMask donc que maintenant:
//		entity.setRepresentationMask (	// TO REMOVE
//								entityItem->getNativeRepresentationMask ( ));
		item->setCheckState (0, true == show ? Qt::Checked : Qt::Unchecked);
		// C'est dommage mais item->setCheckState n'informe pas le tree widget :
		if (true == itemCreationTriggersDisplayCallback ( ))
			stateChangeCallback (item, 0);
	}	// if (0 != getGraphicalWidget ( ))
}	// QtEntitiesPanel::addStructuredMeshEntity


void QtEntitiesPanel::addStructuredMeshEntities (const vector<Structured::StructuredMeshEntity*>& entities, bool show)
{
	vector<Utils::Entity*>		list;
	vector<QTreeWidgetItem*>	itemList;

	for (vector<Structured::StructuredMeshEntity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		list.push_back (*it);
		QTreeWidgetItem*	item	= createStructuredMeshItem (**it);
		itemList.push_back (item);
		item->setCheckState (0, true == show ? Qt::Checked : Qt::Unchecked);
		// C'est dommage mais item->setCheckState n'informe pas le tree widget :
		if (true == itemCreationTriggersDisplayCallback ( ))
			stateChangeCallback (item, 0);
	}	// for (vector<Structured::StructuredMeshEntity*>::const_iterator it = entities.begin ( ); ...
	if (0 != entities.size ( ))
	{
		itemsStateChange (itemList, false);	// // CP MODIF du 20/09/12
		updateEntryItems (DisplayRepresentation::DISPLAY_STRUCTURED_MESH);
	}

	if (0 != getGraphicalWidget ( ))
		getGraphicalWidget ( )->getRenderingManager ( ).addEntities (list, show, DisplayRepresentation::DISPLAY_STRUCTURED_MESH);
}	// QtEntitiesPanel::addStructuredMeshEntities


void QtEntitiesPanel::removeStructuredMeshEntity (Structured::StructuredMeshEntity& entity)
{
assert (0 && "QtEntitiesPanel::removeStructuredMeshEntity is deprecated");
	QTreeWidgetItem*	parent	= getStructuredMeshEntryItem (entity.getDim ( ));
	CHECK_NULL_PTR_ERROR (parent)
	for (size_t i = 0; i < parent->childCount ( ); i++)
	{
		QtStructuredMeshEntityTreeWidgetItem*	item	=
			dynamic_cast<QtStructuredMeshEntityTreeWidgetItem*>(parent->child (i));	
		if ((0 != item) && (&entity == item->getEntity ( )))
		{
			if (0 != getGraphicalWidget ( ))
				getGraphicalWidget ( )->getRenderingManager ( ).removeEntity (entity);
			parent->takeChild (i);
			delete item;
			break;
		}	// if ((0 != item) && (&entity == item->getEntity ( )))
	}	// for (size_t i = 0; i < parent->childCount ( ); i++)
}	// QtEntitiesPanel::removeStructuredMeshEntity


void QtEntitiesPanel::removeStructuredMeshEntities (const vector<Structured::StructuredMeshEntity*>& entities)
{
	vector<Utils::Entity*>	list;

	for (vector<Structured::StructuredMeshEntity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		list.push_back (*it);
		QTreeWidgetItem*	parent	= getStructuredMeshEntryItem ((*it)->getDim ( ));
		CHECK_NULL_PTR_ERROR (parent)
		for (size_t i = 0; i < parent->childCount ( ); i++)
		{
			QtStructuredMeshEntityTreeWidgetItem*	item	=
				dynamic_cast<QtStructuredMeshEntityTreeWidgetItem*>(parent->child (i));	
			if ((0 != item) && (*it == item->getEntity ( )))
			{
				parent->takeChild (i);
				delete item;
				break;
			}	// if ((0 != item) && (&entity == item->getEntity ( )))
		}	// for (size_t i = 0; i < parent->childCount ( ); i++)
	}	// for (vector<Structured::StructuredMeshEntity*>::const_iterator it = entities.begin ( );
	if (0 != entities.size ( ))
		updateEntryItems (DisplayRepresentation::DISPLAY_STRUCTURED_MESH);

	if (0 != getGraphicalWidget ( ))
		getGraphicalWidget ( )->getRenderingManager ( ).removeEntities (list, DisplayRepresentation::DISPLAY_STRUCTURED_MESH);
}	// QtEntitiesPanel::removeMeshEntities


void QtEntitiesPanel::updateEntryItems (DisplayRepresentation::display_type type)
{
	assert (0 != _cadVolumesItem);
	assert (0 != _cadSurfacesItem);
	assert (0 != _cadCurvesItem);
	assert (0 != _cadCloudsItem);
	assert (0 != _topoBlocksItem);
	assert (0 != _topoFacesItem);
	assert (0 != _topoEdgesItem);
	assert (0 != _topoVerticesItem);
	assert (0 != _meshVolumesItem);
	assert (0 != _meshSurfacesItem);
	assert (0 != _meshLinesItem);
	assert (0 != _meshCloudsItem);
	assert (0 != _sysCoordItem);
	switch (type)
	{
		case DisplayRepresentation::DISPLAY_GEOM		:
		updateEntryItem (*_cadVolumesItem);
		updateEntryItem (*_cadSurfacesItem);
		updateEntryItem (*_cadCurvesItem);
		updateEntryItem (*_cadCloudsItem);
		break;
		case DisplayRepresentation::DISPLAY_TOPO		:
		updateEntryItem (*_topoBlocksItem);
		updateEntryItem (*_topoFacesItem);
		updateEntryItem (*_topoEdgesItem);
		updateEntryItem (*_topoVerticesItem);
		break;
		case DisplayRepresentation::DISPLAY_MESH		:
		updateEntryItem (*_meshVolumesItem);
		updateEntryItem (*_meshSurfacesItem);
		updateEntryItem (*_meshLinesItem);
		updateEntryItem (*_meshCloudsItem);
		break;
		case DisplayRepresentation::DISPLAY_SYSCOORD		:
		updateEntryItem (*_sysCoordItem);
		case DisplayRepresentation::DISPLAY_STRUCTURED_MESH	:
		if (0 != _structuredMeshVolumesItem)
			updateEntryItem (*_structuredMeshVolumesItem);
		break;
	}	// switch (type)
}	// QtEntitiesPanel::updateEntryItems


void QtEntitiesPanel::entitiesAddedToSelection (const vector<Entity*>& entities)
{
	for (vector<Entity*>::const_iterator it = entities.begin ( );
	     entities.end ( ) != it; it++)
	{
		QtEntityTreeWidgetItem*	item	= getItem (**it);
		if (0 != item)
		{
			if (false == item->isSelected ( ))
			{
				QItemSelectionModel*	selectionModel	=
					0 == _entitiesWidget ? 0 : _entitiesWidget->selectionModel ( );
				QtObjectSignalBlocker	blocker (selectionModel);
				item->setSelected (true);
				if (0 != _entitiesWidget)
					_entitiesWidget->viewport ( )->update ( );
			}	// if (false == item->isSelected ( ))
		}	// if (0 != item)
	}	// for (vector<Entity*>::const_iterator it = entities.begin ( ); ...
}	// QtEntitiesPanel::entitiesAddedToSelection


void QtEntitiesPanel::entitiesRemovedFromSelection (
									const vector<Entity*>& entities, bool clear)
{
	// CP : protection par blockSignals car la désélection peut être effective,
	// provenir d'ailleurs (ex : SelectionManager::clearSelection). Si on ne
	// fait rien le setSelected (false) envoit des signaux qui seront reçus
	// pour certains à ce widget qui demandera alors la désélection de entity
	// => exception levée alors qu'elle n'a pas lieu d'être.
	QItemSelectionModel*	selectionModel	=
				0 == _entitiesWidget ? 0 : _entitiesWidget->selectionModel ( );
	QtObjectSignalBlocker	blocker (selectionModel);

	if (true == clear)	// Optimisation
	{
//		_entitiesWidget->clearSelection ( );	// révision svn 1855
		if (0 != _entitiesWidget)				// révision svn 1855
		{
			// _entitiesWidget->clearSelection ( ) efface la sélection courante
			// mais item->isSelected ( ) retourne toujours true si 
			// blocker (selectionModel) est instancié. Et on en a besoin
			// => désélection comme ci-dessous ...
			QtObjectSignalBlocker	blocker (_entitiesWidget);
			QList<QTreeWidgetItem*>	selection	=
										_entitiesWidget->selectedItems ( );
			for (int i = 0; i < selection.count ( ); i++)
				selection [i]->setSelected (false);
		}	// if (0 != _entitiesWidget)
	}	// if (true == clear)
	else
	{
		for (vector<Entity*>::const_iterator it = entities.begin ( );
		     entities.end ( ) != it; it++)
		{
			QtEntityTreeWidgetItem*	item	= getItem (**it);
			if (0 != item)
			{
				if (true == item->isSelected ( ))
					item->setSelected (false);
			}	// if (0 != item)
		}	// for (vector<Entity*>::const_iterator it = entities.begin ( ); ...
	}	// else if (true == clear)

	if (0 != _entitiesWidget)
		_entitiesWidget->viewport ( )->update ( );
}	// QtEntitiesPanel::entitiesRemovedFromSelection


void QtEntitiesPanel::selectionCleared ( )
{
	// Requiert un changement de thread, mais entityRemovedFromSelection
	// est appelé pour chaque entité désélectionnée.
//	if (0 != _entitiesWidget)
//		_entitiesWidget->clearSelection ( );
}	// QtEntitiesPanel::selectionCleared


FilterEntity::objectType QtEntitiesPanel::getSelectedEntitiesTypes ( ) const
{
	CHECK_NULL_PTR_ERROR (_entitiesWidget)
	FilterEntity::objectType	types	= FilterEntity::NoneEntity;

	QList<QTreeWidgetItem*>	selectedItems	=
										_entitiesWidget->selectedItems ( );
	for (size_t i = 0; i < selectedItems.count ( ); i++)
	{
		QTreeWidgetItem*		item	= selectedItems [i];
		QtEntityTreeWidgetItem*	entityItem	=
									dynamic_cast<QtEntityTreeWidgetItem*>(item);
		if (0 != entityItem)
		{
			if (0 != entityItem->getEntity ( ))
				types	= (FilterEntity::objectType)(
						types | entityItem->getEntity ( )->getFilteredType ( ));
		}	// if (0 != entityItem)
		else
		{	// Ensemble d'entités ?
			FilterEntity::objectType	type	= getFilteredType (item);
			types	= (FilterEntity::objectType)(types | type);
		}	// else if (0 != entityItem)
	}	// for (size_t i = 0; i < selectedItems.count ( ); i++)

	return types;
}	// QtEntitiesPanel::getSelectedEntitiesTypes


void QtEntitiesPanel::setGraphicalWidget (Qt3DGraphicalWidget* widget3D)
{	
	QtEntitiesItemViewPanel::setGraphicalWidget (widget3D);

	if (0 != getGraphicalWidget ( ))
	{
		CHECK_NULL_PTR_ERROR (getSelectionManager ( ))
		getGraphicalWidget ( )->getRenderingManager ( ).setSelectionManager (getSelectionManager ( ));
	}	// if (0 != getGraphicalWidget ( ))
}	// QtEntitiesPanel::setGraphicalWidget


QtEntitiesTreeWidget* QtEntitiesPanel::getTreeWidget ( )
{
	return _entitiesWidget;
}	// QtEntitiesPanel::getTreeWidget


const QtEntitiesTreeWidget* QtEntitiesPanel::getTreeWidget ( ) const
{
	return _entitiesWidget;
}	// QtEntitiesPanel::getTreeWidget


void QtEntitiesPanel::createGui ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	if (0 != _entitiesWidget)
	{
		INTERNAL_ERROR (exc,
				"QtEntitiesPanel::createGui", "IHM déjà initialisée.")
		throw exc;
	}	// if (0 != _entitiesWidget)
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (QtConfiguration::margin);
	layout->setSpacing (QtConfiguration::spacing);
	setLayout (layout);
	layout->setSizeConstraint (QLayout::SetMinimumSize);

	_entitiesWidget	= new QtEntitiesTreeWidget (*this);
	layout->addWidget (_entitiesWidget);
	connect (_entitiesWidget, SIGNAL (itemClicked (QTreeWidgetItem*, int)),
	         this, SLOT (stateChangeCallback (QTreeWidgetItem*, int)));
//_entitiesWidget->setItemsExpandable (true);
//_entitiesWidget->setRootIsDecorated (true);
	_entitiesWidget->setSelectionBehavior (QTreeWidget::SelectRows);
	_entitiesWidget->setSelectionMode (QTreeWidget::ExtendedSelection);
	_entitiesWidget->setSortingEnabled (false);
	QAbstractItemModel*	treeModel	= _entitiesWidget->model ( );
#ifdef REPRESENTATION_TYPE_BY_POPUP
	_entitiesWidget->setColumnCount (1);
#else
	_entitiesWidget->setColumnCount (2);
	_entitiesWidget->setColumnWidth (1, 100);
	treeModel->setHeaderData (1, Qt::Horizontal, QVariant ("Affichage 3D"));
#endif	// REPRESENTATION_TYPE_BY_POPUP
	_entitiesWidget->setColumnWidth (0, 200);
	CHECK_NULL_PTR_ERROR (treeModel)
	treeModel->setHeaderData (0, Qt::Horizontal, QVariant ("Entités"));

	// Les entrées de l'arbre :
	// C.A.O. :
	_cadItem	= new QTreeWidgetItem (_entitiesWidget);
	_cadItem->setText (0, "C.A.O.");
	_cadItem->setChildIndicatorPolicy (QTreeWidgetItem::ShowIndicator);
	_entitiesWidget->addTopLevelItem (_cadItem);
	_cadVolumesItem		= createEntryItem (*_cadItem, "Volumes",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::GeomVolume), DisplayRepresentation::DISPLAY_GEOM);
	_cadSurfacesItem	= createEntryItem (*_cadItem, "Surfaces",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::GeomSurface), DisplayRepresentation::DISPLAY_GEOM);
	_cadCurvesItem		= createEntryItem (*_cadItem, "Courbes",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::GeomCurve), DisplayRepresentation::DISPLAY_GEOM);
	_cadCloudsItem		= createEntryItem (*_cadItem, "Points",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::GeomVertex), DisplayRepresentation::DISPLAY_GEOM);
	// Topologie :
	_topologyItem	= new QTreeWidgetItem (_entitiesWidget);
	_topologyItem->setText (0, "Topologie");
	_entitiesWidget->addTopLevelItem (_topologyItem);
	_topoBlocksItem		= createEntryItem (*_topologyItem, "Blocs",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::TopoBlock), DisplayRepresentation::DISPLAY_TOPO);
	_topoFacesItem	= createEntryItem (*_topologyItem, "Faces",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::TopoFace), DisplayRepresentation::DISPLAY_TOPO);
	_topoEdgesItem		= createEntryItem (*_topologyItem, "Arêtes",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::TopoEdge), DisplayRepresentation::DISPLAY_TOPO);
	_topoVerticesItem		= createEntryItem (*_topologyItem, "Sommets",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::TopoVertex), DisplayRepresentation::DISPLAY_TOPO);
	// Maillage :
	_meshingItem	= new QTreeWidgetItem (_entitiesWidget);
	_meshingItem->setText (0, "Maillage");
	_entitiesWidget->addTopLevelItem (_meshingItem);
	_meshVolumesItem		= createEntryItem (*_meshingItem, "Volumes",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::MeshVolume), DisplayRepresentation::DISPLAY_MESH);
	_meshSurfacesItem	= createEntryItem (*_meshingItem, "Surfaces",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::MeshSurface), DisplayRepresentation::DISPLAY_MESH);
	_meshLinesItem		= createEntryItem (*_meshingItem, "Lignes",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::MeshLine), DisplayRepresentation::DISPLAY_MESH);
	_meshCloudsItem		= createEntryItem (*_meshingItem, "Nuages",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::MeshCloud), DisplayRepresentation::DISPLAY_MESH);

	// Repère :
	_coordinateSystemItem	= new QTreeWidgetItem (_entitiesWidget);
	_coordinateSystemItem->setText (0, "Repères");
	_entitiesWidget->addTopLevelItem (_coordinateSystemItem);
	_sysCoordItem  = createEntryItem (*_coordinateSystemItem, "Repères", 
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::SysCoord),	DisplayRepresentation::DISPLAY_SYSCOORD);

	// Maillages structurés :
	_structuredMeshingItem	= new QTreeWidgetItem (_entitiesWidget);
	_structuredMeshingItem->setText (0, "Maillage structuré");
	_entitiesWidget->addTopLevelItem (_structuredMeshingItem);
	_structuredMeshVolumesItem= createEntryItem (*_structuredMeshingItem, "Volumes",
				GraphicalEntityRepresentation::getDefaultRepresentationMask (Entity::StructuredMesh), DisplayRepresentation::DISPLAY_STRUCTURED_MESH);

#ifndef REPRESENTATION_TYPE_BY_POPUP
	// ItemDelegate : à faire une fois que l'arborescence ext créée, dixit Qt :
	_entitiesWidget->setItemDelegate (
			new QtEntitiesTreeWidget::QtEntityTreeWidgetItemDelegate (this));
#endif	// REPRESENTATION_TYPE_BY_POPUP

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtEntitiesPanel::createGui


void QtEntitiesPanel::createPopupMenu ( )
{
	// Forcer l'affichage d'un menu contextuel :
	assert (0 != _entitiesWidget);
	_treeViewPopupMenu	= new QMenu (_entitiesWidget);
	// Affichage :
	//QMenu*	entry	= new QMenu ("Affichage ...", _treeViewPopupMenu); [EB] désactivation de ce premier menu
	//_treeViewPopupMenu->addMenu (entry);
	QMenu*	entry	= _treeViewPopupMenu;
	_treeViewActions._displayAction				=
			createAction (*entry, "Afficher", QString::fromUtf8("Affiche les entités sélectionnées."),
					SLOT (displayEntitiesCallback ( )), this, false, OFF);
	_treeViewActions._hideAction				=
			createAction (*entry, "Masquer", QString::fromUtf8("Masque les entités sélectionnées."),
					SLOT (hideEntitiesCallback ( )), this, false, OFF);
	_treeViewActions._representationMaskAction	=
			createAction (*entry, "Représentations ...", QString::fromUtf8("Affiche une boite de dialogue de sélection des types de représentations (points, filaire, surfacique) à appliquer."),
			          SLOT (representationTypesCallback ( )), this, false, OFF);

	_entitiesWidget->setContextMenuPolicy (Qt::CustomContextMenu);
	connect (_entitiesWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
	         this, SLOT (displayTreeViewPopupMenu (const QPoint&)));
}	// QtEntitiesPanel::createPopupMenu


void QtEntitiesPanel::selectionChanged (
		vector<Entity*>& selectedEntities, vector<Entity*>& deselectedEntities)
{
	BEGIN_QT_TRY_CATCH_BLOCK
	CHECK_NULL_PTR_ERROR (getSelectionManager ( ))
	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))

	// Eviter des calculs sur une sélection qui va être détruite => on commence
	// par deselectedEntities.
	getSelectionManager ( )->removeFromSelection (deselectedEntities);
	getSelectionManager ( )->addToSelection (selectedEntities);
}	// QtEntitiesPanel::selectionChanged


QtEntityTreeWidgetItem* QtEntitiesPanel::getItem (const Mgx3D::Utils::Entity& entity)
{
	QTreeWidgetItem*			parent		= 0;
	const Geom::GeomEntity*			geomEntity	= dynamic_cast<const Geom::GeomEntity*>(&entity);
	const Topo::TopoEntity*			topoEntity	= dynamic_cast<const Topo::TopoEntity*>(&entity);
	const Mesh::MeshEntity*			meshEntity	= dynamic_cast<const Mesh::MeshEntity*>(&entity);
	const CoordinateSystem::SysCoord*	repEntity	= dynamic_cast<const CoordinateSystem::SysCoord*>(&entity);
	const Structured::StructuredMeshEntity*	structuredMeshEntity= dynamic_cast<const Structured::StructuredMeshEntity*>(&entity);
	if (0 != geomEntity)
	{
		parent	= _cadItem;
		for (size_t i = 0; i < parent->childCount ( ); i++)
		{
			QTreeWidgetItem*	group	= parent->child (i);
			assert (0 != group);
			for (size_t g = 0; g < group->childCount ( ); g++)
			{
				QtCADEntityTreeWidgetItem*	item	=
					dynamic_cast<QtCADEntityTreeWidgetItem*>(group->child (g));	
				if ((0 != item) && (&entity == item->getEntity ( )))
					return item;
			}	// for (size_t g = 0; g < group->childCount ( ); g++)
		}	// for (size_t i = 0; i < parent->childCount ( ); i++)
	}	// if (0 != geomEntity)
	else if (0 != topoEntity)
	{
		parent	= _topologyItem;
		for (size_t i = 0; i < parent->childCount ( ); i++)
		{
			QTreeWidgetItem*	group	= parent->child (i);
			assert (0 != group);
			for (size_t g = 0; g < group->childCount ( ); g++)
			{
				QtTopologicEntityTreeWidgetItem*	item	=
					dynamic_cast<QtTopologicEntityTreeWidgetItem*>(group->child (g));	
				if ((0 != item) && (&entity == item->getEntity ( )))
					return item;
			}	// for (size_t g = 0; g < group->childCount ( ); g++)
		}	// for (size_t i = 0; i < parent->childCount ( ); i++)
	}	// else if (0 != topoEntity)
	else if (0 != meshEntity)
	{
		parent	= _meshingItem;
		for (size_t i = 0; i < parent->childCount ( ); i++)
		{
			QTreeWidgetItem*	group	= parent->child (i);
			assert (0 != group);
			for (size_t g = 0; g < group->childCount ( ); g++)
			{
				QtMeshEntityTreeWidgetItem*	item	=
					dynamic_cast<QtMeshEntityTreeWidgetItem*>(group->child (g));	
				if ((0 != item) && (&entity == item->getEntity ( )))
					return item;
			}	// for (size_t g = 0; g < group->childCount ( ); g++)
		}	// for (size_t i = 0; i < parent->childCount ( ); i++)
	}	// else if (0 != meshEntity)
	else if (0 != repEntity)
	{
		parent	= _coordinateSystemItem;
		for (size_t i = 0; i < parent->childCount ( ); i++)
		{
			QTreeWidgetItem*	group	= parent->child (i);
			assert (0 != group);
			for (size_t g = 0; g < group->childCount ( ); g++)
			{
				QtSysCoordEntityTreeWidgetItem*	item	=
					dynamic_cast<QtSysCoordEntityTreeWidgetItem*>(group->child (g));
				if ((0 != item) && (&entity == item->getEntity ( )))
					return item;
			}	// for (size_t g = 0; g < group->childCount ( ); g++)
		}	// for (size_t i = 0; i < parent->childCount ( ); i++)
	}	// else if (0 != repEntity)
	else if (0 != structuredMeshEntity)
	{
		parent	= _structuredMeshingItem;
		for (size_t i = 0; i < parent->childCount ( ); i++)
		{
			QTreeWidgetItem*	group	= parent->child (i);
			assert (0 != group);
			for (size_t g = 0; g < group->childCount ( ); g++)
			{
				QtStructuredMeshEntityTreeWidgetItem*	item	=
					dynamic_cast<QtStructuredMeshEntityTreeWidgetItem*>(group->child (g));	
				if ((0 != item) && (&entity == item->getEntity ( )))
					return item;
			}	// for (size_t g = 0; g < group->childCount ( ); g++)
		}	// for (size_t i = 0; i < parent->childCount ( ); i++)
	}	// else if (0 != structuredMeshEntity)

	return 0;
}	// QtEntitiesPanel::getItem


QTreeWidgetItem* QtEntitiesPanel::createEntryItem (
			QTreeWidgetItem& parent, const string& title, unsigned long mask,
			DisplayRepresentation::display_type entitiesType)
{
	QtEntitiesGroupTreeWidgetItem*	item	=
		new QtEntitiesGroupTreeWidgetItem (&parent, title, this, entitiesType);
	item->setRepresentationMask (mask);
	parent.addChild (item);

	return item;
}	// QtEntitiesPanel::createEntryItem


QtCADEntityTreeWidgetItem* QtEntitiesPanel::createCADItem (Geom::GeomEntity& entity)
{
	QTreeWidgetItem*		parent	= getCADEntryItem (entity.getDim ( ));
	CHECK_NULL_PTR_ERROR (parent)
	QtEntitiesGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtEntitiesGroupTreeWidgetItem*>(parent);
	unsigned long	rep3d	= 0 == groupItem ? GraphicalEntityRepresentation::CURVES | GraphicalEntityRepresentation::ISOCURVES : groupItem->getRepresentationMask ( );
	QtCADEntityTreeWidgetItem*	item	= new QtCADEntityTreeWidgetItem (parent, entity, rep3d);
	item->setCheckState (0, Qt::Unchecked);
	parent->addChild (item);
	// setHidden (false) : corrige un bogue où de temps en temps les items ne
	// sont pas visibles ???
	item->setHidden (false);

	return item;
}	// QtEntitiesPanel::createCADItem


QTreeWidgetItem* QtEntitiesPanel::getCADEntryItem (unsigned char dim) const
{
	CHECK_NULL_PTR_ERROR (_cadCloudsItem)
	CHECK_NULL_PTR_ERROR (_cadCurvesItem)
	CHECK_NULL_PTR_ERROR (_cadSurfacesItem)
	CHECK_NULL_PTR_ERROR (_cadVolumesItem)
	switch (dim)
	{
		case	0	: return _cadCloudsItem;
		case	1	: return _cadCurvesItem;
		case	2	: return _cadSurfacesItem;
		case	3	: return _cadVolumesItem;
	}	// switch (dim)

	UTF8String	error (Charset::UTF_8);
	error << "Erreur : dimension invalide (" << (unsigned long)dim << ").";
	INTERNAL_ERROR (exc, error, "QtEntitiesPanel::getCADEntryItem")
	throw exc;
}	// QtEntitiesPanel::getCADEntryItem


QtTopologicEntityTreeWidgetItem* QtEntitiesPanel::createTopologicItem (Topo::TopoEntity& entity)
{
	QTreeWidgetItem*			parent	= getTopologicEntryItem (entity.getDim ( ));
	CHECK_NULL_PTR_ERROR (parent)
	QtEntitiesGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtEntitiesGroupTreeWidgetItem*>(parent);
	unsigned long	rep3d	= 0 == groupItem ? GraphicalEntityRepresentation::CURVES | GraphicalEntityRepresentation::ISOCURVES : groupItem->getRepresentationMask ( );
	QtTopologicEntityTreeWidgetItem*	item	= new QtTopologicEntityTreeWidgetItem (parent, entity, rep3d);
	item->setCheckState (0, Qt::Unchecked);
	parent->addChild (item);
	// setHidden (false) : corrige un bogue où de temps en temps les items ne
	// sont pas visibles ??? (mais bien existant : cocher le parent provoque
	// la visibilité des enfants invisibles).
	item->setHidden (false);

	return item;
}	// QtEntitiesPanel::createTopologicItem


QTreeWidgetItem* QtEntitiesPanel::getTopologicEntryItem (unsigned char dim) const
{
	CHECK_NULL_PTR_ERROR (_topoVerticesItem)
	CHECK_NULL_PTR_ERROR (_topoEdgesItem)
	CHECK_NULL_PTR_ERROR (_topoFacesItem)
	CHECK_NULL_PTR_ERROR (_topoBlocksItem)
	switch (dim)
	{
		case	0	: return _topoVerticesItem;
		case	1	: return _topoEdgesItem;
		case	2	: return _topoFacesItem;
		case	3	: return _topoBlocksItem;
	}	// switch (dim)

	UTF8String	error (Charset::UTF_8);
	error << "Erreur : dimension invalide (" << (unsigned long)dim << ").";
	INTERNAL_ERROR (exc, error, "QtEntitiesPanel::getTopologicEntryItem")
	throw exc;
}	// QtEntitiesPanel::getTopologicEntryItem


QtMeshEntityTreeWidgetItem* QtEntitiesPanel::createMeshItem (Mesh::MeshEntity& entity)
{
	QTreeWidgetItem*			parent	= getMeshEntryItem (entity.getDim ( ));
	CHECK_NULL_PTR_ERROR (parent)
	QtEntitiesGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtEntitiesGroupTreeWidgetItem*>(parent);
	unsigned long	rep3d	= 0 == groupItem ? GraphicalEntityRepresentation::CURVES : groupItem->getRepresentationMask ( );
	QtMeshEntityTreeWidgetItem*	item	= new QtMeshEntityTreeWidgetItem (parent, entity, rep3d);
	item->setCheckState (0, Qt::Unchecked);
	parent->addChild (item);
	// setHidden (false) : corrige un bogue où de temps en temps les items ne
	// sont pas visibles ???
	item->setHidden (false);

	return item;
}	// QtEntitiesPanel::createMeshItem


QTreeWidgetItem* QtEntitiesPanel::getMeshEntryItem (unsigned char dim) const
{
	CHECK_NULL_PTR_ERROR (_meshCloudsItem)
	CHECK_NULL_PTR_ERROR (_meshLinesItem)
	CHECK_NULL_PTR_ERROR (_meshSurfacesItem)
	CHECK_NULL_PTR_ERROR (_meshVolumesItem)
	switch (dim)
	{
		case	0	: return _meshCloudsItem;
		case	1	: return _meshLinesItem;
		case	2	: return _meshSurfacesItem;
		case	3	: return _meshVolumesItem;
	}	// switch (dim)

	UTF8String	error (Charset::UTF_8);
	error << "Erreur : dimension invalide (" << (unsigned long)dim << ").";
	INTERNAL_ERROR (exc, error, "QtEntitiesPanel::getMeshEntryItem")
	throw exc;
}	// QtEntitiesPanel::getMeshEntryItem

QtSysCoordEntityTreeWidgetItem* QtEntitiesPanel::createSysCoordItem (Mgx3D::CoordinateSystem::SysCoord& entity)
{
	QTreeWidgetItem*		parent	= getSysCoordEntryItem ();
	CHECK_NULL_PTR_ERROR (parent)
	QtEntitiesGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtEntitiesGroupTreeWidgetItem*>(parent);
	unsigned long	rep3d	= 0 == groupItem ? GraphicalEntityRepresentation::TRIHEDRON : groupItem->getRepresentationMask ( );
	QtSysCoordEntityTreeWidgetItem*	item	= new QtSysCoordEntityTreeWidgetItem (parent, entity, rep3d);
	item->setCheckState (0, Qt::Unchecked);
	parent->addChild (item);
	// setHidden (false) : corrige un bogue où de temps en temps les items ne
	// sont pas visibles ???
	item->setHidden (false);

	return item;
}	// QtEntitiesPanel::createSysCoordItem


QTreeWidgetItem* QtEntitiesPanel::getSysCoordEntryItem ( ) const
{
	CHECK_NULL_PTR_ERROR (_sysCoordItem);
	return _sysCoordItem;
}	// QtEntitiesPanel::getSysCoordEntryItem


QtStructuredMeshEntityTreeWidgetItem* QtEntitiesPanel::createStructuredMeshItem (Mgx3D::Structured::StructuredMeshEntity& entity)
{
	QTreeWidgetItem*			parent		= getStructuredMeshEntryItem (entity.getDim ( ));
	CHECK_NULL_PTR_ERROR (parent)
	QtEntitiesGroupTreeWidgetItem*		groupItem	= dynamic_cast<QtEntitiesGroupTreeWidgetItem*>(parent);
	const unsigned long			rep3d		= 0 == groupItem ? GraphicalEntityRepresentation::VOLUMES | GraphicalEntityRepresentation::CELLS_VALUES : groupItem->getRepresentationMask ( );
	QtStructuredMeshEntityTreeWidgetItem*	item		= new QtStructuredMeshEntityTreeWidgetItem (parent, entity, rep3d);
	item->setCheckState (0, Qt::Unchecked);
	parent->addChild (item);
	// setHidden (false) : corrige un bogue où de temps en temps les items ne
	// sont pas visibles ???
	item->setHidden (false);

	return item;
}	// QtEntitiesPanel::createStructuredMeshItem


QTreeWidgetItem* QtEntitiesPanel::getStructuredMeshEntryItem (unsigned char dim) const
{
	CHECK_NULL_PTR_ERROR (_structuredMeshVolumesItem)
	return _structuredMeshVolumesItem;
}	// QtEntitiesPanel::getStructuredMeshEntryItem


void QtEntitiesPanel::updateEntryItem (QTreeWidgetItem& entryItem)
{
	const int	childCount		= entryItem.childCount ( );
	THREESTATES	displayState	= 0 == childCount ? OFF : UNDETERMINED;
	if (0 != displayState)
	{
		QTreeWidgetItem*	child	= entryItem.child (0);
		CHECK_NULL_PTR_ERROR (child)
		switch (child->checkState (0))
		{
			case Qt::Checked	: displayState	= ON;	break;
			case Qt::Unchecked	: displayState	= OFF;	break;
			default				: displayState	= UNDETERMINED;
		}	// switch (child->checkState (0))
	}	// if (0 != displayState)

	for (int i = 1; i < childCount; i++)
	{
		QTreeWidgetItem*	child	= entryItem.child (i);
		CHECK_NULL_PTR_ERROR (child)
		displayState	= newState (
			displayState, Qt::Checked == child->checkState (0) ? true : false);
		if (UNDETERMINED == displayState)
			break;
	}	// for (int i = 1; i < childCount; i++)

	switch (displayState)
	{
		case ON		: entryItem.setCheckState (0, Qt::Checked);	break;
		case OFF	: entryItem.setCheckState (0, Qt::Unchecked);	break;
		default		: entryItem.setCheckState (0, Qt::PartiallyChecked);
	}	// switch (displayState)
}	// QtEntitiesPanel::updateEntryItem


FilterEntity::objectType QtEntitiesPanel::getFilteredType (QTreeWidgetItem* item) const
{
	if (0 == item)
		return FilterEntity::NoneEntity;

	if (item == _cadItem)
		return (FilterEntity::objectType)(
		       FilterEntity::GeomVolume | FilterEntity::GeomSurface |
		       FilterEntity::GeomCurve  | FilterEntity::GeomVertex);
	if (item == _topologyItem){

		FilterEntity::objectType type = (FilterEntity::objectType)(
				FilterEntity::TopoBlock  | FilterEntity::TopoCoFace |
				FilterEntity::TopoCoEdge | FilterEntity::TopoVertex);
		if (Internal::InternalPreferences::instance ( )._displayEdge.getValue ( ))
			type = (FilterEntity::objectType)(type | FilterEntity::TopoEdge);
		if (Internal::InternalPreferences::instance ( )._displayFace.getValue ( ))
			type = (FilterEntity::objectType)(type | FilterEntity::TopoFace);

		return type;
	}
	if (item == _meshingItem)
		return (FilterEntity::objectType)(
		       FilterEntity::MeshVolume | FilterEntity::MeshSubVolume |
		       FilterEntity::MeshSurface| FilterEntity::MeshSubSurface|
			   FilterEntity::MeshLine| FilterEntity::MeshCloud);
	if (item == _cadVolumesItem)
		return FilterEntity::GeomVolume;
	if (item == _cadSurfacesItem)
		return FilterEntity::GeomSurface;
	if (item == _cadCurvesItem)
		return FilterEntity::GeomCurve;
	if (item == _cadCloudsItem)
		return FilterEntity::GeomVertex;
	if (item == _topoBlocksItem)
		return FilterEntity::TopoBlock;
	if (item == _topoFacesItem)
		return FilterEntity::TopoCoFace;
	if (item == _topoEdgesItem)
		return FilterEntity::TopoCoEdge;
	if (item == _topoVerticesItem)
		return FilterEntity::TopoVertex;
	if (item == _meshVolumesItem)
		return (FilterEntity::objectType)(FilterEntity::MeshVolume | FilterEntity::MeshSubVolume);
	if (item == _meshSurfacesItem)
		return (FilterEntity::objectType)(FilterEntity::MeshSurface | FilterEntity::MeshSubSurface);
	if (item == _meshLinesItem)
		return FilterEntity::MeshLine;
	if (item == _meshCloudsItem)
		return FilterEntity::MeshCloud;

	if (item == _coordinateSystemItem)
		return FilterEntity::SysCoord;
	if (item == _sysCoordItem)
		return FilterEntity::SysCoord;

	if (item == _structuredMeshingItem)
		return FilterEntity::StructuredMesh;
	if (item == _structuredMeshVolumesItem)
		return FilterEntity::StructuredMesh;

	return FilterEntity::NoneEntity;
}	// QtEntitiesPanel::getFilteredType


bool QtEntitiesPanel::itemCreationTriggersDisplayCallback ( ) const
{
	return true;
}	// QtEntitiesPanel::itemCreationTriggersDisplayCallback


bool QtEntitiesPanel::groupEntitiesDisplayModifications ( ) const
{
	return false;
}	// QtEntitiesPanel::groupEntitiesDisplayModifications


void QtEntitiesPanel::stateChangeCallback (QTreeWidgetItem* item, int col)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	QtEntitiesGroupTreeWidgetItem*	groupItem	= dynamic_cast<QtEntitiesGroupTreeWidgetItem*>(item);
	if (0 != groupItem)
	{
		groupItem->stateChange (col);
		return;
	}	// if (0 != groupItem)

	itemStateChange (item, true);
 
	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtEntitiesPanel::stateChangeCallback


void QtEntitiesPanel::itemStateChange (QTreeWidgetItem*item , bool updateParent)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (item)
	CHECK_NULL_PTR_ERROR (getGraphicalWidget ( ))
	QtEntityTreeWidgetItem*	entityItem	= dynamic_cast<QtEntityTreeWidgetItem*>(item);
	if (0 == entityItem)
		return;

	const bool	checked	= Qt::Checked == item->checkState (0) ? true : false;
	getGraphicalWidget ( )->getRenderingManager ( ).displayRepresentation (
							*(entityItem->getEntity ( )), checked,
							entityItem->getRepresentationMask ( ));

	if (true == updateParent)
	{
		// Les enfants ont ils tous le même caractère affiché/masqué ?
		QTreeWidgetItem*	parent	= item->parent ( );
		CHECK_NULL_PTR_ERROR (parent)
		const Qt::CheckState	state	= item->checkState (0);
		parent->setCheckState (0, state);
		for (int i = 0; i < parent->childCount ( ); i++)
		{
			if (parent->child (i)->checkState (0) != state)
			{
				parent->setCheckState (0, Qt::PartiallyChecked);
				break;
			}	// if (parent->child (i)->checkState (0) != state)
		}	// for (int i = 0; i < parent->childCount ( ); i++)
	}	// if (true == updateParent)
 
	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtEntitiesPanel::itemStateChange


void QtEntitiesPanel::itemsStateChange (const vector<QTreeWidgetItem*>& list , bool updateParent)
{
	if (0 == list.size ( ))
		return;

	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (getGraphicalWidget ( ))
	QTreeWidgetItem*	parent	= list [0]->parent ( );

	unique_ptr<RenderingManager::DisplayLocker>	displayLocker (
		new RenderingManager::DisplayLocker (
							getGraphicalWidget ( )->getRenderingManager ( )));
	size_t	i	= 0;
	for (vector<QTreeWidgetItem*>::const_iterator it = list.begin ( );
	     list.end ( ) != it; it++, i++)
	{

		CHECK_NULL_PTR_ERROR (*it)
		QtEntityTreeWidgetItem*	entityItem	= dynamic_cast<QtEntityTreeWidgetItem*>(*it);
		if (0 == entityItem)
			continue;

		const bool	checked	= Qt::Checked == (*it)->checkState (0) ? true:false;
		getGraphicalWidget ( )->getRenderingManager ( ).displayRepresentation (
							*(entityItem->getEntity ( )), checked,
							entityItem->getRepresentationMask ( ));
		if (0 == i % Resources::instance ( )._updateRefreshRate.getValue ( ))
		{	// On fait un render tous les x entités.

			// On veut unlock avant lock => en 2 temps (reset (0) puis
			// reset (new DisplayLocker (...)).
			// Si directement reset (new DisplayLocker (...))  on a
			// new DisplayLocker (...)
			// puis delete de l'ancien DisplayLocker => unlock en dernier ...
			displayLocker.reset (0);
			displayLocker.reset (new RenderingManager::DisplayLocker (
							getGraphicalWidget ( )->getRenderingManager ( )));
			// Eventuelle actualisation IHM :
			if (true == QApplication::hasPendingEvents ( ))
//				QApplication::processEvents (QEventLoop::AllEvents, 5000);
				QtMgx3DApplication::processEvents (5000);
		}
	}	// for (vector<QTreeWidgetItem*>::const_iterator it = list.begin ( );

	if ((true == updateParent) && (0 != parent))
	{
		// Les enfants ont ils tous le même caractère affiché/masqué ?
		const Qt::CheckState	state	= list [0]->checkState (0);
		parent->setCheckState (0, state);
		for (int i = 0; i < parent->childCount ( ); i++)
		{
			if (parent->child (i)->checkState (0) != state)
			{
				parent->setCheckState (0, Qt::PartiallyChecked);
				break;
			}	// if (parent->child (i)->checkState (0) != state)
		}	// for (int i = 0; i < parent->childCount ( ); i++)
	}	// if (true == updateParent)
 
	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtEntitiesPanel::itemsStateChange


void QtEntitiesPanel::displayEntitiesCallback ( )
{
	showSelectedEntities (true);
}	// QtEntitiesPanel::displayEntitiesCallback


void QtEntitiesPanel::hideEntitiesCallback ( )
{
	showSelectedEntities (false);
}	// QtEntitiesPanel::hideEntitiesCallback


void QtEntitiesPanel::representationTypesCallback ( )
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker (
							getGraphicalWidget ( )->getRenderingManager ( ));

	// On s'intéresse aux types sélectionnés :
	FilterEntity::objectType	types	= getSelectedEntitiesTypes ( );
//	vector<Entity*>				entities;
//	getContext ( ).getGroupManager ( ).getShownEntities (types, entities);
	CHECK_NULL_PTR_ERROR (getSelectionManager ( ))
	vector<Entity*>	entities	= getSelectionManager ( )->getEntities ( );
	changeRepresentationTypes (entities);

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtEntitiesPanel::representationTypesCallback


void QtEntitiesPanel::displayTreeViewPopupMenu (const QPoint& point)
{
	if ((0 != _treeViewPopupMenu) && (0 != _entitiesWidget))
		_treeViewPopupMenu->popup (_entitiesWidget->mapToGlobal (point));
}	// QtEntitiesPanel::displayTreeViewPopupMenu


void QtEntitiesPanel::showSelectedEntities (bool show)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	// Verrouiller les opérations d'affichage (optimisation) :
	RenderingManager::DisplayLocker	displayLocker (
							getGraphicalWidget ( )->getRenderingManager ( ));

	// Ce traitement concerne des items représentant directement des entités,
	// mais également des items représentant des groupes.
	// Dans un premier temps on remplace tous ces items "groupes" par les items
	// qu'ils représentent.
	QtObjectSignalBlocker	itemBlocker (_entitiesWidget);
	QList<QTreeWidgetItem*>	selectedItems	=
										_entitiesWidget->selectedItems ( );
	vector<Entity*>	entities;
	for (size_t i = 0; i < selectedItems.count ( ); i++)
	{
		QTreeWidgetItem*		item	= selectedItems [i];
		if (0 == item)
			continue;
		QtEntityTreeWidgetItem*	entityItem	=
									dynamic_cast<QtEntityTreeWidgetItem*>(item);
		if (0 != entityItem)
			entities.push_back (entityItem->getEntity ( ));
		else
		{
			item->setCheckState (0, true == show ? Qt::Checked : Qt::Unchecked);
			for (size_t c = 0; c < item->childCount ( ); c++)
			{	
				entityItem	=
						dynamic_cast<QtEntityTreeWidgetItem*>(item->child (c));
				if (0 != entityItem)
				{
					entityItem->setCheckState (
								0, true == show ? Qt::Checked : Qt::Unchecked);
					entities.push_back (entityItem->getEntity ( ));
				}	// if (0 != entityItem)
			}	// for (size_t c = 0; c < item->childCount ( ); c++)
		}	// else if (0 != entityItem)
	}	// for (size_t i = 0; i < selectedItems.count ( ); i++)

	getGraphicalWidget ( )->getRenderingManager (
								).displayRepresentations (
			entities, show, 0,
			(DisplayRepresentation::display_type)(
			DisplayRepresentation::DISPLAY_GEOM |
			DisplayRepresentation::DISPLAY_TOPO |
			DisplayRepresentation::DISPLAY_MESH));

	COMPLETE_QT_TRY_CATCH_BLOCK (QtMgx3DApplication::displayUpdatesErrors ( ), this, getAppTitle ( ))
}	// QtEntitiesPanel::showSelectedEntities


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
