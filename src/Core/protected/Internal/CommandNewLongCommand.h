/*----------------------------------------------------------------------------*/
/*
 * \file CommandNewLongCommand.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 20 déc. 2010
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWLONGCOMMAND_H_
#define COMMANDNEWLONGCOMMAND_H_

#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
/** \class CommandNewLongCommand
 *  \brief Commande longue (pour test d'intéruption)
 *
 */
/*----------------------------------------------------------------------------*/
class CommandNewLongCommand : public Internal::CommandInternal {
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le nom de la commande
     */
    CommandNewLongCommand(Internal::Context& c, std::string name);

    /*------------------------------------------------------------------------*/
    /**
     * la commande peut être interrompue par appel
     * de la méthode <I>cancel</I>
     */
    virtual bool cancelable ( ) const
        { return true; }

    /*------------------------------------------------------------------------*/
    /**
     * la commande a vocation à être lancée dans un autre <I>thread</I>.
     */
    virtual bool threadable ( ) const
        { return true; }

    /*------------------------------------------------------------------------*/
    /**
     * Le temps estimé pour exécuter une commande.
     */
    virtual unsigned long getEstimatedDuration (PLAY_TYPE playType = DO)
        { return (unsigned long)20; }

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction des commandes référencées
     */
    virtual ~CommandNewLongCommand();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    void internalUndo();

    /*------------------------------------------------------------------------*/
    /** \brief  rejoue la commande
     */
    void internalRedo();


private:
};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* COMMANDNEWLONGCOMMAND_H_ */
