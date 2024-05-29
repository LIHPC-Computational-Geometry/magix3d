#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DPreferencesDialog.h"
#include "QtComponents/QtMgx3DApplication.h"
#include <QtUtil/QtHelpWindow.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>

#include <assert.h>


using namespace std;
using namespace TkUtil;
using namespace Preferences;


namespace Mgx3D
{

namespace QtComponents
{


QtMgx3DPreferencesDialog::QtMgx3DPreferencesDialog (
		QWidget* parent, Section* section, const string& fromFile, 
		bool editable, bool withApply, bool modal,
		const UTF8String& dlgTitle, const UTF8String& appTitle, const string& helpURL,
		const string& helpTag)
	: QtPreferencesDialog (parent, section, fromFile,
	                       editable, withApply, modal, dlgTitle, appTitle),
	_helpURL (helpURL), _helpTag (helpTag)
{
	// Creation du menu "Aide" :
	if ((0 != _helpURL.length ( )) && (0 != getHelpMenu ( )))
	{
#ifdef QT_3
		getHelpMenu ( )->insertItem ("A&ide", this, SLOT (helpCallback ( )));
#else	// QT_3
		getHelpMenu ( )->addAction ("A&ide", this, SLOT (helpCallback ( )));
#endif	// QT_3
	}	// if ((0 != _helpURL.length ( )) && (0 != getHelpMenu ( )))
}	// QtMgx3DPreferencesDialog::QtMgx3DPreferencesDialog


QtMgx3DPreferencesDialog::QtMgx3DPreferencesDialog (
											const QtMgx3DPreferencesDialog&)
	: QtPreferencesDialog (0, 0, "", false, false, false, "", "")
{
	assert (0 && "QtMgx3DPreferencesDialog copy constructor is not allowed.");
}	// QtMgx3DPreferencesDialog::QtMgx3DPreferencesDialog (const QtMgx3DPreferencesDialog&)


QtMgx3DPreferencesDialog& QtMgx3DPreferencesDialog::operator =(const QtMgx3DPreferencesDialog&)
{
	assert (0 && "QtMgx3DPreferencesDialog::operator = is not allowed.");
	return *this;
}	// QtMgx3DPreferencesDialog::operator =


QtMgx3DPreferencesDialog::~QtMgx3DPreferencesDialog ( )
{
}	// QtMgx3DPreferencesDialog::~QtMgx3DPreferencesDialog


void QtMgx3DPreferencesDialog::applyCallback ( )
{
	try
	{
		QtMgx3DApplication::instance ( ).applyConfiguration (getSection ( ));
	}
	catch (Exception& exc)
	{
		UTF8String	errorMsg (Charset::UTF_8);
		errorMsg << "Impossibilité d'appliquer la configuration :\n"
		         << exc.getMessage ( );
		displayErrorMessage (errorMsg.iso ( ));
	}
	catch (...)
	{
		displayErrorMessage ("Impossibilité d'appliquer la configuration : erreur non documentée.");
	}
}	// QtMgx3DPreferencesDialog::applyCallback


void QtMgx3DPreferencesDialog::helpCallback ( )
{
	QtHelpWindow::displayURL (_helpURL, _helpTag);
}	// QtMgx3DPreferencesDialog::helpCallback


}	// namespace Mgx3D

}	// namespace QtComponents

