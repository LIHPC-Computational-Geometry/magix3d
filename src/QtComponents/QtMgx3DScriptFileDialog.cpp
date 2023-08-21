#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtMgx3DScriptFileDialog.h"

#include <QtUtil/QtUnicodeHelper.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include "Internal/ContextIfc.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QButtonGroup>

#include <assert.h>
#include <strings.h>

using namespace std;
using namespace TkUtil;
using namespace Mgx3D;


namespace Mgx3D
{

namespace QtComponents
{

unsigned int	QtMgx3DScriptFileDialog::encodageScript	= 0;


QtMgx3DScriptFileDialog::QtMgx3DScriptFileDialog (
			QWidget* parent, const char* title, bool open, Charset::CHARSET charset, bool modal)
	: QFileDialog (parent),
	  _saveEntitiesNamesCheckBox (0), _saveEntitiesIdRefCheckBox(0),
	  _saveEntitiesCoordCheckBox(0), _charsetComboBox (0)
{
	setWindowTitle (title);
	setModal (modal);
	setOption (QFileDialog::DontUseNativeDialog);	// Sinon plantage sous Cent OS 7 / Gnome / Qt 5.9.1
	if (true == modal)
		setWindowModality (Qt::WindowModal);

	createGui (open, charset);
}	// QtMgx3DScriptFileDialog::QtMgx3DScriptFileDialog


QtMgx3DScriptFileDialog::QtMgx3DScriptFileDialog (const QtMgx3DScriptFileDialog&)
	: QFileDialog (0), _saveEntitiesNamesCheckBox (0), _saveEntitiesIdRefCheckBox(0),
	  _saveEntitiesCoordCheckBox(0), _charsetComboBox (0)
{
	MGX_FORBIDDEN ("QtMgx3DScriptFileDialog copy constructor is not allowed.");
}	// QtMgx3DScriptFileDialog::QtMgx3DScriptFileDialog (const QtMgx3DScriptFileDialog&)


QtMgx3DScriptFileDialog& QtMgx3DScriptFileDialog::operator = (const QtMgx3DScriptFileDialog&)
{
	MGX_FORBIDDEN ("QtMgx3DScriptFileDialog operator = is not allowed.");
	return *this;
}	// QtMgx3DScriptFileDialog::operator =


QtMgx3DScriptFileDialog::~QtMgx3DScriptFileDialog ( )
{
}	// QtMgx3DScriptFileDialog::~QtMgx3DScriptFileDialog


void QtMgx3DScriptFileDialog::accept ( )
{
	try
	{
		encodageScript	= getEncodageScript ( );
		QFileDialog::accept ( );
	}
	catch (...)
	{
	}
}	// QtMgx3DScriptFileDialog::accept


unsigned int QtMgx3DScriptFileDialog::getEncodageScript ( ) const
{
	CHECK_NULL_PTR_ERROR(_saveEntitiesNamesCheckBox);
	CHECK_NULL_PTR_ERROR(_saveEntitiesIdRefCheckBox);
	CHECK_NULL_PTR_ERROR(_saveEntitiesCoordCheckBox);

	if (Qt::Checked == _saveEntitiesNamesCheckBox->checkState ( ))
		return Internal::ContextIfc::WITHNAMES;
	else if (Qt::Checked == _saveEntitiesIdRefCheckBox->checkState ( ))
		return Internal::ContextIfc::WITHIDREF;
	else if (Qt::Checked == _saveEntitiesCoordCheckBox->checkState ( ))
		return Internal::ContextIfc::WITHCOORD;
	else {
		MGX_FORBIDDEN ("QtMgx3DScriptFileDialog::getEncodageScrip, cas non prévu");
	}
	return Internal::ContextIfc::WITHNAMES;
}


Charset::CHARSET QtMgx3DScriptFileDialog::getCharset ( ) const
{
	CHECK_NULL_PTR_ERROR (_charsetComboBox)
	return _charsetComboBox->getCharset ( );
}	// QtMgx3DScriptFileDialog::getCharset


void QtMgx3DScriptFileDialog::createGui (bool open, Charset::CHARSET charset)
{
	setFileMode (true == open ? ExistingFiles : AnyFile);
	setAcceptMode (true == open ? AcceptOpen : AcceptSave);
	setLabelText (FileName, "Fichier");
	setLabelText (Accept, true == open ? "Ouvrir" : "Enregistrer");
	setLabelText (Reject, "Annuler");

	if (0 == layout ( ))
	{
		UTF8String	msg (Charset::UTF_8);
		msg << "Impossibilité d'étendre les possibilités du sélecteur de fichier : "
		    << "absence de layout dans le sélecteur de fichier Qt.";
		ConsoleOutput::cerr ( ) << msg << co_endl;
		return;
	}	// if (0 == layout ( )) 

	QWidget* 	extension	= new QWidget ( );
	QVBoxLayout*	mainLayout	= new QVBoxLayout ( );
	extension->setLayout (mainLayout);

	// [EB] utilisation de checkbox pour sélectionner un des modes de sauvegarde
	QButtonGroup* buttonGroup	= new QButtonGroup (this);
	buttonGroup->setExclusive (true);
	_saveEntitiesNamesCheckBox	=
			new QCheckBox (QString::fromUtf8("Enregistrer avec des noms pour les entités"));
	buttonGroup->addButton (_saveEntitiesNamesCheckBox);
	if (encodageScript == Internal::ContextIfc::WITHNAMES)
		_saveEntitiesNamesCheckBox->setCheckState (Qt::Checked);

	_saveEntitiesIdRefCheckBox =
			new QCheckBox (QString::fromUtf8("Enregistrer avec des références sur les commandes précédentes pour les entités"));
	buttonGroup->addButton (_saveEntitiesIdRefCheckBox);
	if (encodageScript == Internal::ContextIfc::WITHIDREF)
		_saveEntitiesIdRefCheckBox->setCheckState (Qt::Checked);

	_saveEntitiesCoordCheckBox =
			new QCheckBox (QString::fromUtf8("Enregistrer avec des coordonnées pour retrouver les entités"));
	buttonGroup->addButton (_saveEntitiesCoordCheckBox);
	if (encodageScript == Internal::ContextIfc::WITHCOORD)
		_saveEntitiesCoordCheckBox->setCheckState (Qt::Checked);

	mainLayout->addWidget (_saveEntitiesNamesCheckBox);
	mainLayout->addWidget (_saveEntitiesIdRefCheckBox);
	mainLayout->addWidget (_saveEntitiesCoordCheckBox);

	// Jeu de caractères pour l'encodage du script :
	UTF8String		text ("Jeu de caractères :", Charset::UTF_8);
	_charsetComboBox	= new QtLabelisedCharsetComboBox (extension, text, charset);
	int	index		= _charsetComboBox->getComboBox ( ).findData ((uint)Charset::UNKNOWN);
	if (-1 != index)
		_charsetComboBox->getComboBox ( ).removeItem (index);
	mainLayout->addWidget (_charsetComboBox);

	// On ajoute l'extension au sélecteur de fichier :
	QLayout*	dlgLayout	= layout ( );
	if (0 != dlgLayout)
	{
		QGridLayout*	gridLayout	= dynamic_cast<QGridLayout*>(dlgLayout);
		if (0 != gridLayout)
			gridLayout->addWidget (extension, gridLayout->rowCount( ), 0, 1, 2);
		else
			layout ( )->addWidget (extension);
	}	// if (0 != dlgLayout)
}	// QtMgx3DScriptFileDialog::createGui

}	// namespace QtComponents

}	// namespace Mgx3D
