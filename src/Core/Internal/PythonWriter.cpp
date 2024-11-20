/*----------------------------------------------------------------------------*/
/*
 * \file PythonWriter.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 21 janv. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Utils/Common.h"
#include "Internal/PythonWriter.h"

#include <TkUtil/InformationLog.h>
#include <TkUtil/UTF8String.h>

#include <IQualif.h>
#include <Lima/malipp.h>

#include <Standard_Version.hxx>


/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
PythonWriter::
PythonWriter (const TkUtil::UTF8String& fileName,
        const TkUtil::Charset& charset,
        TkUtil::Log::TYPE mask,
        bool enableDate, bool enableTime)
: TkUtil::PythonLogOutputStream (fileName, charset, mask, enableDate, enableTime)
{
	setShell(PYTHON_INTERPRETER);
}
/*----------------------------------------------------------------------------*/
PythonWriter::
PythonWriter (const PythonWriter&)
: TkUtil::PythonLogOutputStream ("invalid file name", 0)
{
    MGX_FORBIDDEN("PythonWriter: constructeur par copie interdit");
}
/*----------------------------------------------------------------------------*/
PythonWriter& PythonWriter::
operator = (const PythonWriter&)
{
    MGX_FORBIDDEN("PythonWriter::operator = : constructeur par copie interdit");
    return *this;
}
/*----------------------------------------------------------------------------*/
PythonWriter::
~PythonWriter ( )
{
}
/*----------------------------------------------------------------------------*/
void PythonWriter::
writeScriptComments  ()
{
	PythonLogOutputStream::writeScriptComments ( );
	TkUtil::UTF8String	comment (TkUtil::Charset::UTF_8);

	comment << "Version de Qualif      : " << Qualif::QualifVersion ( );
	logComment (TkUtil::InformationLog (comment));
	comment.clear ( );

	/*	comment << "Version de Lima++      : " << Lima::lima_version ( );
	logComment (TkUtil::InformationLog (comment));
	comment.clear ( );

	comment << "Version de HDF 5       : "
	<< Lima::MaliPPReader::lireVersionHDF ( ).getVersion ( );*/
	logComment (TkUtil::InformationLog (comment));
    comment.clear ( );

    comment << "Version de OCC         : "
            	<< OCC_VERSION_COMPLETE;


    logComment (TkUtil::InformationLog (comment));
    comment.clear ( );

}
/*----------------------------------------------------------------------------*/
void PythonWriter::
addComment(const TkUtil::UTF8String& cmt)
{
    write(formatComment(TkUtil::InformationLog(cmt)));
}
/*----------------------------------------------------------------------------*/
void PythonWriter::
addCommand(const TkUtil::UTF8String& cmd)
{
    write (cmd);
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
