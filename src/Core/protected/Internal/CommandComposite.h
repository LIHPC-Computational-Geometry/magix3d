/*----------------------------------------------------------------------------*/
/*
 * \file CommandComposite.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 20 déc. 2010
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCOMPOSITE_H_
#define COMMANDCOMPOSITE_H_

#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
/** \class CommandComposite
 *  \brief Commande composite (succession de commandes)
 *
 *  Se charge de la destruction des commandes lors de sa propre destruction
 */
/*----------------------------------------------------------------------------*/
class CommandComposite : public Internal::CommandInternal {
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le nom de la commande
     */
    CommandComposite(Internal::Context& c, std::string name);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction des commandes référencées
     */
    virtual ~CommandComposite();

    /*------------------------------------------------------------------------*/
    /** Ajoute une commande à la composition */
    virtual void addCommand(CommandInternal* cmd) {m_commands.push_back(cmd);}

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** Ce qui est fait après la commande suivant le cas en erreur ou non
     */
    virtual void postExecute(bool hasError);

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    void internalUndo();

    /*------------------------------------------------------------------------*/
    /** \brief  rejoue la commande
     */
    void internalRedo();


private:
    /// la liste des commandes de la composition
    std::vector<CommandInternal*> m_commands;
};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* COMMANDCOMPOSITE_H_ */
