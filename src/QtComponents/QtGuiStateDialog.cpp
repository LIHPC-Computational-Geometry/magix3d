/**
 * \file        QtGuiStateDialog.cpp
 * \author      Charles PIGNEROL
 * \date        29/11/2013
 */

#include "Internal/ContextIfc.h"

#include "Utils/Common.h"
#include "QtComponents/QtGuiStateDialog.h"
#include <QtUtil/QtErrorManagement.h>

#include <TkUtil/InternalError.h>
#include <TkUtil/Process.h>
#include <TkUtil/UTF8String.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <QBoxLayout>
#include <QFileDialog>
#include <QLabel>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;


namespace Mgx3D
{

namespace QtComponents
{


// ===========================================================================
//                        LA CLASSE QtGuiStateDialog
// ===========================================================================

QtGuiStateDialog::QtGuiStateDialog (
			QWidget* parent, const string& appTitle, bool loadState,
			bool saveState, const string& fileName)
	: QDialog (parent), _loadStateCheckBox (0), _saveStateCheckBox (0),
	  _backupCopyCheckBox (0), _saveStateImmediatelyCheckBox (0),
	  _destroyStateCheckBox (0), _closurePanel(0), _safeguardFile (fileName),
	  _loadState (0), _saveState (0)
{
	createGui (parent, appTitle, loadState, saveState, fileName);
}	// QtGuiStateDialog::QtGuiStateDialog


QtGuiStateDialog::QtGuiStateDialog (
			QWidget* parent, const string& appTitle,
			Preferences::BoolNamedValue* loadState,
			Preferences::BoolNamedValue* saveState, const string& fileName)
	: QDialog (parent), _loadStateCheckBox (0), _saveStateCheckBox (0),
	  _backupCopyCheckBox (0), _saveStateImmediatelyCheckBox (0),
	  _destroyStateCheckBox (0), _closurePanel(0), _safeguardFile (fileName),
	  _loadState (loadState), _saveState (saveState)
{
	createGui (parent, appTitle,
	           0 == _loadState ? true : _loadState->getValue ( ),
	           0 == _saveState ? true : _saveState->getValue ( ), fileName);
}	// QtGuiStateDialog::QtGuiStateDialog


QtGuiStateDialog::QtGuiStateDialog (const QtGuiStateDialog&)
	: QDialog (0), _loadStateCheckBox (0), _saveStateCheckBox (0),
	  _backupCopyCheckBox (0), _saveStateImmediatelyCheckBox (0),
	  _destroyStateCheckBox (0), _closurePanel(0), _safeguardFile ("invalid file"),
	  _loadState (0), _saveState (0)
{
	MGX_FORBIDDEN ("QtGuiStateDialog copy constructor is not allowed.");
}	// QtGuiStateDialog::QtGuiStateDialog (const QtGuiStateDialog&)


QtGuiStateDialog& QtGuiStateDialog::operator = (const QtGuiStateDialog&)
{
	MGX_FORBIDDEN ("QtGuiStateDialog assignment operator is not allowed.");
	return *this;
}	// QtGuiStateDialog::QtGuiStateDialog (const QtGuiStateDialog&)


QtGuiStateDialog::~QtGuiStateDialog ( )
{
}	// QtGuiStateDialog::~QtGuiStateDialog


void QtGuiStateDialog::accept ( )
{
	try
	{

	if (0 != _loadState)
		_loadState->setValue (loadState ( ));
	if (0 != _saveState)
		_saveState->setValue (saveState ( ));

	if (true == createBackupCopy ( ))
		createSafeguard ( );

	if (true == immediatelyDestroyState ( ))
	{
		UTF8String	message (Charset::UTF_8);
		message << "Le fichier " << _safeguardFile.getFullFileName ( )
		         << " a été détruit.";
		_safeguardFile.remove ( );
		QtMessageBox::displayInformationMessage (
						this, windowTitle ( ).toStdString ( ), message);
	}	// if (true == immediatelyDestroyState ( ))

	}
	catch (const Exception& exc)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur :" << "\n"
		      << exc.getFullMessage ( );
		QtMessageBox::displayErrorMessage (
							this, windowTitle ( ).toStdString ( ), error);
	}
	catch (...)
	{
		QtMessageBox::displayErrorMessage (
			this, windowTitle ( ).toStdString ( ), "Erreur non documentée.");
	}

	QDialog::accept ( );
}	// QtGuiStateDialog::accept


bool QtGuiStateDialog::loadState ( ) const
{
	if (0 == _loadStateCheckBox)
	{
		INTERNAL_ERROR (exc, "QtGuiStateDialog::loadState", "Widget non instancié.")
		throw exc;
	}	// if (0 == _loadStateCheckBox)

	return Qt::Checked == _loadStateCheckBox->checkState ( ) ? true : false;
}	// QtGuiStateDialog::loadState


bool QtGuiStateDialog::saveState ( ) const
{
	if (0 == _saveStateCheckBox)
	{
		INTERNAL_ERROR (exc, "QtGuiStateDialog::saveState", "Widget non instancié.")
		throw exc;
	}	// if (0 == _saveStateCheckBox)

	return Qt::Checked == _saveStateCheckBox->checkState ( ) ? true : false;
}	// QtGuiStateDialog::saveState


bool QtGuiStateDialog::createBackupCopy ( ) const
{
	if (0 == _backupCopyCheckBox)
	{
		INTERNAL_ERROR (exc, "QtGuiStateDialog::createBackupCopy", "Widget non instancié.")
		throw exc;
	}	// if (0 == _backupCopyCheckBox)

	return Qt::Checked == _backupCopyCheckBox->checkState ( ) ? true : false;
}	// QtGuiStateDialog::createBackupCopy


bool QtGuiStateDialog::immediatelySaveState ( ) const
{
	if (0 == _saveStateImmediatelyCheckBox)
	{
		INTERNAL_ERROR (exc, "QtGuiStateDialog::immediatelySaveState", "Widget non instancié.")
		throw exc;
	}	// if (0 == _saveStateImmediatelyCheckBox)

	return Qt::Checked == _saveStateImmediatelyCheckBox->checkState ( ) ?
	       true : false;
}	// QtGuiStateDialog::immediatelySaveState


bool QtGuiStateDialog::immediatelyDestroyState ( ) const
{
	if (0 == _destroyStateCheckBox)
	{
		INTERNAL_ERROR (exc, "QtGuiStateDialog::immediatelyDestroyState", "Widget non instancié.")
		throw exc;
	}	// if (0 == _destroyStateCheckBox)

	return Qt::Checked == _destroyStateCheckBox->checkState ( ) ? true : false;
}	// QtGuiStateDialog::immediatelyDestroyState


void QtGuiStateDialog::createGui (
			QWidget* parent, const string& appTitle, bool loadState,
			bool saveState, const string& fileName)
{
	setModal (true);
	setWindowModality (Qt::WindowModal);
	setWindowTitle (appTitle.c_str ( ));

	QBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (QtConfiguration::margin);
	layout->setSizeConstraint (QLayout::SetMinimumSize);
	setLayout (layout);

	// Nom du fichier de sauvegarde de l'état de l'IHM :
	UTF8String	name (Charset::UTF_8);
	name << "Fichier de sauvegarde de l'état de l'IHM : " << fileName;
	QLabel*	label	= new QLabel (UTF8TOQSTRING (name), this);
	layout->addWidget (label);
	layout->addSpacing (20);

	// Restauration/sauvegarde automatique :
	_loadStateCheckBox	=
		new QCheckBox (QString::fromUtf8("Restaurer automatiquement l'état enregistré."), this);
	_loadStateCheckBox->setCheckState (
					true == loadState ? Qt::Checked : Qt::Unchecked);
	layout->addWidget (_loadStateCheckBox);
	_saveStateCheckBox	=
		new QCheckBox (QString::fromUtf8("Enregistrer automatiquement l'état de l'IHM à la fin."), this);
	_saveStateCheckBox->setCheckState (
					true == saveState ? Qt::Checked : Qt::Unchecked);
	connect (_saveStateCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (protectSafeguardFileCallback ( )));
	layout->addWidget (_saveStateCheckBox);
	layout->addSpacing (20);

	// Actions immédiates :
	_backupCopyCheckBox	=
		new QCheckBox (QString::fromUtf8("Créer une copie de sauvegarde de l'enregistrement actuel de l'état de l'IHM."));
	layout->addWidget (_backupCopyCheckBox);
	_saveStateImmediatelyCheckBox	=
		new QCheckBox (QString::fromUtf8("Enregistrer immédiatement l'état de l'IHM."), this);
	connect (_saveStateImmediatelyCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (protectSafeguardFileCallback ( )));
	layout->addWidget (_saveStateImmediatelyCheckBox);
	_destroyStateCheckBox	=
		new QCheckBox (QString::fromUtf8("Détruire immédiatement l'enregistrement de l'état de l'IHM."), this);
	connect (_destroyStateCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (protectSafeguardFileCallback ( )));
	layout->addWidget (_destroyStateCheckBox);
	layout->addSpacing (20);

	_closurePanel	= new QtDlgClosurePanel (this, false, "OK", "", "Annuler");
	layout->addWidget (_closurePanel);
	_closurePanel->setMinimumSize (_closurePanel->sizeHint ( ));
	connect (_closurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(accept ( )));
	connect (_closurePanel->getCancelButton ( ), SIGNAL(clicked ( )), this,
	         SLOT(reject ( )));
	_closurePanel->getApplyButton ( )->setAutoDefault (false);
	_closurePanel->getApplyButton ( )->setDefault (false);
	_closurePanel->getCancelButton ( )->setAutoDefault (false);
	_closurePanel->getCancelButton ( )->setDefault (false);
	layout->activate ( );

	protectSafeguardFileCallback ( );
}	// QtGuiStateDialog::createGui


void QtGuiStateDialog::protectSafeguardFileCallback ( )
{
	if ((true == saveState ( )) || (true == immediatelySaveState ( )) ||
	    (true == immediatelyDestroyState ( )))
	{
		if (0 == _backupCopyCheckBox)
		{
			INTERNAL_ERROR (exc, "QtGuiStateDialog::protectSafeguardFileCallback", "Widget non instancié.")
			throw exc;
		}	// if (0 == _backupCopyCheckBox)
		_backupCopyCheckBox->setChecked (Qt::Checked);
	}
}	// QtGuiStateDialog::protectSafeguardFileCallback


void QtGuiStateDialog::createSafeguard ( )
{
	UTF8String	backup (Charset::UTF_8);
	try
	{
		bool	ok	= false;
		backup << _safeguardFile.getFullFileName ( ) << ".backup";

		while (false == ok)
		{
			File	backupFile (backup);
			if (true == backupFile.exists ( ))
			{
				UTF8String	warning (Charset::UTF_8);
				warning << "Le fichier " << backupFile.getFullFileName ( )
				        << "\nexiste déjà. Souhaitez-vous l'écraser ?";
				switch (QMessageBox::warning (this, windowTitle ( ),
				        UTF8TOQSTRING (warning), "Oui", "Non", "Annuler", 0, 2))
				{
					case	0	: break;
					case	1	:
					{
						string	dir	= backupFile.getPath ( ).getFullFileName( );
						QFileDialog	dialog (
									this, windowTitle ( ), dir.c_str ( ));
						dialog.setFileMode (QFileDialog::AnyFile);
						dialog.setAcceptMode (QFileDialog::AcceptSave);
						dialog.setOption (QFileDialog::DontUseNativeDialog);
						if (QDialog::Accepted == dialog.exec ( ))
						{
							QStringList	fileList	= dialog.selectedFiles ( );
							if (0 != fileList.size ( ))
								backup	= fileList [0].toStdString ( );
						}	// if (QDialog::Accepted == dialog.exec ( ))

						continue;
					}
					case	2	: return;
				}	// switch (QMessageBox::warning (this, windowTitle ( ), ...
			}	// if (true == backupFile.exists ( ))
			else	// Le répertoire existe t'il ?
			{
				File	dir (backupFile.getPath ( ));
				if (false == dir.exists ( ))
					dir.create (false);
			}	// else if (true == backupFile.exists ( ))

			if (false == backupFile.isWritable ( ))
			{
				UTF8String	message (Charset::UTF_8);
				message << "Impossible de créer une copie de sauvegarde, vous "
				        << "n'avez pas les droits en écriture sur le fichier "
				        << "\n"
				        << backupFile.getFullFileName ( );
				QtMessageBox::displayErrorMessage (
								this, windowTitle ( ).toStdString ( ), message);

				string	dir	= backupFile.getPath ( ).getFullFileName( );
				QFileDialog	dialog (this, windowTitle ( ), dir.c_str ( ));
				dialog.setFileMode (QFileDialog::AnyFile);
				dialog.setAcceptMode (QFileDialog::AcceptSave);
				dialog.setOption (QFileDialog::DontUseNativeDialog);
				if (QDialog::Accepted == dialog.exec ( ))
				{
					QStringList	fileList	= dialog.selectedFiles ( );
					if (0 != fileList.size ( ))
						backup	= fileList [0].toStdString ( );
					continue;
				}	// if (QDialog::Accepted == dialog.exec ( ))
			}	// if (false == backupFile.isWritable ( ))

			ok	= true;
		}	// while (false == ok)

		unique_ptr<Process>	copier (new Process ("cp"));
		copier->getOptions ( ).addOption (_safeguardFile.getFullFileName ( ));
		copier->getOptions ( ).addOption (backup);
		copier->execute (false);
		int	status	= copier->wait ( );
		if (0 != status)
			throw Exception (copier->getErrorMessage ( ));
	}
	catch (const TkUtil::Exception& exc)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur lors de la sauvegarde du fichier d'enregistrement de "
		      << "l'état de l'IHM " << "\n"
		      << _safeguardFile.getFullFileName ( )
		      << " dans le fichier " << "\n"
		      << backup << " :" << "\n" 
		      << exc.getFullMessage ( );
		QtMessageBox::displayErrorMessage (
								this, windowTitle ( ).toStdString ( ), error);
	}
	catch (...)
	{
		UTF8String	error (Charset::UTF_8);
		error << "Erreur non documentée lors de la sauvegarde du fichier "
		      << "d'enregistrement de l'état de l'IHM "
		      << "\n"
		      << _safeguardFile.getFullFileName ( )
		      << " dans le fichier " << "\n"
		      << backup;
		QtMessageBox::displayErrorMessage (
								this, windowTitle ( ).toStdString ( ), error);
	}
}	// QtGuiStateDialog::createSafeguard


}	// namespace QtComponents

}	// namespace Mgx3D
