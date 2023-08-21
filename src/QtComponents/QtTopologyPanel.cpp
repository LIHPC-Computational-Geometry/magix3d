/**
 * \file        QtTopologyPanel.cpp
 * \author      Charles PIGNEROL
 * \date        18/12/2012
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtTopologyPanel.h"

#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtObjectSignalBlocker.h>

#include <QLabel>
#include <QHBoxLayout>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::QtComponents;


namespace Mgx3D
{

namespace QtComponents
{

// ===========================================================================
//                        LA CLASSE QtTopologyPanel
// ===========================================================================


static const char*	coordsNames [QtTopologyPanel::OGRID+1][3]	=
	{ "invalid", "invalid", "invalid",
	  "ni :", "nj :", "nk :", "nr :", " nt :", "nz :",
	  "nr :", "nt :", "np :", "n axe :", " ni :", "nr :" };

static const char*	coordsTips [QtTopologyPanel::OGRID+1][3] =
	{
"champ invalide", "champ invalide", "champ invalide",
"ni est le nombre de bras selon l'axe des abscisses (X).",
"nj est le nombre de bras selon l'axe des ordonnées (Y).",
"nk est le nombre de bras selon l'axe des élévations (Z).",
"nr est le nombre de bras selon l'axe rau.",
"nt est le nombre de couches selon l'angle theta.",
"nz est le nombre de bras selon l'axe des élévations (Z).",
"nr est le nombre de bras selon l'axe rau.",
"nt est le nombre de couches selon l'angle theta.",
"np est le nombre de couches selon l'angle phi.",
"n axe est le nombre de bras selon l'axe du cylindre.",
"ni est le nombre de bras entre le point triple et l'axe de symétrie.",
"nr est le nombre de couches entre le bloc central et la surface externe."
};

string QtTopologyPanel::typeToName (
						QtTopologyPanel::TOPOLOGY_TYPE type, bool use3DLabel)
{
	switch (type)
	{
		case QtTopologyPanel::UNSTRUCTURED_TOPOLOGY		:
			if (true == use3DLabel)
				return "Bloc non structuré";
			else
				return "Face non structurée";
		case QtTopologyPanel::STRUCTURED_TOPOLOGY		:
			if (true == use3DLabel)
				return "Bloc structuré";
			else
				return "Face structurée";
		case QtTopologyPanel::STRUCTURED_FREE_TOPOLOGY		:
			if (true == use3DLabel)
				return "Bloc structuré libre (sans association)";
			else
				return "Face structurée libre (sans association)";
		case QtTopologyPanel::OGRID_BLOCKS					:
			if (true == use3DLabel)
				return "Blocs structurés en O-grid";
			else
				return "Faces structurées en O-grid";
		case QtTopologyPanel::INSERTION_TOPOLOGY		:
			return "Bloc pour l'insertion";
	}	// switch (type)

	UTF8String	error (Charset::UTF_8);
	error << "Type de topologie (" << (unsigned long)type
	      << ") non recensée dans QtTopologyPanel::typeToName.";
	throw Exception (error);
}	// QtTopologyPanel::typeToName


QtTopologyPanel::QtTopologyPanel (
	QWidget* parent, QtMgx3DMainWindow& mainWindow,
	bool createTopology, bool allowGroup, int groupDimension,
	QtTopologyPanel::TOPOLOGY_TYPE type,
	QtTopologyPanel::COORDINATES_TYPE coordsType,
	int axe1Default, int axe2Default, int axe3Default, bool use3DLabels,
	bool verticalLayout)
	: QWidget (parent), ValidatedField ( ),
	  _createTopologyCheckBox (0), _groupNamePanel (0),
	  _topologyComboBox (0),
	  _topologyType (QtTopologyPanel::UNSTRUCTURED_TOPOLOGY),
	  _coordinatesType (coordsType),
	  _defaultCreateTopology (createTopology), _defaultTopologyType (type),
	  _oGridLabel (0), _oGridRatioTextField (0),
	  _axe1NumLabel (0), _axe2NumLabel (0), _axe3NumLabel (0),
	  _axe1NumTextField (0), _axe2NumTextField (0),
	  _axe3NumTextField (0),
	  _defaultOGridRatio (0.5),
	  _defaultAxe1Num (axe1Default), _defaultAxe2Num (axe2Default),
	  _defaultAxe3Num (axe3Default), _use3DLabels (use3DLabels)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);

	// Créer une topologie associée ?
	_createTopologyCheckBox	=
					new QCheckBox (QString::fromUtf8("Création d'une topologie associée"), this);
	layout->addWidget (_createTopologyCheckBox);
	connect (_createTopologyCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (createTopologyCallback (int)));

	// Le groupe de rattachement :
	if (true == allowGroup)
	{
		_groupNamePanel	= new QtMgx3DGroupNamePanel (
							this, "Groupe", mainWindow, groupDimension,
							QtMgx3DGroupNamePanel::CREATION, "");
		layout->addWidget (_groupNamePanel);
	}	// if (true == allowGroup)

	// Le type de topologie à créer :
	QHBoxLayout*	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	QLabel*	label	= new QLabel ("Type", this);
	hlayout->addWidget (label);
	_topologyComboBox	= new QComboBox (this);
	hlayout->addWidget (_topologyComboBox, 1000);
	connect (_topologyComboBox, SIGNAL (currentIndexChanged (int)), this,
	         SLOT (topologyTypeCallback (int)));
	QtObjectSignalBlocker	blocker (_topologyComboBox);
	for (size_t t = (size_t)QtTopologyPanel::STRUCTURED_TOPOLOGY;
	     t <= (size_t)QtTopologyPanel::INSERTION_TOPOLOGY; t++)
		_topologyComboBox->addItem (QString::fromUtf8(
			typeToName ((QtTopologyPanel::TOPOLOGY_TYPE)t,use3DLabels).c_str( )),
			QVariant ((qulonglong)t));

	// Si O-grid, paramétrage de cette topologie :
	hlayout	= new QHBoxLayout (0);
	layout->addLayout (hlayout);
	_oGridLabel	= new QLabel ("Ratio O-grid", this);
	hlayout->addWidget (_oGridLabel);
	_oGridRatioTextField	= new QtDoubleTextField (_defaultOGridRatio, this);
	hlayout->addWidget (_oGridRatioTextField);
//	_oGridRatioTextField->setRange (0., 1.);
	_oGridRatioTextField->setNotation (QDoubleValidator::StandardNotation);
	hlayout->addStretch (10);

	// naxe, ni, nr :
	// Nombres de bras :
	if ((0 <= axe1Default) || (0 <= axe2Default) || (0 <= axe3Default))
	{
	    hlayout = new QHBoxLayout (0);
		hlayout->setSpacing (QtConfiguration::spacing);
		hlayout->setContentsMargins (
						QtConfiguration::margin, QtConfiguration::margin,
						QtConfiguration::margin, QtConfiguration::margin);
		layout->addLayout (hlayout);

		int	width	= 0;
		if (0 <= axe1Default)
		{
			if (true == verticalLayout)
			{
				hlayout->setContentsMargins (0, 0, 0, 0);
			}
			_axe1NumLabel	= new QLabel (coordsNames[coordsType][0], this);
			hlayout->addWidget (_axe1NumLabel);
			_axe1NumTextField	= new QtIntTextField (this);
			_axe1NumTextField->setValue(6);
			_axe1NumTextField->setVisibleColumns (6);
			hlayout->addWidget (_axe1NumTextField, 1);
			_axe1NumTextField->setFixedSize (
										_axe1NumTextField->sizeHint ( ));
			_axe1NumLabel->setToolTip (QString::fromUtf8(coordsTips[coordsType][0]));
			_axe1NumTextField->setToolTip (QString::fromUtf8(coordsTips[coordsType][0]));
			int	tmp	= _axe1NumLabel->sizeHint ( ).width ( );
			width	= tmp > width ? tmp : width;
		}	// if (0 <= axe1Default)
		if (0 <= axe2Default)
		{
			if (true == verticalLayout)
			{
				hlayout->addStretch (10.);
				hlayout = new QHBoxLayout (0);
				hlayout->setContentsMargins (0, 0, 0, 0);
				layout->addLayout (hlayout);
			}	// if (true == verticalLayout)
			_axe2NumLabel	= new QLabel (coordsNames[coordsType][1], this);
			hlayout->addWidget (_axe2NumLabel);
			_axe2NumTextField = new QtIntTextField (this);
			_axe2NumTextField->setValue(6);
			_axe2NumTextField->setVisibleColumns (6);
			hlayout->addWidget (_axe2NumTextField, 1);
			_axe2NumTextField->setFixedSize (
										_axe2NumTextField->sizeHint ( ));
			_axe2NumLabel->setToolTip (QString::fromUtf8(coordsTips[coordsType][1]));
			_axe2NumTextField->setToolTip (QString::fromUtf8(coordsTips[coordsType][1]));
			int	tmp	= _axe2NumLabel->sizeHint ( ).width ( );
			width	= tmp > width ? tmp : width;
		}	// if (0 <= axe2Default)
		if (0 <= axe3Default)
		{
			if (true == verticalLayout)
			{
				hlayout->addStretch (10.);
				hlayout = new QHBoxLayout (0);
				hlayout->setContentsMargins (0, 0, 0, 0);
				layout->addLayout (hlayout);
			}	// if (true == verticalLayout)
			_axe3NumLabel	= new QLabel (coordsNames[coordsType][2], this);
			hlayout->addWidget (_axe3NumLabel);
			_axe3NumTextField = new QtIntTextField (this);
			_axe3NumTextField->setValue(6);
			_axe3NumTextField->setVisibleColumns (6);
			hlayout->addWidget (_axe3NumTextField, 1);
			_axe3NumTextField->setFixedSize (_axe3NumTextField->sizeHint ( ));
			_axe3NumLabel->setToolTip (QString::fromUtf8(coordsTips[coordsType][2]));
			_axe3NumTextField->setToolTip (QString::fromUtf8(coordsTips[coordsType][2]));
			hlayout->addStretch (10);
			int	tmp	= _axe3NumLabel->sizeHint ( ).width ( );
			width	= tmp > width ? tmp : width;
		}	// if (0 <= axe3Default)

		if (0 != _axe1NumLabel)
			_axe1NumLabel->setFixedWidth (width);
		if (0 != _axe2NumLabel)
			_axe2NumLabel->setFixedWidth (width);
		if (0 != _axe3NumLabel)
			_axe3NumLabel->setFixedWidth (width);
	}	// if ((0 <= axe1Default) || (0 <= axe2Default) || ...

	// Actualisation IHM :
	setTopologyType (type);
	createTopologyCallback (0);
	topologyTypeCallback (0);
}	// QtTopologyPanel::QtTopologyPanel


QtTopologyPanel::QtTopologyPanel (const QtTopologyPanel&)
	: QWidget (0), ValidatedField ( ),
	  _createTopologyCheckBox (0), _groupNamePanel (0),
	  _topologyComboBox (0),
	  _topologyType (QtTopologyPanel::UNSTRUCTURED_TOPOLOGY),
	  _coordinatesType (QtTopologyPanel::NO_COORDINATES),
	  _defaultCreateTopology (false),
	  _defaultTopologyType (QtTopologyPanel::UNSTRUCTURED_TOPOLOGY),
	  _oGridLabel (0), _oGridRatioTextField (0),
	  _axe1NumLabel (0), _axe2NumLabel (0), _axe3NumLabel (0),
	  _axe1NumTextField (0), _axe2NumTextField (0),
	  _axe3NumTextField (0),
	  _defaultOGridRatio (0.5),
	  _defaultAxe1Num (-1), _defaultAxe2Num (-1), _defaultAxe3Num (-1),
	  _use3DLabels (true)
{
	MGX_FORBIDDEN ("QtTopologyPanel copy constructor is not allowed.");
}	// QtTopologyPanel::QtTopologyPanel (const QtTopologyPanel&)


QtTopologyPanel& QtTopologyPanel::operator = (const QtTopologyPanel&)
{
	MGX_FORBIDDEN ("QtTopologyPanel assignment operator is not allowed.");
	return *this;
}	// QtTopologyPanel::QtTopologyPanel (const QtTopologyPanel&)


QtTopologyPanel::~QtTopologyPanel ( )
{
}	// QtTopologyPanel::~QtTopologyPanel


bool QtTopologyPanel::createTopology ( ) const
{
	CHECK_NULL_PTR_ERROR (_createTopologyCheckBox)
	return _createTopologyCheckBox->isChecked ( );
}	// QtTopologyPanel::createTopology


void QtTopologyPanel::createTopology (bool create)
{
	CHECK_NULL_PTR_ERROR (_createTopologyCheckBox)
	_createTopologyCheckBox->setChecked (create);
}	// QtTopologyPanel::createTopology


string QtTopologyPanel::getGroupName ( ) const
{
	if (0 == _groupNamePanel)
	{
		INTERNAL_ERROR (exc, "Rattachement à un groupe non prévu au sein de ce panneau.", "QtTopologyPanel::getGroupName")
		throw exc;
	}

	return _groupNamePanel->getGroupName ( );
}	// QtTopologyPanel::getGroupName


void QtTopologyPanel::allowGroupName (bool allow)
{
	if (0 == _groupNamePanel)
	{
		INTERNAL_ERROR (exc, "Rattachement à un groupe non prévu au sein de ce panneau.", "QtTopologyPanel::allowGroupName")
		throw exc;
	}

	if (true == allow)
		_groupNamePanel->autoUpdate ( );
	_groupNamePanel->setEnabled (allow);
}	// QtTopologyPanel::allowGroupName


void QtTopologyPanel::displayTopologyCreation (bool enable)
{
	 CHECK_NULL_PTR_ERROR (_createTopologyCheckBox)
	_createTopologyCheckBox->setVisible (enable);
}	// QtTopologyPanel::displayTopologyCreation


QtTopologyPanel::TOPOLOGY_TYPE QtTopologyPanel::getTopologyType ( ) const
{
	CHECK_NULL_PTR_ERROR (_topologyComboBox)
	QVariant	v	= _topologyComboBox->itemData (
										_topologyComboBox->currentIndex ( ));
	return (QtTopologyPanel::TOPOLOGY_TYPE)(v.toULongLong ( ));
}	// QtTopologyPanel::getTopologyType


void QtTopologyPanel::setTopologyType (QtTopologyPanel::TOPOLOGY_TYPE type)
{
	CHECK_NULL_PTR_ERROR (_topologyComboBox)
	if (getTopologyType ( ) == type)
		return;

	int	index	= _topologyComboBox->findData (QVariant ((qulonglong)type));
	_topologyComboBox->setCurrentIndex (index);
}	// QtTopologyPanel::setTopologyType


void QtTopologyPanel::enableTopologyType (
							QtTopologyPanel::TOPOLOGY_TYPE type, bool enable)
{
	CHECK_NULL_PTR_ERROR (_topologyComboBox)
	int	index	= _topologyComboBox->findData (QVariant ((qulonglong)type));
	if (false == enable)
	{
		if (-1 != index)
		{
			QtTopologyPanel::TOPOLOGY_TYPE	t	= getTopologyType ( );
			_topologyComboBox->removeItem (index);
			setTopologyType (t);
		}	// if (-1 != index)

	}	// if (false == enable)
	else
	{
		if (-1 == index)
			_topologyComboBox->addItem (QString::fromUtf8(
					typeToName ((QtTopologyPanel::TOPOLOGY_TYPE)type,
													 _use3DLabels).c_str ( )),
					QVariant ((qulonglong)type));
	}	// else if (false == enable)
}	// QtTopologyPanel::enableTopologyType


QtTopologyPanel::TOPOLOGY_TYPE QtTopologyPanel::getDefaultTopologyType ( ) const
{
	return _defaultTopologyType;
}	// QtTopologyPanel::getDefaultTopologyType


QtTopologyPanel::COORDINATES_TYPE QtTopologyPanel::getCoordinatesType ( ) const
{
	return _coordinatesType;
}	// QtTopologyPanel::getCoordinatesType


double QtTopologyPanel::getOGridRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_oGridRatioTextField)
	return _oGridRatioTextField->getValue ( );
}	// QtTopologyPanel::getOGridRatio


void QtTopologyPanel::setOGridRatio (double ratio)
{
	CHECK_NULL_PTR_ERROR (_oGridRatioTextField)
	_oGridRatioTextField->setValue (ratio);
}	// QtTopologyPanel::setOGridRatio


void QtTopologyPanel::enableOGridRatio (bool enable)
{
	CHECK_NULL_PTR_ERROR (_oGridRatioTextField)
	_oGridRatioTextField->setEnabled (enable);
}	// QtTopologyPanel::enableOGridRatio


int QtTopologyPanel::getAxe1EdgesNum ( ) const
{
	if (0 == _axe1NumTextField)
		throw Exception (UTF8String ("QtTopologyPanel::getAxe1EdgesNum : champ de saisie non instancié.", Charset::UTF_8));
	return _axe1NumTextField->getValue ( );
}   // QtTopologyPanel::getAxe1EdgesNum


void QtTopologyPanel::setAxe1EdgesNum (int edgesNum)
{
	if (0 == _axe1NumTextField)
		throw Exception (UTF8String ("QtTopologyPanel::setAxe1EdgesNum : champ de saisie non instancié.", Charset::UTF_8));
	_axe1NumTextField->setValue (edgesNum);
}	// QtTopologyPanel::setAxe1EdgesNum


int QtTopologyPanel::getAxe2EdgesNum ( ) const
{
	if (0 == _axe2NumTextField)
		throw Exception (UTF8String ("QtTopologyPanel::getAxe2EdgesNum : champ de saisie non instancié.", Charset::UTF_8));
	return _axe2NumTextField->getValue ( );
}   // QtTopologyPanel::getAxe2EdgesNum


void QtTopologyPanel::setAxe2EdgesNum (int edgesNum)
{
	if (0 == _axe2NumTextField)
		throw Exception (UTF8String ("QtTopologyPanel::setAxe2EdgesNum : champ de saisie non instancié.", Charset::UTF_8));
	_axe2NumTextField->setValue (edgesNum);
}	// QtTopologyPanel::setAxe2EdgesNum


int QtTopologyPanel::getAxe3EdgesNum ( ) const
{
	if (0 == _axe3NumTextField)
		throw Exception (UTF8String ("QtTopologyPanel::getAxe3EdgesNum : champ de saisie non instancié.", Charset::UTF_8));
	return _axe3NumTextField->getValue ( );
}   // QtTopologyPanel::getAxe3EdgesNum


void QtTopologyPanel::setAxe3EdgesNum (int num)
{
	if (0 == _axe3NumTextField)
		throw Exception (UTF8String ("QtTopologyPanel::setAxe3EdgesNum : champ de saisie non instancié.", Charset::UTF_8));
	_axe3NumTextField->setValue (num);
}	// QtTopologyPanel::setAxe3EdgesNum


void QtTopologyPanel::validate ( )
{
	// On s'assure ici qu'aucune exception ne sera levée :
	if (true == createTopology ( ))
	{
		switch (getTopologyType ( ))
		{
			case QtTopologyPanel::UNSTRUCTURED_TOPOLOGY		:
			case QtTopologyPanel::STRUCTURED_TOPOLOGY		:
			case QtTopologyPanel::STRUCTURED_FREE_TOPOLOGY  :
			case QtTopologyPanel::INSERTION_TOPOLOGY		:
				break;
			case QtTopologyPanel::OGRID_BLOCKS					:
				getOGridRatio ( );
				if (0 != _axe1NumTextField)
					getAxe1EdgesNum ( );
				if (0 != _axe2NumTextField)
					getAxe2EdgesNum ( );
				if (0 != _axe3NumTextField)
					getAxe3EdgesNum ( );
				break;
			default												:
			{
				UTF8String	error (Charset::UTF_8);
				error << "Type de topologie ("
				      << (unsigned long)getTopologyType ( )
				      << ") non recensée.";
				INTERNAL_ERROR (exc, "QtTopologyPanel::validate", error)
				throw Exception (exc);
			}
		}	// switch (getTopologyType ( ))
	}	// if (true == createTopology ( ))
}	// QtTopologyPanel::validate


void QtTopologyPanel::autoUpdate ( )
{
	// TODO mettre ce qui suit dans un [re]init  [EB]
	CHECK_NULL_PTR_ERROR (_createTopologyCheckBox)
	setTopologyType (_defaultTopologyType);
	_createTopologyCheckBox->setChecked (_defaultCreateTopology);
	setOGridRatio (_defaultOGridRatio);
	if (0 != _groupNamePanel)
		_groupNamePanel->autoUpdate ( );
	if (0 != _axe1NumTextField)
		setAxe1EdgesNum (_defaultAxe1Num);
	if (0 != _axe2NumTextField)
		setAxe2EdgesNum (_defaultAxe2Num);
	if (0 != _axe3NumTextField)
		setAxe3EdgesNum (_defaultAxe3Num);
}	// QtTopologyPanel::autoUpdate


void QtTopologyPanel::createTopologyCallback (int)
{
	updatePanel ( );

	emit topologyCreationModified ( );
}	// QtTopologyPanel::createTopologyCallback


void QtTopologyPanel::topologyTypeCallback (int)
{
	updatePanel ( );

	emit topologyTypeModified ( );
}	// QtTopologyPanel::topologyTypeCallback


void QtTopologyPanel::updatePanel ( )
{
	try
	{

		CHECK_NULL_PTR_ERROR (_createTopologyCheckBox)
		CHECK_NULL_PTR_ERROR (_topologyComboBox)
		CHECK_NULL_PTR_ERROR (_oGridLabel)
		CHECK_NULL_PTR_ERROR (_oGridRatioTextField)
		const bool	topologyCreationEnabled	=
										_createTopologyCheckBox->isChecked( );
		_topologyComboBox->setEnabled (topologyCreationEnabled);
		bool	ogrid		= false;
		bool	axe1		= false, axe2	= false, axe3	= false;;

		if (true == topologyCreationEnabled)
		{
			switch (getTopologyType ( ))
			{
				case QtTopologyPanel::UNSTRUCTURED_TOPOLOGY	:
				case QtTopologyPanel::INSERTION_TOPOLOGY    :
					break;
				case QtTopologyPanel::STRUCTURED_TOPOLOGY	:
				case QtTopologyPanel::STRUCTURED_FREE_TOPOLOGY	:
					axe1	= axe2	= axe3	= true;
					break;
				case QtTopologyPanel::OGRID_BLOCKS				:
					ogrid	= true;
					axe1	= axe2	= axe3	= true;
			}	// switch (getTopologyType ( ))
			switch (getCoordinatesType ( ))
			{
				case QtTopologyPanel::NO_COORDINATES	:
					axe1	= axe2	= axe3	= false;
					break;
				case QtTopologyPanel::CARTESIAN			:
				case QtTopologyPanel::CYLINDRIC			:
				case QtTopologyPanel::POLAR				:
				case QtTopologyPanel::OGRID				: ;
			}	// switch (getCoordinatesType ( ))
		}	// if (true == topologyCreationEnabled)

		_oGridLabel->setVisible (ogrid);
		_oGridRatioTextField->setVisible (ogrid);
		if (0 != _axe1NumLabel)
			_axe1NumLabel->setVisible (axe1);
		if (0 != _axe1NumTextField)
			_axe1NumTextField->setVisible (axe1);
		if (0 != _axe2NumLabel)
			_axe2NumLabel->setVisible (axe2);
		if (0 != _axe2NumTextField)
			_axe2NumTextField->setVisible (axe2);
		if (0 != _axe3NumLabel)
			_axe3NumLabel->setVisible (axe3);
		if (0 != _axe3NumTextField)
			_axe3NumTextField->setVisible (axe3);
	}
	catch (const Exception& exc)
	{
		cerr << __FILE__ << ' ' << __LINE__
		     << " Erreur dans QtTopologyPanel::updatePanel : "
		     << exc.getFullMessage ( ) << endl;
	}
	catch (...)
	{
		cerr << __FILE__ << ' ' << __LINE__ << " Erreur non documentée dans "
		     << "QtTopologyPanel::updatePanel." << endl;
	}
}	// QtTopologyPanel::updatePanel


// ============================================================================
//                             FONCTIONS STATIQUES
// ============================================================================



}	// namespace QtComponents

}	// namespace Mgx3D
