/*----------------------------------------------------------------------------*/
/** \file CommandInternal.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 14/12/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef INTERNAL_COMMANDINTERNAL_H_
#define INTERNAL_COMMANDINTERNAL_H_

#include "Internal/Context.h"
#include "Utils/Command.h"
#include "Internal/InfoCommand.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
/**
   @brief Partie de la commande qui contient l'Internal::InfoCommand
 */
class CommandInternal : public Utils::Command
{
public:
    /*------------------------------------------------------------------------*/
    /// Accesseur sur l'InfoCommand
    virtual InfoCommand& getInfoCommand() {return m_info_command;}
    virtual const InfoCommand& getInfoCommand() const {return m_info_command;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur les warnings importants
    virtual std::string getWarningToPopup ( ) const
	{ return m_warning_to_pop_up.iso ( ); }

    /*------------------------------------------------------------------------*/
    /// Destructeur
    virtual ~CommandInternal();

    /*------------------------------------------------------------------------*/
    /** \brief  accesseur sur le contexte
     */
    virtual Internal::Context& getContext() {return m_context;}
    virtual const Internal::Context& getContext() const {return m_context;}


    /*------------------------------------------------------------------------*/
    /** Petit nettoyage dans InfoCommand pour le cas d'un redo,
     *  et appel à la fonction Command::execute()
     */
    virtual Utils::CommandIfc::status execute();

    /*------------------------------------------------------------------------*/
    /** \brief  déjoue la commande
     */
    virtual Utils::Command::status undo ( );

    /*------------------------------------------------------------------------*/
    /** \brief  rejoue la commande
     */
    virtual Utils::Command::status redo ( );

    /*------------------------------------------------------------------------*/
    /**
     * \return      Le gestionnaire de message utilisé pour afficher des
     *              informations sur le déroulement de la commande, ou 0.
     * \see         log
     */
    virtual TkUtil::LogOutputStream* getLogStream ( )
            { return getContext().getLogStream(); }

    /*------------------------------------------------------------------------*/
    /** Ce qui est fait de propre à la commande pour l'execute */
    virtual void internalExecute() =0;

    /** Ce qui est fait de propre à la commande pour annuler une commande */
    virtual void internalUndo() =0;

    /** Ce qui est fait de propre à la commande pour rejouer une commande */
    virtual void internalRedo() =0;

    /*------------------------------------------------------------------------*/
    /** Ce qui est fait avant la commande
     */
    virtual void preExecute()
    {}

    /** Ce qui est fait après la commande suivant le cas en erreur ou non
     */
    virtual void postExecute(bool hasError)
    {}

protected:

    /*------------------------------------------------------------------------*/
    /// Constructeur par défaut
    CommandInternal(Internal::Context& c, const std::string& name);

    /// Constructeur par copie (interdit)
    CommandInternal(const CommandInternal&);

    /// Opérateur de copie (interdit)
    CommandInternal& operator = (const CommandInternal&);

    /*------------------------------------------------------------------------*/
private:
    /** le contexte */
    Internal::Context& m_context;

    /** info sur les entités modifiées par la commande */
    InfoCommand m_info_command;

protected:

    /** Message dont on veut être certain que l'utilisateur ait pris connaissance
     *  mais sans conséquence sur la commande */
    TkUtil::UTF8String m_warning_to_pop_up;

    /** données permettant de revenir à l'état initial du NameManager avant le début de la commande */
    std::vector<unsigned long> m_name_manager_before;

    /** données correspondant à l'état final du NameManager à la fin de la commande */
    std::vector<unsigned long> m_name_manager_after;

};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* INTERNAL_COMMANDINTERNAL_H_ */
/*----------------------------------------------------------------------------*/
