/*----------------------------------------------------------------------------*/
/** \file UndoRedoManagerIfc.h
 *
 *  \author Franck Ledoux, Charles Pignerol
 *
 *  \date 09/02/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef UTILS_UNDOREDOMANAGER_IFC_H_
#define UTILS_UNDOREDOMANAGER_IFC_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <string>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
class CommandIfc;
/*----------------------------------------------------------------------------*/
/** \class UndoRedoManagerIfc
 *  \brief L'interface UndoRedoManagerIfc défini les services d'un gestionnaire
 *         de mécanisme d'annulation/rejeu de commandes.
 *
 */
/*----------------------------------------------------------------------------*/
class UndoRedoManagerIfc
{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur.
     */
    virtual ~UndoRedoManagerIfc();

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

    /// Suppression des commandes undones
    virtual void clearUndone();

    /*------------------------------------------------------------------------*/
    /**
	 * \return Retourne un pointeur sur la dernière commande annulable, ou 0.
     */
    virtual CommandIfc* undoableCommand ( );

    /*------------------------------------------------------------------------*/
    /**
	 * \return Retourne le nom de la dernière commande annulable, ou une chaine
	 * vide.
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


	protected :

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    UndoRedoManagerIfc();
};	// class UndoRedoManagerIfc
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* UTILS_UNDOREDOMANAGER_IFC_H_ */
/*----------------------------------------------------------------------------*/

