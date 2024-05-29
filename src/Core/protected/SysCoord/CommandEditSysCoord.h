/*----------------------------------------------------------------------------*/
/*
 * \file CommandEditSysCoord.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6 Juin22 mai 2018
 */
/*----------------------------------------------------------------------------*/
#ifndef PROTECTED_SYSCOORD_COMMANDEDITSYSCOORD_H_
#define PROTECTED_SYSCOORD_COMMANDEDITSYSCOORD_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
class CommandEditSysCoord: public Internal::CommandInternal {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param c le contexte (permet l'accès au manager)
     * \param name le nom de la commande
     */
	CommandEditSysCoord(Internal::Context& c, const std::string& name);


    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandEditSysCoord();

   /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    virtual void internalUndo();

    /*------------------------------------------------------------------------*/
    /** \brief  rejoue la commande
     */
    virtual void internalRedo();

    /// mémorise la sauvegarde des propriétés avant commande
    void setInfoProperty(SysCoord* syscoord, SysCoordProperty* save_ppty);

    /// permute les propriétés internes avec leur sauvegarde
    void permInternalsStats();

private:

    /// repère concerné par la commande
    SysCoord* m_syscoord;

    /// sauvegarde de la propriété
    SysCoordProperty* m_save_ppty;

};
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* PROTECTED_SYSCOORD_COMMANDEDITSYSCOORD_H_ */
/*----------------------------------------------------------------------------*/
