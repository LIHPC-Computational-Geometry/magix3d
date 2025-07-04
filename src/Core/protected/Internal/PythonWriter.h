/*----------------------------------------------------------------------------*/
/*
 * \file PythonWriter.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 21 janv. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_PYTHONWRITER_H_
#define MGX3D_PYTHONWRITER_H_

#include <PythonUtil/PythonLogOutputStream.h>
#include <TkUtil/UTF8String.h>
#include <iostream>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace MpiMagix3D {
class MpiMagix3DBatchRunner;
}
/*----------------------------------------------------------------------------*/
namespace Internal {

class ScriptingManager;

/*----------------------------------------------------------------------------*/
/** Cette classe gère la sortie dans un fichier des commandes Python
 *  pour un contexte donné
 *
 */
class PythonWriter : public TkUtil::PythonLogOutputStream
{
friend class ScriptingManager;
friend class MpiMagix3D::MpiMagix3DBatchRunner;

public :

    /**
     * Constructeur. Déclare les paquetages à utiliser.
     * @param       Nom du fichier script utilisé.
     * @param       Encodage utilisé
     * @param       Masque appliqué.
     * @param       true si les dates doivent être affichées.
     * @param       true si les heures doivent être affichées.
     */
    PythonWriter (const TkUtil::UTF8String& fileName,
            const TkUtil::Charset& charset,
            TkUtil::Log::TYPE mask = TkUtil::Log::SCRIPTING,
            bool enableDate = true, bool enableTime = true);

    /**
     * Destructeur.
     */
    virtual ~PythonWriter ( );
	

protected:

	/**
	 * Ecrit dans le script des commentaires généraux (date de création,
	 * utilisateur, application qui a créé le script, ...).
	 */
	virtual void writeScriptComments ( );

    /** Ajoute au script un commentaire (ligne sans retour chariot)
     */
    virtual void addComment(const TkUtil::UTF8String& cmt);

    /** Ajoute au script une commande (ligne sans retour chariot)
     */
    virtual void addCommand(const TkUtil::UTF8String& cmd);


private :
    /**
        * Constructeur de copie et opérateur =. Interdits.
        */
    PythonWriter (const PythonWriter&);
    PythonWriter& operator = (const PythonWriter&);
};
/*----------------------------------------------------------------------------*/
/**
 * \return	Une chaine de caractères utf-8 représentant en 
 *          langage python la liste des entités transmise en arguments.
 */
template <class EntityType> std::string entitiesToPythonList (const std::vector<EntityType*>& entities)
{
	TkUtil::UTF8String	list (TkUtil::Charset::UTF_8);
	list << '[';
	typename std::vector<EntityType*>::const_iterator it = entities.begin ( );
	if (entities.end ( ) != it)
	{
		list << "\"" << (*it)->getName ( ) << "\"";
		it++;
	}
	for ( ; entities.end ( ) != it; it++)
		list << ",\"" << (*it)->getName ( ) << "\"";
	list << ']';

	return list.utf8 ( );
}
/*----------------------------------------------------------------------------*/
/**
 * \return	Une chaine de caractères utf-8 représentant en 
 *          langage python la liste des objets "scriptables" transmise en arguments.
 */
template <class Scriptable> std::string scriptablesToPythonList (const std::vector<Scriptable>& objects)
{
	TkUtil::UTF8String	list (TkUtil::Charset::UTF_8);
	list << '[';
	typename std::vector<Scriptable>::const_iterator it = objects.begin ( );
	if (objects.end ( ) != it)
	{
		list << (*it).getScriptCommand ( );
		it++;
	}
	for ( ; objects.end ( ) != it; it++)
		list << "," << (*it).getScriptCommand ( );
	list << ']';

	return list.utf8 ( );
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_PYTHONWRITER_H_ */
