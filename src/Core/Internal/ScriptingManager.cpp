/*----------------------------------------------------------------------------*/
/*
 * \file ScriptingManager.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24 janv. 2011
 */
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Internal/ScriptingManager.h"
#include "Internal/PythonWriter.h"
#include "Utils/Common.h"

#include "TkUtil/Exception.h"
#include "TkUtil/File.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/ErrorLog.h>
#include <TkUtil/TraceLog.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
ScriptingManager::
ScriptingManager (Internal::Context* c)
: m_python_writer(0), m_context(c), m_charset (TkUtil::Charset (TkUtil::Charset::UTF_8))
{
}
/*----------------------------------------------------------------------------*/
const TkUtil::Charset& ScriptingManager::getCharset ( ) const
{
	return m_charset;
}
/*----------------------------------------------------------------------------*/
void ScriptingManager::setCharset (const TkUtil::Charset& charset)
{
	m_charset	= charset;
}
/*----------------------------------------------------------------------------*/
void ScriptingManager::
initPython(const TkUtil::UTF8String& fileName, bool withHeader)
{
    if (m_python_writer)
        throw TkUtil::Exception(TkUtil::UTF8String("ScriptingManager::initPython déjà effectué", TkUtil::Charset::UTF_8));

	TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
    message << "Utilisation du fichier Python "<<fileName <<" pour l'archivage des commandes en Python";
    m_context->getLogStream()->log(TkUtil::TraceLog (message, TkUtil::Log::INFORMATION));

	TkUtil::File	file (fileName);
	if (false == file.exists ( ))
	{	// On le créé => créé son répertoire si nécessaire.
		try
		{
			file.create (true);
		}
		catch (const TkUtil::Exception& exc)
		{
			TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);
			error << "Impossibilité de créer le fichier Python " << fileName
			      << " pour l'archivage des commandes en Python.";
			m_context->getLogStream()->log(TkUtil::ErrorLog (error));
		}
	}	// if (false == file.exists ( ))

    m_python_writer = new Internal::PythonWriter(fileName, m_charset);
	if (false == m_python_writer->isEnabled ( ))
	{
		TkUtil::UTF8String	error (TkUtil::Charset::UTF_8);
		error << "Impossibilité d'écrire le fichier Python " << fileName
		      << " pour l'archivage des commandes en Python.";
		m_context->getLogStream()->log(TkUtil::ErrorLog (error));
	}	// if (false == m_python_writer->isEnabled ( ))

	if (withHeader){
		// init pour script python
		m_python_writer->writeHeader(); // pas d'info pour Process::getCurrentSoftware()
		m_python_writer->addBlankLine();

		//    m_python_writer->declareTryBlock();

		char*           env = getenv ("MGX3D_PATH");
		if (0 != env) {

			m_python_writer->addCommand(IN_UTIL UTF8String ("# Vous avez la possibilité ici de sélectionner la version de Magix3D", IN_UTIL Charset::UTF_8));

	        const std::string    mgx3DPath (0 == env ? "" : env);
	        IN_UTIL UTF8String   path1 (TkUtil::Charset::UTF_8);
	        path1 << "# sys.path.append(\"" << mgx3DPath << "\")";
	        m_python_writer->addCommand(path1);


			TkUtil::UTF8String   importMaillage (TkUtil::Charset::UTF_8);
			importMaillage << "# import maillage";
			m_python_writer->addCommand(importMaillage);

			m_python_writer->addCommand("");

			TkUtil::UTF8String   importLine (TkUtil::Charset::UTF_8);
			importLine << "import pyMagix3D as " << getMgx3DAlias();
			m_python_writer->addCommand(importLine);

			TkUtil::UTF8String   contextLine (TkUtil::Charset::UTF_8);
			//contextLine << getContextAlias() << " = " << getMgx3DAlias() <<".getContext(\""<< getContext().getName()<< "\")";
			contextLine << getContextAlias() << " = " << getMgx3DAlias() <<".getStdContext()";
			m_python_writer->addCommand(contextLine);

			m_python_writer->addCommand(""); // une ligne vide
		}
		else {
			std::cerr <<"MGX3D_PATH n'est pas définit\n";
		}
	} // end if (withHeader)
}
/*----------------------------------------------------------------------------*/
ScriptingManager::
ScriptingManager (const ScriptingManager&)
	: m_python_writer (0), m_context (0), m_charset (TkUtil::Charset::UNKNOWN)
{
    MGX_FORBIDDEN("ScriptingManager: constructeur par copie interdit");
}
/*----------------------------------------------------------------------------*/
ScriptingManager& ScriptingManager::
operator = (const ScriptingManager&)
{
    MGX_FORBIDDEN("ScriptingManager::operator = : constructeur par copie interdit");
    return *this;
}
/*----------------------------------------------------------------------------*/
ScriptingManager::
~ScriptingManager ( )
{
    if (m_python_writer){
    //    m_python_writer->closeTryBlock();
     delete m_python_writer;
    }
}
/*----------------------------------------------------------------------------*/
void ScriptingManager::setContext (Internal::Context* context)
{
    m_context   = context;
}
/*----------------------------------------------------------------------------*/
PythonWriter* ScriptingManager::
getPythonWriter ( )
{
	return m_python_writer;
}	// ScriptingManager::getPythonWriter
/*----------------------------------------------------------------------------*/
void ScriptingManager::
addComment(const TkUtil::UTF8String& cmt)
{
    if (m_python_writer)
        m_python_writer->addComment(cmt);
}
/*----------------------------------------------------------------------------*/
void ScriptingManager::
addCommand(const TkUtil::UTF8String& cmd)
{
    if (m_python_writer){
        TkUtil::UTF8String cmd2 = replaceInternalCommand(cmd);
        if (cmd != cmd2){
            TkUtil::UTF8String cmd3 (TkUtil::Charset::UTF_8);
            cmd3 << "# Commande tappée: " <<cmd;
            m_python_writer->addCommand(cmd3);
        }
        m_python_writer->addCommand(cmd2);
    }
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String ScriptingManager::replaceInternalCommand(const TkUtil::UTF8String& cmd)
{
    if (cmd.empty())
        return cmd;

    // substitue l'appel à la fonction getSelectedEntities() par son contenu
    size_t pos = cmd.find("getSelectedEntities");
    if ((size_t)-1 != pos) {

        // recherche de "ctx"
        size_t posDeb = cmd.rfind("ctx.",pos);
        if ((size_t)-1 == posDeb)
            throw TkUtil::Exception(TkUtil::UTF8String("Echec lors de la recherche de \"ctx.\" dans la ligne de commande", TkUtil::Charset::UTF_8));

        // recherche de ")"
        size_t posFin = cmd.find(")",pos);
        if ((size_t)-1 == posFin)
             throw TkUtil::Exception(TkUtil::UTF8String("Echec lors de la recherche de \")\" dans la ligne de commande", TkUtil::Charset::UTF_8));

        // récupération de ce qui est avant "ctx" et après "ctx.getSelectedEntities ( )"
        TkUtil::UTF8String cmd2 (TkUtil::Charset::UTF_8);

        cmd2 << cmd.substring(0, posDeb-1);

        // utilisation des noms à la place de la méthode
        std::vector<std::string> vs = getContext().getSelectionManager().getEntitiesNames();

        if (vs.empty())
            cmd2 = "[]";
        else {
            cmd2 << "[\""<<vs[0];
            for (uint i=1; i<vs.size(); i++)
                cmd2 << "\",\"" << vs[i];
            cmd2 << "\"]";
        }

        cmd2 << cmd.substring(posFin+1);

        return cmd2;
    }
    else
        return cmd;
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
