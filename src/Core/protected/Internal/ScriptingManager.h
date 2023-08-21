/*----------------------------------------------------------------------------*/
/*
 * \file ScriptingManager.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24 janv. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef SCRIPTINGMANAGER_H_
#define SCRIPTINGMANAGER_H_

#include <TkUtil/UTF8String.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {

class PythonWriter;
class Context;

/*----------------------------------------------------------------------------*/
/** Cette classe gère l'utilisation d'écrivains du type PythonWriter
 *
 */
class ScriptingManager
{
public :

    /**
     * Constructeur.
     */
    ScriptingManager (Internal::Context* c);

    /**
     * Destructeur.
     */
    virtual ~ScriptingManager ( );

    /** Initialise le manager pour les écritures des scripts en Python
     *
     * Si cette initialisation n'est pas faites, les fonctions ne font rien
     */
    virtual void initPython(const TkUtil::UTF8String& fileName, bool withHeader=true);

	/**
	 *\return Un pointeur sur l'écrivain en langage <I>Python</I>.
	 */
	virtual PythonWriter* getPythonWriter ( );

    /** Ajoute au script un commentaire (ligne sans retour chariot)
     */
    virtual void addComment(const TkUtil::UTF8String& cmt);

    /** Ajoute au script une commande (ligne sans retour chariot)
     */
    virtual void addCommand(const TkUtil::UTF8String& cmd);

    /** Accesseur sur le context
     */
    virtual Internal::Context& getContext() {return *m_context;}

    /**
     * \param       Nouveau context.
     */
    virtual void setContext (Internal::Context* context);

    /**
     * \return      Le jeu de caractères utilisé pour l'encodage des scripts.
     */
    const virtual TkUtil::Charset& getCharset ( ) const;

    /**
     * \param      Le jeu de caractères à utiliser pour l'encodage des scripts.
     */
    virtual void setCharset (const TkUtil::Charset& charset);


protected:

    /** fonction qui remplace certaines commandes par le résultat pour être rejouable dans un script
     */
    TkUtil::UTF8String replaceInternalCommand(const TkUtil::UTF8String& cmd);

private :
    /**
        * Constructeur de copie et opérateur =. Interdits.
        */
    ScriptingManager (const ScriptingManager&);
    ScriptingManager& operator = (const ScriptingManager&);

    Internal::PythonWriter* m_python_writer;

    Internal::Context* m_context;
    /** Le jeu de caractères utilisé pour l'encodage des scripts. */
    TkUtil::Charset m_charset;
};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* SCRIPTINGMANAGER_H_ */
