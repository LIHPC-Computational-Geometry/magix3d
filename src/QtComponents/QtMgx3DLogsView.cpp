#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DLogsView.h"

#include <QtUtil/QtUnicodeHelper.h>

#include <assert.h>
#include <iostream>
#include <memory>

using namespace TkUtil;
using namespace std;

static const TkUtil::Charset	charset ("àéèùô");

USE_ENCODING_AUTODETECTION


namespace Mgx3D
{

namespace QtComponents
{

// ============================================================================
//                          LA CLASSE QtMgx3DLogsView
// ============================================================================

QtMgx3DLogsView::QtMgx3DLogsView (
		QWidget *parent, Log::TYPE mask, bool enableDate, bool enableTime)
	: QtLogsView (parent, mask, enableDate, enableTime)
{
}	// QtMgx3DLogsView::QtMgx3DLogsView


QtMgx3DLogsView::QtMgx3DLogsView (const QtMgx3DLogsView&)
	: QtLogsView (0, 0, false, false)
{
	assert (0 && "QtMgx3DLogsView copy constructor is forbidden.");
}	// QtMgx3DLogsView::QtMgx3DLogsView (const QtMgx3DLogsView&)


QtMgx3DLogsView& QtMgx3DLogsView::operator = (const QtMgx3DLogsView&)
{
	assert (0 && "QtMgx3DLogsView assignment operator is forbidden.");
	return *this;
}	// QtMgx3DLogsView::operator =


QtMgx3DLogsView::~QtMgx3DLogsView ( )
{
}	// QtMgx3DLogsView::~QtMgx3DLogsView


void QtMgx3DLogsView::writeSettings (QSettings& settings)
{
	settings.beginGroup ("LogsView");
	settings.setValue ("size", size ( ));
	settings.endGroup ( );
}	// QtMgx3DLogsView::writeSettings


void QtMgx3DLogsView::readSettings (QSettings& settings)
{
	settings.beginGroup ("LogsView");
	resize (settings.value ("size", size ( )).toSize ( ));
	settings.endGroup ( );
}	// QtMgx3DLogsView::readSettings


void QtMgx3DLogsView::log (const Log& log)
{
	AutoMutex	autoMutex (getMutex ( ));
	QtLogsView::log (log);
/*
const WChar_t*  uStr    = log.getText ( ).unicode ( );
const size_t	len	= log.getText ( ).length ( );
string	iso;
cout << "CONVERSION OF " << log.getText ( ).iso ( ) << " ..." << endl;
for (size_t i = 0; i < len; i++)
{
const unsigned char	firstByte	= uStr [i] >> 8;
	if (firstByte == 0)
{
cout << (char)uStr [i];
		iso += ((char)uStr [i]);
}
else cout << "|";
}   // for (size_t i = 0; i < l + 1; l++)
cout << endl;

	// Migration CentOS 7/Qt5.8.0 : problème d'encodage
	// des messages (UTF-8/ISO8859-1).
	unique_ptr<Log>	l (log.clone ( ));
//	l->setText (STDSTR (log.getText ( ).iso ( ).c_str ( )));
append (iso.c_str ( ));
append (QSTR (iso.c_str ( )));
append (QString::fromUtf16 (uStr, len));
//	QtLogsView::log (*l);
//	QtLogsView::log (log);
*/
}	// QtMgx3DLogsView::log

}	// namespace QtComponents

}	// namespace Mgx3D

