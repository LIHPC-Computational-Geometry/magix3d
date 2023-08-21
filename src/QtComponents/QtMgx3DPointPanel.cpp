/**
 * \file		QtMgx3DPointPanel.cpp
 * \author		Charles PIGNEROL
 * \date		19/12/2012
 */

#include "Internal/ContextIfc.h"
#include "Internal/Resources.h"

#include "QtComponents/QtMgx3DPointPanel.h"
#include <QtUtil/QtErrorManagement.h>
#include "QtComponents/QtMgx3DMainWindow.h"
#include "QtComponents/QtNumericFieldsFactory.h"
#include "QtComponents/QtMgx3DEntityPanel.h"
#include "Utils/Common.h"
#include "Utils/Spherical.h"
#include "Utils/Cylindrical.h"
#include "Geom/Vertex.h"
#include "Topo/TopoManager.h"
#include "Topo/Vertex.h"
#include "SysCoord/SysCoord.h"
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtObjectSignalBlocker.h>
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

QtMgx3DPointPanel::QtMgx3DPointPanel (QWidget* parent, const string& title,
					bool editable, const string& xTitle,
					const string& yTitle, const string& zTitle,
					double x, double xMin, double xMax,
					double y, double yMin, double yMax,
					double z, double zMin, double zMax,
					QtMgx3DMainWindow* mainWindow,
					FilterEntity::objectType types,
					bool verticalLayout)
	: QWidget (parent), 
	  _initialX (x), _initialY (y), _initialZ (z),
	  _xTextLabel(0), _yTextLabel(0), _zTextLabel(0),
	  _xTextField (0), _yTextField (0), _zTextField (0),
	  _vertexIDTextField (0), _coordTypeComboBox(0), _sysCoordPanel(0), _title (title)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);

	QLabel*	label	= 0;
	if (false == title.empty ( ))
	{
		label	= new QLabel (title.c_str ( ), this);
		layout->addWidget (label);
	}	// if (false == title.empty ( ))

	// [EB] le type de coordonnées
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins (0, 0, 0, 0);
	layout->addLayout (hlayout);
	QLabel* typeLabel	= new QLabel ("Type de coordonnées :", this);
	hlayout->addWidget (typeLabel);
	_coordTypeComboBox = new QComboBox (this);
	hlayout->addWidget (_coordTypeComboBox);
	_coordTypeComboBox->addItem(QString::fromUtf8("Cartésiennes"));
	_coordTypeComboBox->addItem(QString::fromUtf8("Sphériques"));
	_coordTypeComboBox->addItem(QString::fromUtf8("Cylindriques"));
	hlayout->setSpacing (Resources::instance ( )._spacing.getValue ( ));

	if (true == verticalLayout)
	{
		hlayout->addStretch (2.);
		hlayout	= new QHBoxLayout (0);
		hlayout->setContentsMargins (0, 0, 0, 0);
		layout->addLayout (hlayout);
	}	// if (true == verticalLayout)
	_xTextLabel	= new QLabel (xTitle.c_str ( ), this);
	_xTextLabel->setMinimumSize (_xTextLabel->sizeHint ( ));
	hlayout->addWidget (_xTextLabel);
	_xTextField		= &QtNumericFieldsFactory::createPositionTextField (this);
	hlayout->addWidget (_xTextField);
	if (true == verticalLayout)
	{
		hlayout->addStretch (2.);
		hlayout	= new QHBoxLayout (0);
		hlayout->setContentsMargins (0, 0, 0, 0);
		layout->addLayout (hlayout);
	}	// if (true == verticalLayout)
	_yTextLabel					= new QLabel (yTitle.c_str ( ), this);
	_yTextLabel->setMinimumSize (_yTextLabel->sizeHint ( ));
	hlayout->addWidget (_yTextLabel);
	_yTextField		= &QtNumericFieldsFactory::createPositionTextField (this);
	hlayout->addWidget (_yTextField);
	if (true == verticalLayout)
	{
		hlayout->addStretch (2.);
		hlayout	= new QHBoxLayout (0);
		hlayout->setContentsMargins (0, 0, 0, 0);
		layout->addLayout (hlayout);
	}	// if (true == verticalLayout)
	_zTextLabel					= new QLabel (zTitle.c_str ( ), this);
	_zTextLabel->setMinimumSize (_zTextLabel->sizeHint ( ));
	hlayout->addWidget (_zTextLabel);
	_zTextField		= &QtNumericFieldsFactory::createPositionTextField (this);
	hlayout->addWidget (_zTextField);
	hlayout->addStretch (2.);

	if (false == editable)
	{
		_xTextField->setReadOnly (true);
		_yTextField->setReadOnly (true);
		_zTextField->setReadOnly (true);
	}	// if (false == editable)
	else
	{
		_xTextField->setRange (xMin, xMax);
		_yTextField->setRange (yMin, yMax);
		_zTextField->setRange (zMin, zMax);
	}	// else if (false == editable)
	_xTextField->setValue (x);
	_yTextField->setValue (y);
	_zTextField->setValue (z);

	// L'ID de l'éventuel vertex dont on récupère les coordonnées :
	hlayout	= new QHBoxLayout (0);
	hlayout->setContentsMargins (0, 0, 0, 0);
	layout->addLayout (hlayout);
	label	= new QLabel ("Vertex :", this);
	label->setMinimumSize (label->sizeHint ( ));
	hlayout->addWidget (label);
	_vertexIDTextField	= new QtEntityIDTextField (
							this, mainWindow, SelectionManagerIfc::D0, types);
	hlayout->addWidget (_vertexIDTextField);


	_sysCoordPanel	= new QtMgx3DEntityPanel (
			this, "", true, "Repère :", "", mainWindow,
			SelectionManagerIfc::D3, FilterEntity::SysCoord);
	_sysCoordPanel->setMultiSelectMode (false);
	layout->addWidget (_sysCoordPanel);


	connect (_vertexIDTextField, SIGNAL (selectionModified (QString)), this,
	         SLOT (selectedVertexCallback(QString)));
	connect (_vertexIDTextField, SIGNAL (entitiesAddedToSelection (QString)),
	         this, SLOT (pointsAddedToSelectionCallback (QString)));
	connect (_vertexIDTextField, SIGNAL (entitiesRemovedFromSelection(QString)),
	         this, SLOT (pointsRemovedFromSelectionCallback (QString)));

	connect (_xTextField,  SIGNAL (textChanged (const QString&)), this,
	         SLOT (coordinatesModifiedCallback ( )));
	connect (_yTextField,  SIGNAL (textChanged (const QString&)), this,
	         SLOT (coordinatesModifiedCallback ( )));
	connect (_zTextField,  SIGNAL (textChanged (const QString&)), this,
	         SLOT (coordinatesModifiedCallback ( )));
	connect (_xTextField,  SIGNAL (textEdited (const QString&)), this,
	         SLOT (coordinatesEditedCallback ( )));
	connect (_yTextField,  SIGNAL (textEdited (const QString&)), this,
	         SLOT (coordinatesEditedCallback ( )));
	connect (_zTextField,  SIGNAL (textEdited (const QString&)), this,
	         SLOT (coordinatesEditedCallback ( )));

	connect (_coordTypeComboBox, SIGNAL (currentIndexChanged (int)),
			this, SLOT (coordTypeChangedCallback (int)));

	connect (_sysCoordPanel, SIGNAL (selectionModified(const QString&)), this,
			SLOT (coordinatesModifiedCallback ( )));
//	connect (_sysCoordPanel, SIGNAL (entitiesRemovedFromSelection(const QString&)), this,
//			SLOT (coordinatesModifiedCallback ( )));

}	// QtMgx3DPointPanel::QtMgx3DPointPanel


QtMgx3DPointPanel::QtMgx3DPointPanel (const QtMgx3DPointPanel&)
	: QWidget (0),
	  _initialX (0.), _initialY (0.), _initialZ (0.), 
	  _xTextLabel(0), _yTextLabel(0), _zTextLabel(0),
	  _xTextField (0), _yTextField (0), _zTextField (0), _vertexIDTextField (0),
	  _coordTypeComboBox(0), _sysCoordPanel(0), _title ("Invalid QtMgx3DPointPanel")
{
	MGX_FORBIDDEN ("QtMgx3DPointPanel copy constructor is forbidden.")
}	// QtMgx3DPointPanel::QtMgx3DPointPanel (const QtMgx3DPointPanel&)


QtMgx3DPointPanel& QtMgx3DPointPanel::operator = (const QtMgx3DPointPanel&)
{
	MGX_FORBIDDEN ("QtMgx3DPointPanel assignment operator is forbidden.")
	return *this;
}	// QtMgx3DPointPanel::operator =


QtMgx3DPointPanel::~QtMgx3DPointPanel ( )
{
}	// QtMgx3DPointPanel::~QtMgx3DPointPanel


void QtMgx3DPointPanel::blockFieldsSignals (bool block)
{
	CHECK_NULL_PTR_ERROR (_xTextField)
	CHECK_NULL_PTR_ERROR (_yTextField)
	CHECK_NULL_PTR_ERROR (_zTextField)
	CHECK_NULL_PTR_ERROR (_vertexIDTextField)
	CHECK_NULL_PTR_ERROR (_sysCoordPanel)
	CHECK_NULL_PTR_ERROR (_coordTypeComboBox)
	_xTextField->blockSignals (block);
	_yTextField->blockSignals (block);
	_zTextField->blockSignals (block);
	_vertexIDTextField->blockSignals (block);
	_sysCoordPanel->blockSignals (block);
	_coordTypeComboBox->blockSignals (block);
}	// QtMgx3DPointPanel::blockFieldsSignals


bool QtMgx3DPointPanel::isModified ( ) const
{
	Math::Point pt;
	double x,y,z;
	try
	{
		pt = getPoint();
		x	= pt.getX ( );
		y	= pt.getY ( );
		z	= pt.getZ ( );
	}
	catch (...)
	{
		return true;
	}

	if ((x != _initialX) || (y != _initialY) || (z != _initialZ))
		return true;

	return false;
}	// QtMgx3DPointPanel::isModified


void QtMgx3DPointPanel::setX (double x)
{
	CHECK_NULL_PTR_ERROR (_xTextField)
	_xTextField->setText (QString::number (x));
}	// QtMgx3DPointPanel::setX


void QtMgx3DPointPanel::setY (double y)
{
	CHECK_NULL_PTR_ERROR (_yTextField)
	_yTextField->setText (QString::number (y));
}	// QtMgx3DPointPanel::setY


void QtMgx3DPointPanel::setZ (double z)
{
	CHECK_NULL_PTR_ERROR (_zTextField)
	_zTextField->setText (QString::number (z));
}	// QtMgx3DPointPanel::setZ


Math::Point QtMgx3DPointPanel::getPoint ( ) const
{
	bool	ok;
	double	x, y, z;

	CHECK_NULL_PTR_ERROR (_xTextField)
	ok		= true;
	x	= _xTextField->text ( ).toDouble (&ok);

	if (false == ok) {
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg << _title
		         << " : valeur invalide de la première composante ("
		         << _xTextField->text ( ).toStdString ( ) << ").";
		throw Exception (errorMsg);
	}	// if (false == ok)

	CHECK_NULL_PTR_ERROR (_yTextField)
	ok		= true;
	y	= _yTextField->text ( ).toDouble (&ok);

	if (false == ok) {
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg << _title
		         << " : valeur invalide de la deuxième composante ("
		         << _yTextField->text ( ).toStdString ( ) << ").";
		throw Exception (errorMsg);
	}	// if (false == ok)


	CHECK_NULL_PTR_ERROR (_zTextField)
	ok		= true;
	z	= _zTextField->text ( ).toDouble (&ok);

	if (false == ok) {
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg << _title
		         << " : valeur invalide de la deuxième composante ("
		         << _zTextField->text ( ).toStdString ( ) << ").";
		throw Exception (errorMsg);
	}	// if (false == ok)


	Math::Point	point (x, y, z);

	CoordinateSystem::SysCoord* rep = getSysCoord();

	if (rep) {
		point = rep->toGlobal(point);
	}

	return point;
}	// QtMgx3DPointPanel::getPoint


string QtMgx3DPointPanel::getUniqueName ( ) const
{
	CHECK_NULL_PTR_ERROR (_vertexIDTextField)
	const vector<string>	names	= _vertexIDTextField->getUniqueNames ( );
	const string			name	= 0 == names.size ( ) ? string ( ) : names [0];

	return name;
}	// QtMgx3DPointPanel::getUniqueName


void QtMgx3DPointPanel::setUniqueName (const string& name)
{
	CHECK_NULL_PTR_ERROR (_vertexIDTextField)
	QtObjectSignalBlocker	blocker1 (this);
	QtObjectSignalBlocker	blocker2 (_vertexIDTextField);
	vector<string>	names;
	if (0 != name.length ( ))
		names.push_back (name);
	_vertexIDTextField->setUniqueNames (names);
	selectedVertexCallback (name.c_str ( ));
}	// QtMgx3DPointPanel::setUniqueName


void QtMgx3DPointPanel::reset ( )
{
	try
	{
		CHECK_NULL_PTR_ERROR (_vertexIDTextField)
		CHECK_NULL_PTR_ERROR (_xTextField)
		CHECK_NULL_PTR_ERROR (_yTextField)
		CHECK_NULL_PTR_ERROR (_zTextField)
		_vertexIDTextField->reset ( );
		_xTextField->setValue (_initialX);
		_yTextField->setValue (_initialY);
		_zTextField->setValue (_initialZ);
	}
	catch (...)
	{
	}
}	// QtMgx3DPointPanel::reset



void QtMgx3DPointPanel::clearSelection ( )
{
	CHECK_NULL_PTR_ERROR (_vertexIDTextField)
	_vertexIDTextField->clearSelection ( );
}	// QtMgx3DPointPanel::clearSelection


void QtMgx3DPointPanel::stopSelection ( )
{
	CHECK_NULL_PTR_ERROR (_vertexIDTextField)
	_vertexIDTextField->setInteractiveMode (false);
}	// QtMgx3DPointPanel::stopSelection


bool QtMgx3DPointPanel::actualizeGui (bool removeDestroyed)
{
	CHECK_NULL_PTR_ERROR (_vertexIDTextField)
	return _vertexIDTextField->actualizeGui (removeDestroyed);
}	// QtMgx3DPointPanel::actualizeGui


void QtMgx3DPointPanel::selectedVertexCallback (const QString name)
{
	BEGIN_QT_TRY_CATCH_BLOCK

	updateCoordinates (name.toStdString ( ));
	emit pointModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, _title)
}	// QtMgx3DPointPanel::selectedVertexCallback


void QtMgx3DPointPanel::pointAddedToSelectionCallback (QString pointName)
{
	updateCoordinates (pointName.toStdString ( ));
	emit pointAddedToSelection (pointName);
}	// QtMgx3DPointPanel::pointAddedToSelectionCallback


void QtMgx3DPointPanel::pointRemovedFromSelectionCallback (QString pointName)
{
	emit pointRemovedFromSelection (pointName);
}	// QtMgx3DPointPanel::pointRemovedFromSelectionCallback


void QtMgx3DPointPanel::pointsAddedToSelectionCallback (QString pointsNames)
{
	updateCoordinates (pointsNames.toStdString ( ));
	emit pointAddedToSelection (pointsNames);
}	// QtMgx3DPointPanel::pointsAddedToSelectionCallback


void QtMgx3DPointPanel::pointsRemovedFromSelectionCallback (QString pointsNames)
{
	emit pointRemovedFromSelection (pointsNames);
}	// QtMgx3DPointPanel::pointsRemovedFromSelectionCallback


void QtMgx3DPointPanel::coordinatesEditedCallback ( )
{	// Appelé en cas de changement des coordonnées par l'utilisateur.
	// => l'éventuel vertex correspondant aux coordonnées n'est plus vrai
	// => on l'efface
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_vertexIDTextField)
	_vertexIDTextField->blockSignals (true);
	_vertexIDTextField->clear ( );
	_vertexIDTextField->blockSignals (false);
	emit pointModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, _title)
}	// QtMgx3DPointPanel::coordinatesEditedCallback


void QtMgx3DPointPanel::coordinatesModifiedCallback ( )
{	// Appelé en cas de changement des coordonnées, que ce soit du fait de
	// l'utilisateur ou du programme.
	BEGIN_QT_TRY_CATCH_BLOCK

	CHECK_NULL_PTR_ERROR (_vertexIDTextField)
	emit pointModified ( );

	COMPLETE_QT_TRY_CATCH_BLOCK (true, this, _title)
}	// QtMgx3DPointPanel::coordinatesModifiedCallback


void QtMgx3DPointPanel::updateCoordinates (const string& name)
{

	CHECK_NULL_PTR_ERROR (_vertexIDTextField)

	Utils::Math::Point pt;

	if (0 != _vertexIDTextField->getObjectsCount ( ))
	{
		switch (_vertexIDTextField->getObjectType ( ))
		{
			case Entity::GeomVertex	:
			{
				const Geom::Vertex*	vertex	=
					getContext ().getGeomManager ( ).getVertex (name, true);
				CHECK_NULL_PTR_ERROR (vertex);
				pt = vertex->getCoord();
			}	// case Entity::GeomVertex
			break;
			case Entity::TopoVertex	:
			{
				const Topo::Vertex*	vertex	=
					getContext ().getTopoManager ( ).getVertex (name, true);
				CHECK_NULL_PTR_ERROR (vertex);
				pt = vertex->getCoord();
			}	// case Entity::TopoVertex
			break;
			default					:
			{
				UTF8String	message (Charset::UTF_8);
				message << "Entité de type "
			        << Entity::objectTypeToObjectTypeName (_vertexIDTextField->getObjectType ( ))
			        << " (" << (unsigned long)_vertexIDTextField->getObjectType ( )
			        << ") non supporté pour la saisie d'un point.";
				throw Exception (message);
			}	// default
		}	// switch (_vertexIDTextField->getObjectType ( ))
	}	// if (0 != _vertexIDTextField->getObjectsCount ( ))

	CHECK_NULL_PTR_ERROR (_xTextField)
	CHECK_NULL_PTR_ERROR (_yTextField)
	CHECK_NULL_PTR_ERROR (_zTextField)

	// on met les coordonnées dans le repère sélectionné si c'est le cas
	CoordinateSystem::SysCoord* rep = getSysCoord();

	if (rep) {
		pt = rep->toLocal(pt);
	}


	coordinateType ct = getTypeCoordinate();

	if (ct == cartesian){
		_xTextField->setText (QString::number (pt.getX()));
		_yTextField->setText (QString::number (pt.getY()));
		_zTextField->setText (QString::number (pt.getZ()));
	}
	else if (ct == spherical){
		Utils::Math::Spherical sph(pt);
		_xTextField->setText (QString::number (sph.getRho()));
		_yTextField->setText (QString::number (sph.getTheta()));
		_zTextField->setText (QString::number (sph.getPhi()));
	}
	else if (ct == cylindrical){
		Utils::Math::Cylindrical cyl(pt);
		_xTextField->setText (QString::number (cyl.getRho()));
		_yTextField->setText (QString::number (cyl.getPhi()));
		_zTextField->setText (QString::number (cyl.getZ()));
	}
	else
		throw Exception ("Erreur interne, coordinateType de updateCoordinates en dehors des clous");

}	// QtMgx3DPointPanel::updateCoordinates


CoordinateSystem::SysCoord* QtMgx3DPointPanel::getSysCoord() const
{
	CHECK_NULL_PTR_ERROR (_sysCoordPanel);
	string repName = _sysCoordPanel->getUniqueName();

	if (!repName.empty()){
		return getContext().getLocalSysCoordManager().getSysCoord(repName, true);
	}
	else
		return 0;
}

Internal::Context& QtMgx3DPointPanel::getContext() const
{
	// TODO peu mieux faire ...
	CHECK_NULL_PTR_ERROR(_vertexIDTextField);
	Internal::ContextIfc* ctxifc = &_vertexIDTextField->getMainWindow ( ).getContext();
	Internal::Context* ctx = dynamic_cast<Internal::Context*>(ctxifc);
	CHECK_NULL_PTR_ERROR(ctx);
	return *ctx;
}


void QtMgx3DPointPanel::coordTypeChangedCallback (int index)
{
	CHECK_NULL_PTR_ERROR(_xTextLabel)
	CHECK_NULL_PTR_ERROR(_yTextLabel)
	CHECK_NULL_PTR_ERROR(_zTextLabel)

	coordinateType ct = getTypeCoordinate();
	if (ct == cartesian){
		_xTextLabel->setText("X :");
		_yTextLabel->setText("Y :");
		_zTextLabel->setText("Z :");
	}
	else if (ct == spherical){
		_xTextLabel->setText("Rhô :");
		_yTextLabel->setText("Thêta :");
		_zTextLabel->setText("Phi :");
	}
	else if (ct == cylindrical){
		_xTextLabel->setText("Rhô :");
		_yTextLabel->setText("Phi :");
		_zTextLabel->setText("Z :");
	}
	else
		throw Exception ("Erreur interne, coordinateType de coordTypeChangedCallback en dehors des clous");

	if (_vertexIDTextField->getObjectsCount ( ) != 0)
		updateCoordinates(_vertexIDTextField->getText());

} // QtTopologyProjectVerticesPanel::coordTypeChangedCallback


QtMgx3DPointPanel::coordinateType QtMgx3DPointPanel::getTypeCoordinate()
{
	CHECK_NULL_PTR_ERROR(_coordTypeComboBox);
	if (_coordTypeComboBox->currentIndex() == 0)
		return cartesian;
	else if (_coordTypeComboBox->currentIndex() == 1)
		return spherical;
	else if (_coordTypeComboBox->currentIndex() == 2)
		return cylindrical;
	else
		throw Exception ("Erreur interne, index de _coordTypeComboBox en dehors des clous");
}


}	// namespace QtComponents

}	// namespace Mgx3D
