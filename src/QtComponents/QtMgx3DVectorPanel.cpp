/**
 * \file		QtMgx3DVectorPanel.cpp
 * \author		Charles PIGNEROL
 * \date		25/11/2013
 */

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DVectorPanel.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "Utils/Common.h"
#include "Utils/MgxNumeric.h"
#include "Internal/Resources.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Topo/TopoManager.h"
#include "Topo/CoEdge.h"

#include <QtUtil/QtConfiguration.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>

#include <QLayout>
#include <QLabel>
#include <QValidator>

#include <iostream>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Utils::Math;
using namespace Mgx3D::Topo;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace QtComponents
{

QtMgx3DVectorPanel::QtMgx3DVectorPanel (QWidget* parent, const string& title,
					bool editable, const string& xTitle,
					const string& yTitle, const string& zTitle,
					double dx, double dxMin, double dxMax,
					double dy, double dyMin, double dyMax,
					double dz, double dzMin, double dzMax,
					QtMgx3DMainWindow* mainWindow,
					FilterEntity::objectType types,
					bool verticalLayout)
	: QWidget (parent), 
	  _initialDx (dx), _initialDy (dy), _initialDz (dz),
	  _dxTextField (0), _dyTextField (0), _dzTextField (0),
	  _segmentIDTextField (0), _point1IDTextField (0), _point2IDTextField (0),
	  _title (title)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);

	QLabel*	label	= 0;
	if (false == title.empty ( ))
	{
		label	= new QLabel (title.c_str ( ), this);
		layout->addWidget (label);
	}	// if (false == title.empty ( ))

	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	layout->addLayout (hlayout);
	label	= new QLabel (xTitle.c_str ( ), this);
	label->setMinimumSize (label->sizeHint ( ));
	hlayout->addWidget (label);
	_dxTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	hlayout->addWidget (_dxTextField);
	if (true == verticalLayout)
	{
		hlayout->addStretch (2.);
		hlayout	= new QHBoxLayout (0);
		hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
		hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
		layout->addLayout (hlayout);
	}	// if (true == verticalLayout)
	label					= new QLabel (yTitle.c_str ( ), this);
	label->setMinimumSize (label->sizeHint ( ));
	hlayout->addWidget (label);
	_dyTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	hlayout->addWidget (_dyTextField);
	if (true == verticalLayout)
	{
		hlayout->addStretch (2.);
		hlayout	= new QHBoxLayout (0);
		hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
		hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
		layout->addLayout (hlayout);
	}	// if (true == verticalLayout)
	label					= new QLabel (zTitle.c_str ( ), this);
	label->setMinimumSize (label->sizeHint ( ));
	hlayout->addWidget (label);
	_dzTextField	= &QtNumericFieldsFactory::createDistanceTextField (this);
	hlayout->addWidget (_dzTextField);
		hlayout->addStretch (2.);
	connect (_dxTextField,  SIGNAL (textEdited (const QString&)), this,
	         SLOT (coordinatesEditedCallback ( )));
	connect (_dyTextField,  SIGNAL (textEdited (const QString&)), this,
	         SLOT (coordinatesEditedCallback ( )));
	connect (_dzTextField,  SIGNAL (textEdited (const QString&)), this,
	         SLOT (coordinatesEditedCallback ( )));

	if (false == editable)
	{
		_dxTextField->setReadOnly (true);
		_dyTextField->setReadOnly (true);
		_dzTextField->setReadOnly (true);
	}	// if (false == editable)
	else
	{
		_dxTextField->setRange (dxMin, dxMax);
		_dyTextField->setRange (dyMin, dyMax);
		_dzTextField->setRange (dzMin, dzMax);
	}	// else if (false == editable)
	_dxTextField->setValue (dx);
	_dyTextField->setValue (dy);
	_dzTextField->setValue (dz);

	// L'ID de l'éventuel segment dont on récupère les coordonnées :
	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	layout->addLayout (hlayout);
	label	= new QLabel ("Segment :", this);
	label->setToolTip (QString::fromUtf8("Segment/Arête représentant le vecteur."));
	label->setMinimumSize (label->sizeHint ( ));
	hlayout->addWidget (label);
	_segmentIDTextField	= new QtEntityIDTextField (
							this, mainWindow, SelectionManagerIfc::D1, types);
	_segmentIDTextField->setToolTip (QString::fromUtf8("Segment/Arête représentant le vecteur."));
	hlayout->addWidget (_segmentIDTextField);
	connect (_segmentIDTextField, SIGNAL (selectionModified (QString)), this,
	         SLOT (selectedSegmentCallback(QString)));
	if (0 == (types & FilterEntity::AllEdges))
	{
		label->setVisible (false);
		_segmentIDTextField->setVisible (false);
	}	// if (0 == (types & FilterEntity::AllEdges))

	// Les IDs des éventuels points définissant le vecteur :
	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins  (
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ),
						Resources::instance ( )._margin.getValue ( ));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));
	layout->addLayout (hlayout);
	QLabel*	label1	= new QLabel ("Point 1 :", this);
	label1->setToolTip (QString::fromUtf8("Origine du vecteur."));
	label1->setMinimumSize (label1->sizeHint ( ));
	hlayout->addWidget (label1);
	_point1IDTextField	= new QtEntityIDTextField (
							this, mainWindow, SelectionManagerIfc::D0, types);
	_point1IDTextField->setToolTip (QString::fromUtf8("Origine du vecteur."));
	hlayout->addWidget (_point1IDTextField);
	connect (_point1IDTextField, SIGNAL (selectionModified (QString)), this,
	         SLOT (selectedPointCallback(QString)));
	QLabel*	label2	= new QLabel ("Point 2 :", this);
	label2->setToolTip (QString::fromUtf8("Terminaison du vecteur."));
	label2->setMinimumSize (label2->sizeHint ( ));
	hlayout->addWidget (label2);
	_point2IDTextField	= new QtEntityIDTextField (
							this, mainWindow, SelectionManagerIfc::D0, types);
	_point2IDTextField->setToolTip (QString::fromUtf8("Terminaison du vecteur."));
	hlayout->addWidget (_point2IDTextField);
	connect (_point2IDTextField, SIGNAL (selectionModified (QString)), this,
	         SLOT (selectedPointCallback(QString)));
	if (0 == (types & FilterEntity::AllPoints))
	{
		CHECK_NULL_PTR_ERROR (label1)
		CHECK_NULL_PTR_ERROR (label2)
		label1->setVisible (false);
		_point1IDTextField->setVisible (false);
		label2->setVisible (false);
		_point2IDTextField->setVisible (false);
	}	// if (0 == (types & FilterEntity::AllPoints))
}	// QtMgx3DVectorPanel::QtMgx3DVectorPanel


QtMgx3DVectorPanel::QtMgx3DVectorPanel (const QtMgx3DVectorPanel&)
	: QWidget (0),
	  _initialDx (0.), _initialDy (0.), _initialDz (0.), 
	  _dxTextField (0), _dyTextField (0), _dzTextField (0),
	  _segmentIDTextField (0), _point1IDTextField (0), _point2IDTextField (0),
	  _title ("Invalid QtMgx3DVectorPanel")
{
	MGX_FORBIDDEN ("QtMgx3DVectorPanel copy constructor is forbidden.")
}	// QtMgx3DVectorPanel::QtMgx3DVectorPanel (const QtMgx3DVectorPanel&)


QtMgx3DVectorPanel& QtMgx3DVectorPanel::operator = (const QtMgx3DVectorPanel&)
{
	MGX_FORBIDDEN ("QtMgx3DVectorPanel assignment operator is forbidden.")
	return *this;
}	// QtMgx3DVectorPanel::operator =


QtMgx3DVectorPanel::~QtMgx3DVectorPanel ( )
{
}	// QtMgx3DVectorPanel::~QtMgx3DVectorPanel


void QtMgx3DVectorPanel::blockFieldsSignals (bool block)
{
	CHECK_NULL_PTR_ERROR (_dxTextField)
	CHECK_NULL_PTR_ERROR (_dyTextField)
	CHECK_NULL_PTR_ERROR (_dzTextField)
	CHECK_NULL_PTR_ERROR (_segmentIDTextField)
	CHECK_NULL_PTR_ERROR (_point1IDTextField)
	CHECK_NULL_PTR_ERROR (_point2IDTextField)
	_dxTextField->blockSignals (block);
	_dyTextField->blockSignals (block);
	_dzTextField->blockSignals (block);
	_segmentIDTextField->blockSignals (block);
	_point1IDTextField->blockSignals (block);
	_point2IDTextField->blockSignals (block);
}	// QtMgx3DVectorPanel::blockFieldsSignals


bool QtMgx3DVectorPanel::isModified ( ) const
{
	double	dx,dy, dz;

	try
	{
		dx	= getDx ( );
		dy	= getDy ( );
		dz	= getDz ( );
	}
	catch (...)
	{
		return true;
	}

	if ((dx != _initialDx) || (dy != _initialDy) || (dz != _initialDz))
		return true;

	return false;
}	// QtMgx3DVectorPanel::isModified


void QtMgx3DVectorPanel::setDx (double dx)
{
	CHECK_NULL_PTR_ERROR (_dxTextField)
	_dxTextField->setText (QString::number (dx));
}	// QtMgx3DVectorPanel::setDx


double QtMgx3DVectorPanel::getDx ( ) const
{
	CHECK_NULL_PTR_ERROR (_dxTextField)
	bool	ok		= true;
	double	value	= _dxTextField->text ( ).toDouble (&ok);

	if (false == ok)
	{
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg << _title
		         << " : valeur invalide de la première composante ("
		         << _dxTextField->text ( ).toStdString ( ) << ").";

		throw Exception (errorMsg);
	}	// if (false == ok)

	return value;
}	// QtMgx3DVectorPanel::getDx


void QtMgx3DVectorPanel::setDy (double dy)
{
	CHECK_NULL_PTR_ERROR (_dyTextField)
	_dyTextField->setText (QString::number (dy));
}	// QtMgx3DVectorPanel::setDy


double QtMgx3DVectorPanel::getDy ( ) const
{
	CHECK_NULL_PTR_ERROR (_dyTextField)
	bool	ok		= true;
	double	value	= _dyTextField->text ( ).toDouble (&ok);

	if (false == ok)
	{
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg << _title
		         << " : valeur invalide de la seconde composante ("
		         << _dyTextField->text ( ).toStdString ( ) << ").";

		throw Exception (errorMsg);
	}	// if (false == ok)

	return value;
}	// QtMgx3DVectorPanel::getDy


void QtMgx3DVectorPanel::setDz (double dz)
{
	CHECK_NULL_PTR_ERROR (_dzTextField)
	_dzTextField->setText (QString::number (dz));
}	// QtMgx3DVectorPanel::setDz


double QtMgx3DVectorPanel::getDz ( ) const
{
	CHECK_NULL_PTR_ERROR (_dzTextField)
	bool	ok		= true;
	double	value	= _dzTextField->text ( ).toDouble (&ok);

	if (false == ok)
	{
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg << _title
		         << " : valeur invalide de la troisième composante ("
		         << _dzTextField->text ( ).toStdString ( ) << ").";

		throw Exception (errorMsg);
	}	// if (false == ok)

	return value;
}	// QtMgx3DVectorPanel::getDz


Math::Vector QtMgx3DVectorPanel::getVector ( ) const
{
	return Vector (getDx ( ), getDy ( ), getDz ( ));
}	// QtMgx3DVectorPanel::getVector


void QtMgx3DVectorPanel::setSegment (const string& name)
{
	CHECK_NULL_PTR_ERROR (_segmentIDTextField)
	vector<string>	names;
	if (0 != name.length ( ))
		names.push_back (name);
	try
	{
		if (!names.empty() || !_segmentIDTextField->getUniqueNames().empty()){
			_segmentIDTextField->setUniqueNames (names);
			selectedSegmentCallback (name.c_str ( ));
		}
	}
	catch (...)
	{
	}
}	// QtMgx3DVectorPanel::setSegment


void QtMgx3DVectorPanel::reset ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_segmentIDTextField)
		_segmentIDTextField->reset ( );
	}
	catch (...)
	{
	}
	try
	{
		CHECK_NULL_PTR_ERROR (_point1IDTextField)
		_point1IDTextField->reset ( );
	}
	catch (...)
	{
	}
	try
	{
		CHECK_NULL_PTR_ERROR (_point2IDTextField)
		_point2IDTextField->reset ( );
	}
	catch (...)
	{
	}
	try
	{
		CHECK_NULL_PTR_ERROR (_dxTextField)
		CHECK_NULL_PTR_ERROR (_dyTextField)
		CHECK_NULL_PTR_ERROR (_dzTextField)
		_dxTextField->setValue (0.);
		_dyTextField->setValue (0.);
		_dzTextField->setValue (0.);
	}
	catch (...)
	{
	}
}	// QtMgx3DVectorPanel::reset


void QtMgx3DVectorPanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_segmentIDTextField)
	CHECK_NULL_PTR_ERROR (_point1IDTextField)
	CHECK_NULL_PTR_ERROR (_point2IDTextField)
	_segmentIDTextField->setInteractiveMode (false);
	_point1IDTextField->setInteractiveMode (false);
	_point2IDTextField->setInteractiveMode (false);
}	// QtMgx3DVectorPanel::stopSelection


bool QtMgx3DVectorPanel::actualizeGui (bool removeDestroyed)
{
	CHECK_NULL_PTR_ERROR (_segmentIDTextField)
	CHECK_NULL_PTR_ERROR (_point1IDTextField)
	CHECK_NULL_PTR_ERROR (_point2IDTextField)
	bool	modified	= _segmentIDTextField->actualizeGui (removeDestroyed);
	if (true == _point1IDTextField->actualizeGui (removeDestroyed))
		modified	= true;
	if (true == _point2IDTextField->actualizeGui (removeDestroyed))
		modified	= true;

	return modified;
}	// QtMgx3DVectorPanel::actualizeGui


Math::Point QtMgx3DVectorPanel::getPoint (const string& name) const
{
	CHECK_NULL_PTR_ERROR (_point1IDTextField)
	try
	{
		const Geom::Vertex*	vertex	=
			_point1IDTextField->getMainWindow ( ).getContext ( 
									).getGeomManager ( ).getVertex (name, true);
		CHECK_NULL_PTR_ERROR (vertex)
		return vertex->getCoord ( );
	}
	catch (...)
	{
	}
	try
	{
		const Topo::Vertex*	vertex	=
			_point1IDTextField->getMainWindow ( ).getContext ( 
									).getTopoManager ( ).getVertex (name, true);
		CHECK_NULL_PTR_ERROR (vertex)
		return vertex->getCoord ( );
	}
	catch (...)
	{
	}

	UTF8String	error (Charset::UTF_8);
	error << "Il n'y a pas de point nommé " << name << ".";
	throw Exception (error);
}	// QtMgx3DVectorPanel::getPoint


void QtMgx3DVectorPanel::selectedSegmentCallback (const QString name)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_segmentIDTextField)
	CHECK_NULL_PTR_ERROR (_point1IDTextField)
	CHECK_NULL_PTR_ERROR (_point2IDTextField)

	_point1IDTextField->blockSignals (true);
	_point1IDTextField->clear ( );
	_point1IDTextField->blockSignals (false);
	_point2IDTextField->blockSignals (true);
	_point2IDTextField->clear ( );
	_point2IDTextField->blockSignals (false);

	double	dx	= 0., dy	= 0., dz	= 0.;

	if (0 != _segmentIDTextField->getObjectsCount ( ))
	{
		switch (_segmentIDTextField->getObjectType ( ))
		{
			case Entity::GeomCurve	:
			{
				const Geom::Curve*	segment	=
					_segmentIDTextField->getMainWindow ( ).getContext (
						).getGeomManager ( ).getCurve (
													name.toStdString ( ), true);
				CHECK_NULL_PTR_ERROR (segment);
				vector<Geom::Vertex*>	vertices;
				segment->get (vertices);
				size_t	count	= vertices.size ( );
				if (1 > count)
					throw Exception (UTF8String ("Courbe géométrique dotée au plus d'un sommet.", Charset::UTF_8));
				dx	= vertices [count- 1]->getX ( ) - vertices [0]->getX ( );
				dy	= vertices [count- 1]->getY ( ) - vertices [0]->getY ( );
				dz	= vertices [count- 1]->getZ ( ) - vertices [0]->getZ ( );
			}	// case Entity::GeomCurve
			break;
			case Entity::TopoCoEdge	:
			{
				const Topo::CoEdge*	segment	=
					_segmentIDTextField->getMainWindow ( ).getContext (
						).getTopoManager ( ).getCoEdge (
													name.toStdString ( ), true);
				CHECK_NULL_PTR_ERROR (segment);
				const vector<Topo::Vertex*>&	vertices = segment->getVertices ( );
				size_t	count	= vertices.size ( );
				if (1 > count)
					throw Exception (UTF8String ("Arête topologique dotée au plus d'un sommet.", Charset::UTF_8));
				dx	= vertices [count- 1]->getX ( ) - vertices [0]->getX ( );
				dy	= vertices [count- 1]->getY ( ) - vertices [0]->getY ( );
				dz	= vertices [count- 1]->getZ ( ) - vertices [0]->getZ ( );
			}	// case Entity::TopoVertex
			break;
			default					:
			{
				UTF8String	message (Charset::UTF_8);
				message << "Entité de type "
			        << Entity::objectTypeToObjectTypeName (_segmentIDTextField->getObjectType ( ))
			        << " (" << (unsigned long)_segmentIDTextField->getObjectType ( )
			        << ") non supporté pour la saisie d'un segment.";
				throw Exception (message);
			}	// default
		}	// switch (_segmentIDTextField->getObjectType ( ))
	}	// if (0 != _segmentIDTextField->getObjectsCount ( ))

	setDx (dx);
	setDy (dy);
	setDz (dz);

	emit vectorModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, _title)
}	// QtMgx3DVectorPanel::selectedSegmentCallback


void QtMgx3DVectorPanel::selectedPointCallback (const QString name)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_segmentIDTextField)
	CHECK_NULL_PTR_ERROR (_point1IDTextField)
	CHECK_NULL_PTR_ERROR (_point2IDTextField)

	_segmentIDTextField->blockSignals (true);
	_segmentIDTextField->clear ( );
	_segmentIDTextField->blockSignals (false);

	double	dx	= 0., dy	= 0., dz	= 0.;

	try
	{
		const vector<string>	ids1	= _point1IDTextField->getUniqueNames( );
		const vector<string>	ids2	= _point2IDTextField->getUniqueNames( );
		if ((1 != ids1.size ( )) || (1 != ids2.size ( )))
			throw Exception (UTF8String ("Il faut 2 points pour définir le vecteur.", Charset::UTF_8));

		const Math::Point	pt1	= getPoint (ids1 [0]);
		const Math::Point	pt2	= getPoint (ids2 [0]);
		dx	= pt2.getX ( ) - pt1.getX ( );
		dy	= pt2.getY ( ) - pt1.getY ( );
		dz	= pt2.getZ ( ) - pt1.getZ ( );
	}
	catch (...)
	{
	}

	setDx (dx);
	setDy (dy);
	setDz (dz);

	emit vectorModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, _title)
}	// QtMgx3DVectorPanel::selectedPointCallback


void QtMgx3DVectorPanel::coordinatesEditedCallback ( )
{ // Appelé en cas de changement des coordonnées par l'utilisateur.
  // => l'éventuel vertex correspondant aux coordonnées n'est plus vrai
  // => on l'efface
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_segmentIDTextField)
	CHECK_NULL_PTR_ERROR (_point1IDTextField)
	CHECK_NULL_PTR_ERROR (_point2IDTextField)
	_segmentIDTextField->blockSignals (true);
	_segmentIDTextField->clear ( );
	_segmentIDTextField->blockSignals (false);
	_point1IDTextField->blockSignals (true);
	_point1IDTextField->clear ( );
	_point1IDTextField->blockSignals (false);
	_point2IDTextField->blockSignals (true);
	_point2IDTextField->clear ( );
	_point2IDTextField->blockSignals (false);

	emit vectorModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, _title)
}	// QtMgx3DVectorPanel::coordinatesEditedCallback


}	// namespace QtComponents

}	// namespace Mgx3D
