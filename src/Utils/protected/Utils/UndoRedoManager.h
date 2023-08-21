/*----------------------------------------------------------------------------*/
/** \file UndoRedoManager.h
 *
 *  \author Franck Ledoux
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef UTILS_UNDOREDOMANAGER_H_
#define UTILS_UNDOREDOMANAGER_H_
/*----------------------------------------------------------------------------*/
#include "Utils/UndoRedoManagerIfc.h"
#include <TkUtil/ReferencedObject.h>
#include <TkUtil/LogOutputStream.h>
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
class CommandIfc;
/*----------------------------------------------------------------------------*/
/** \class UndoRedoManager
 *  \brief La classe UndoRedoManager stocke l'ensemble des commandes effectuées
 *         et défaites lors d'une session de Magix. Elle permet aussi de
 *         défaire des opérations effectuées et de rejouer celles défaites.
 *         Elle implémente à ces effets l'interface UndoRedoManagerIfc.
 *
 */
/*----------------------------------------------------------------------------*/
class UndoRedoManager :
		public Mgx3D::Utils::UndoRedoManagerIfc, public TkUtil::ReferencedObject
{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
	 * \param	Nom unique de l'instance
     */
    UndoRedoManager(const std::string& name);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur.
     */
    virtual ~UndoRedoManager();

	/*------------------------------------------------------------------------*/
	/** Réinitialisation     */
	virtual void clear();

	/*------------------------------------------------------------------------*/
	/**
	 * \return	Nom unique de l'instance (utile en mode client/serveur).
	 */
	virtual const std::string& getName ( ) const;

    /*------------------------------------------------------------------------*/
    /** \brief  stockage d'une nouvelle commande.
	 *			Avertit ses éventuels observateurs.
     *
     *  \param  command Une commande Magix	
	 *	\see	has
     */
    virtual void store(CommandIfc* command);

	/**
	 * \return		La liste des commandes effectuées
	 */
	virtual const std::vector<CommandIfc*>& getDoneCommands ( ) const;

	/**
	 * \return		La liste des commandes annulées
	 */
	virtual const std::vector<CommandIfc*>& getUndoneCommands ( ) const;


	/** Suppression des commandes undones
	 */
	virtual void clearUndone();

    /*------------------------------------------------------------------------*/
    /**
	 * \return Retourne un pointeur sur la dernière commande annulable, ou 0.
     */
    virtual CommandIfc* undoableCommand ( );

    /*------------------------------------------------------------------------*/
    /**
	 * \return Le nom de la dernière commande annulable, ou une chaine vide.
     */
    virtual std::string undoableCommandName ( ) const;

    /*------------------------------------------------------------------------*/
    /** \brief  déseffectue la dernière commande effectue et retourne un
     *          pointeur dessus
	 *			Avertit ses éventuels observateurs.
     */
    virtual CommandIfc* undo();

    /*------------------------------------------------------------------------*/
    /**
	 * \return Retourne un pointeur sur la dernière commande rejouable, ou 0.
     */
    virtual CommandIfc* redoableCommand ( );

    /*------------------------------------------------------------------------*/
    /**
	 * \return Le nom de la dernière commande rejouable, ou une chaine vide.
     */
    virtual std::string redoableCommandName ( ) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Rejoue la dernière commande annulée et retourne un
     *          pointeur dessus
	 *			Avertit ses éventuels observateurs.
     */
    virtual CommandIfc* redo();

	/**
	 * Méthodes relatives à l'affichage d'informations relatives à l'instance
	 * dans des flux.
	 */
	//@{
	/**
	 * Le flux recevant les demandes d'affichage. Ce flux est transmis aux
	 * éventuelles instances de la classe <I>Command</I> gérées.
	 */
	virtual void setLogStream (TkUtil::LogOutputStream* stream);

	/**
	 * \return      L'éventuel afficheur utilisé.
	 */
	virtual const TkUtil::LogOutputStream* getLogStream ( ) const
	{ return m_logStream; }
	virtual TkUtil::LogOutputStream* getLogStream ( )
	{ return m_logStream; }
	//@}


	protected :

	/**
	 * \return	Un mutex pour protéger les listes de commandes contre des
	 *			accès concurrents.
	 */
	virtual TkUtil::Mutex* mutex ( ) const
	{ return m_mutex; }

	/**
	 * \return	<I>true</I> si la commande est déjà recensée, <I>false</I>
	 *			dans le cas contraire.
	 */
	virtual bool hasCommand (CommandIfc*);

	/**
	 * Envoit le <I>log</I> transmis en argument dans le flux de messages
	 * associé a l'instance.
	 * \see		TkUtil::Log
	 */
	virtual void log (const TkUtil::Log& log);


private:

	/** Nom unique de l'instance. */
	std::string							m_name;
    /** operation effectuée (DONE)  */
    std::vector<CommandIfc*>			m_done;
    /** operation défaites (UNDONE) */
    std::vector<CommandIfc*>			m_undone;
	/** Le mutex pour protéger les listes de commandes. */
	mutable TkUtil::Mutex*				m_mutex;
	/** L'éventuel afficheur. */
	TkUtil::LogOutputStream*			m_logStream;

};
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* UTILS_UNDOREDOMANAGER_H_ */
/*----------------------------------------------------------------------------*/

