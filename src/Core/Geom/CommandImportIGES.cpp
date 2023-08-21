/*
 * CommandImportIGES.cpp
 *
 *  Created on: 21 juin 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Common.h"

/*----------------------------------------------------------------------------*/
#include "Geom/CommandImportIGES.h"
#include "Geom/GeomManager.h"
#include "Geom/ImportIGESImplementation.h"
#include "Geom/Curve.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/ReferencedMutex.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/File.h>
#include <TkUtil/NumericConversions.h>
#include <fstream>
#include <sstream>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {

class GeomVertex;
//class Curve;
class Surface;
class Volume;
/*----------------------------------------------------------------------------*/
CommandImportIGES::
CommandImportIGES(Internal::Context& c, const std::string& n, const bool splitCompoundCurves)
: CommandCreateGeom(c, "Import IGES"), m_filename(n)
{
    m_impl = new ImportIGESImplementation(c, &getInfoCommand(), m_filename);
    m_impl->setSplitCompoundCurves(splitCompoundCurves);
}
/*----------------------------------------------------------------------------*/
CommandImportIGES::~CommandImportIGES()
{
    if(m_impl)
        delete m_impl;
}
/*----------------------------------------------------------------------------*/
void CommandImportIGES::
internalExecute()
{
    m_impl->perform(m_createdEntities);
#ifdef _DEBUG
    std::cout<<"NB CREATED = "<<m_createdEntities.size()<<std::endl;
#endif
    //stockage dans le manager géométrique des entités créés (donc nouvelles
    //uniquement)
    store(m_createdEntities);

    //gestion de l'appartenance aux groupes
    for(unsigned int i=0;i<m_createdEntities.size();i++)
        addToGroup(m_createdEntities[i]);
}
/*----------------------------------------------------------------------------*/
IGESHeader::IGESHeader ( )
: parameterDelimiter (","), recordDelimiter (";"), productIdentification ( ), fileName ( ), nativeSystemId ( ), preprocessorVersion ( ),
intBitNum (-1), floatPowerMax (0), floatTenPowerMax (0), doublePowerMax (0), doubleTenPowerMax (0),
receivingSystemID ( ), modelSpaceScale (0.), unitsFlag (-1), unitsName ( ), weightGradationsMaxLines (-1), weightMaxWidth (0.),
fileDateCreation ( ), resolutionMin (0.), maxCoordinateValue (0.), authorName ( ), authorOrganization ( ),
specsVersion ( ), draftVersion ( ), fileDateModification ( ), applicationData ( )
{
}   // IGESHeader::IGESHeader
/*----------------------------------------------------------------------------*/
IGESHeader::IGESHeader (const IGESHeader& header)
: parameterDelimiter (header.parameterDelimiter), recordDelimiter (header.recordDelimiter), productIdentification (header.productIdentification),
fileName (header.fileName), nativeSystemId (header.nativeSystemId), preprocessorVersion (header.preprocessorVersion),
intBitNum (header.intBitNum), floatPowerMax (header.floatPowerMax), floatTenPowerMax (header.floatTenPowerMax),
doublePowerMax (header.doublePowerMax), doubleTenPowerMax (header.doubleTenPowerMax),
receivingSystemID (header.receivingSystemID), modelSpaceScale (header.modelSpaceScale), unitsFlag (header.unitsFlag), unitsName (header.unitsName),
weightGradationsMaxLines (header.weightGradationsMaxLines), weightMaxWidth (header.weightMaxWidth),
fileDateCreation (header.fileDateCreation), resolutionMin (header.resolutionMin), maxCoordinateValue (header.maxCoordinateValue),
authorName (header.authorName), authorOrganization (header.authorOrganization),
specsVersion (header.specsVersion), draftVersion (header.draftVersion), fileDateModification (header.fileDateModification), applicationData (header.applicationData)
{
}   // IGESHeader::IGESHeader
/*----------------------------------------------------------------------------*/
IGESHeader& IGESHeader::operator = (const IGESHeader& header)
{
	if (&header != this)
	{
		parameterDelimiter      = header.parameterDelimiter;
		recordDelimiter         = header.recordDelimiter;
		productIdentification   = header.productIdentification;
		fileName                = header.fileName;
		nativeSystemId          = header.nativeSystemId;
		preprocessorVersion     = header.preprocessorVersion;
		intBitNum               = header.intBitNum;
		floatPowerMax           = header.floatPowerMax;
		floatTenPowerMax        = header.floatTenPowerMax;
		doublePowerMax          = header.doublePowerMax;
		doubleTenPowerMax       = header.doubleTenPowerMax;
		receivingSystemID       = header.receivingSystemID;
		modelSpaceScale         = header.modelSpaceScale;
		unitsFlag               = header.unitsFlag;
		unitsName               = header.unitsName;
		weightGradationsMaxLines= header.weightGradationsMaxLines;
		weightMaxWidth          = header.weightMaxWidth;
		fileDateCreation        = header.fileDateCreation;
		resolutionMin           = header.resolutionMin;
		maxCoordinateValue      = header.maxCoordinateValue;
		authorName              = header.authorName;
		authorOrganization      = header.authorOrganization;
		specsVersion            = header.specsVersion;
		draftVersion            = header.draftVersion;
		fileDateModification    = header.fileDateModification;
		applicationData         = header.applicationData;
	}   // if (&header != this)
	
	return *this;
}   // IGESHeader::operator =
/*----------------------------------------------------------------------------*/
static const size_t sectionPos  = 72;

static char lineType (const std::string& line)
{
	if (line.length ( ) != 80)
	{
		TkUtil::UTF8String  error;
		error << "La ligne \"" << line << "\" ne fait pas 80 caractères (" << line.length ( ) << ").";
		throw TkUtil::Exception (error);
	}   // if (line.length ( ) != 80)

	switch ((char)line [sectionPos])
	{
		case    'S' :
		case    'G' :
		case    'D' :
		case    'P' :
		case    'T' :
			break;
		case    'B' :   // cf. Fig H6 of iges5-3_fordownload.pdf
		{
			TkUtil::UTF8String  error;
			error << "Format binaire non supporté.";
			throw TkUtil::Exception (error);
		}
		case    'C' :
		{
			TkUtil::UTF8String  error;
			error << "Format compressé non supporté.";
			throw TkUtil::Exception (error);
		}
	}   // switch (line [sectionPos])

	return line [sectionPos];
}   // lineType

static std::string getParameter (std::string& stream, const std::string& delimiter, const std::string& defaultValue, bool allowEmpty = false)
{
	std::string parameter;

	if (0 == stream.size ( ))
	{
		if (false == allowEmpty)
			throw TkUtil::Exception(TkUtil::UTF8String("getParameter : chaîne de caractères vide.", TkUtil::Charset::UTF_8));
		else
			return defaultValue;
	}   // if (0 == stream.size ( ))

	std::string::size_type   pos = stream.find (delimiter);
	if (std::string::npos == pos)
	{
		parameter = stream;
		stream.clear ( );
	}   // if (std::string::npos == pos)
	else
	{
		parameter   = stream.substr (0, pos);
		stream.erase (0, pos + 1);
	}   // else if (std::string::npos == pos)
	if (true == parameter.empty ( ))
		parameter   = defaultValue;

	return parameter;
}   // getParameter

static std::string getParameterDelimiter (const IGESHeader& header, std::string& stream)
{   // First parameter delimiter is mandatory ',' so we can have "1H,," if it is kept ...
	if (0 == stream.find ("1H,,"))
	{
		stream.erase (0, 4);
		return std::string (",");
	}   // if (0 == stream.find ("1H,,"))

	return getParameter (stream, header.parameterDelimiter, header.parameterDelimiter);
}   // getParameterDelimiter

static std::string stringParameter (const std::string& stringParameter)
{
	if (0 == stringParameter.length ( ))
		return stringParameter;

	std::istringstream   stream (stringParameter);
	unsigned long   count   = 0;
	char            h;
	stream >> count >> h;
	if ((true == stream.fail ( )) || ('H' != h))
	{
		TkUtil::UTF8String  error;
		error << "Paramètre de type chaîne de caractères \"" << stringParameter << "\" invalide. Format attendu : nbcharHchaine";
		throw TkUtil::Exception (error);
	}   // if ((true == stream.fail ( )) || ('H' != h))

	std::string str = stream.str ( ).substr (stream.tellg ( ));
	if (str.length ( ) != count)
	{
		TkUtil::UTF8String  error;
		error << "Longueur de la chaîne de caractères \"" << stringParameter << "\" invalide. Format attendu : nbcharHchaine";
		throw TkUtil::Exception (error);
	}   // if (str.length ( ) != count)

	return str;
}   // stringParameter

static long integerParameter (const std::string& parameter)
{
	try
	{
		return TkUtil::NumericConversions::strToLong (parameter);
	}
	catch (const TkUtil::Exception& exc)
	{
		TkUtil::UTF8String  error;
		error << "Paramètre de type entier \"" << parameter << "\" invalide :\n" << exc.getFullMessage ( );
		throw TkUtil::Exception (error);
	}
	catch (...)
	{
		TkUtil::UTF8String  error;
		error << "Paramètre de type entier \"" << parameter << "\" invalide.";
		throw TkUtil::Exception (error);
	}
}   // integerParameter

static double doubleParameter (const std::string& parameter)
{
	try
	{
		return TkUtil::NumericConversions::strToDouble (parameter);
	}
	catch (const TkUtil::Exception& exc)
	{
		TkUtil::UTF8String  error;
		error << "Paramètre de type double \"" << parameter << "\" invalide :\n" << exc.getFullMessage ( );
		throw TkUtil::Exception (error);
	}
	catch (...)
	{
		TkUtil::UTF8String  error;
		error << "Paramètre de type double \"" << parameter << "\" invalide.";
		throw TkUtil::Exception (error);
	}
}   // doubleParameter

void IGESHeader::read (const std::string& filepath, TkUtil::UTF8String& messages, bool verbose)
{
	TkUtil::File        igesFile (filepath);
	TkUtil::UTF8String  msg (TkUtil::Charset::UTF_8);
	std::string         endofline;  // if necessary

	try
	{
		if ((false == igesFile.exists()) || (false == igesFile.isReadable()))
			throw TkUtil::Exception(TkUtil::UTF8String("Ce fichier n'existe pas ou vous n'avez pas les droits en lecture dessus.", TkUtil::Charset::UTF_8));

		char                         buffer[1024];
		char                         globalSection[102400];
		size_t                       gspos              = 0;
		ifstream                     ifs(igesFile.getFullFileName());
		bool                         completed          = false;
		bool                         msdos              = false;
		memset(globalSection, '\0', 102400);
		while ((false == completed) && ((true == ifs.good()) && (false == ifs.eof())))
		{
			memset(buffer, '\0', 1023);
			ifs.getline(buffer, 1023);
			const size_t length = strlen(buffer);
			if ((length >= 1) && ('\r' == buffer[length - 1]))
			{
				if ((true == verbose) && (false == msdos))
				{
					msg << endofline << "Traitement d'un fichier MS-DOS.";
					endofline   = '\n';
					msdos = true;
				}   // if ((true == verbose) && (false == msdos))
				buffer[length - 1] = '\0';
			}   // if ((length >= 1) && ('\r' == buffer [length - 1]))
			const char linetype = lineType(buffer);
			const char start    = 'S';
			switch (linetype)
			{
				case 'S'    :
					if (true == verbose)
						cout << endofline << "Saut de la ligne \"" << buffer << "\" de la section \"Start\".";
					break;    // Start section
				case 'G'    :
					if (true == verbose)
						cout << endofline << "Processing line \"" << buffer << "\" de la section \"Global\".";
					memcpy(globalSection + gspos, buffer, sectionPos);
					gspos += sectionPos;
					break;
				case 'D'    :
					if (true == verbose)
						cout << endofline << "Saut de la ligne \"" << buffer << "\" de la section \"Directory entry\".";
					completed = true;   // just reading header == start + global
					break;
				case 'P'    :
					if (true == verbose)
						cout << endofline << "Saut de la ligne \"" << buffer << "\" de la section \"Parameter data\".";
					completed = true;   // just reading header == start + global
					break;
				case 'T'    :
					if (true == verbose)
						cout << endofline << "Saut de la ligne \"" << buffer << "\" de la section \"Terminate\".";
					completed = true;   // just reading header == start + global
					break;
				default     :
					if (true == verbose)
						cout << endofline << "Saut de la ligne \"" << buffer << "\" de la section de type inconnu \"" << linetype
						     << "\".";
					completed = true;
			}   // switch (linetype)
		}   // while ((false == completed) && ((true == ifs.good ( )) && (false == ifs.eof ( ))))
		const std::string::size_type recordDelimiterPos = std::string(globalSection).rfind(recordDelimiter);
		if (std::string::npos != recordDelimiterPos)
			globalSection[recordDelimiterPos] = '\0';

		// Process global section :
		std::string gsStream(globalSection);
		parameterDelimiter    = getParameterDelimiter(*this, gsStream);
		recordDelimiter       = stringParameter(getParameter(gsStream, parameterDelimiter, "1H;"));
		productIdentification = stringParameter(getParameter(gsStream, parameterDelimiter, ""));
		fileName              = stringParameter(getParameter(gsStream, parameterDelimiter, ""));
		nativeSystemId        = stringParameter(getParameter(gsStream, parameterDelimiter, ""));
		preprocessorVersion   = stringParameter(getParameter(gsStream, parameterDelimiter, ""));
		intBitNum             = integerParameter(getParameter(gsStream, parameterDelimiter, ""));
		floatPowerMax         = integerParameter(getParameter(gsStream, parameterDelimiter, ""));
		floatTenPowerMax      = integerParameter(getParameter(gsStream, parameterDelimiter, ""));
		doublePowerMax        = integerParameter(getParameter(gsStream, parameterDelimiter, ""));
		doubleTenPowerMax     = integerParameter(getParameter(gsStream, parameterDelimiter, ""));
		receivingSystemID     = stringParameter(getParameter(gsStream, parameterDelimiter, ""));
		modelSpaceScale       = doubleParameter(getParameter(gsStream, parameterDelimiter, ""));
		unitsFlag             = integerParameter(getParameter(gsStream, parameterDelimiter, ""));
		unitsName             = stringParameter(getParameter(gsStream, parameterDelimiter, ""));
		weightGradationsMaxLines = integerParameter(getParameter(gsStream, parameterDelimiter, ""));
		weightMaxWidth       = doubleParameter(getParameter(gsStream, parameterDelimiter, ""));
		fileDateCreation     = stringParameter(getParameter(gsStream, parameterDelimiter, ""));
		resolutionMin        = doubleParameter(getParameter(gsStream, parameterDelimiter, ""));
		maxCoordinateValue   = doubleParameter(getParameter(gsStream, parameterDelimiter, ""));
		authorName           = stringParameter(getParameter(gsStream, parameterDelimiter, ""));
		authorOrganization   = stringParameter(getParameter(gsStream, parameterDelimiter, ""));
		specsVersion         = integerParameter(getParameter(gsStream, parameterDelimiter, ""));
		draftVersion         = integerParameter(getParameter(gsStream, parameterDelimiter, "", true));
		fileDateModification = stringParameter(getParameter(gsStream, parameterDelimiter, ""));
		applicationData      = stringParameter(getParameter(gsStream, parameterDelimiter, "", true));
	}
	catch (const TkUtil::Exception& exc)
	{
		if (true == verbose)
			messages    += msg;
		TkUtil::UTF8String  error (TkUtil::Charset::UTF_8);
		error << "Erreur de lecture du fichier IGES " << filepath;
		throw TkUtil::Exception (error, exc);
	}
	catch (...)
	{
		if (true == verbose)
			messages    += msg;
		TkUtil::UTF8String  error (TkUtil::Charset::UTF_8);
		error << "Erreur de lecture du fichier IGES " << filepath << " :\nErreur non renseignée.";
		throw TkUtil::Exception (error);
	}

	if (true == verbose)
		messages    += msg;
}   // IGESHeader::read
/*----------------------------------------------------------------------------*/
Mgx3D::Utils::Unit::lengthUnit IGESHeader::getMgxUnit (bool couldRaises) const
{
	try
	{
		return Utils::Unit::toLengthUnit (unitsName);
	}
	catch (...)
	{
		if (true == couldRaises)
			throw;
	}

	return Utils::Unit::undefined;
}   // IGESHeader::getMgxUnit
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
