/*----------------------------------------------------------------------------*/
/*
 * \file CommandDuplicateSysCoord.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6 Juin 2018
 */
/*----------------------------------------------------------------------------*/
#ifndef PROTECTED_SYSCOORD_COMMANDDUPLICATESYSCOORD_H_
#define PROTECTED_SYSCOORD_COMMANDDUPLICATESYSCOORD_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
class CommandDuplicateSysCoord: public Internal::CommandInternal {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param c le contexte (permet l'accès au manager)
     * \param syscoord le repère à copier
     * \param groupName le groupe dans lequel est mis la copie
     */
	CommandDuplicateSysCoord(Internal::Context& c,
			SysCoord* syscoord,
			const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction du repère
     */
    virtual ~CommandDuplicateSysCoord();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    virtual void internalUndo();

    /*------------------------------------------------------------------------*/
    /** \brief  rejoue la commande
     */
    virtual void internalRedo();

    /** \brief  ajoute à un groupe un repère
     */
    virtual void addToGroup(SysCoord* rep, bool use_default_name = false);

    /// accesseur sur le repère créé
    SysCoord* getNewSysCoord() {return m_new_syscoord;}

private:

    //// repère initial, copié
    SysCoord* m_init_syscoord;

    //// repère nouveau, la copie
    SysCoord* m_new_syscoord;

    /// le nom d'un groupe dans lequel on met le repère créé
    std::string m_group_name;
};
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* PROTECTED_SYSCOORD_COMMANDDUPLICATESYSCOORD_H_ */
/*----------------------------------------------------------------------------*/
