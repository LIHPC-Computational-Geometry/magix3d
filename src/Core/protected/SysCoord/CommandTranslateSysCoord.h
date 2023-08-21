/*----------------------------------------------------------------------------*/
/*
 * \file CommandTranslateSysCoord.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6 Juin 2018
 */
/*----------------------------------------------------------------------------*/
#ifndef PROTECTED_SYSCOORD_COMMANDTRANSLATESYSCOORD_H_
#define PROTECTED_SYSCOORD_COMMANDTRANSLATESYSCOORD_H_
/*----------------------------------------------------------------------------*/
#include "SysCoord/CommandEditSysCoord.h"
#include "Utils/Vector.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {

class CommandDuplicateSysCoord;

/*----------------------------------------------------------------------------*/
class CommandTranslateSysCoord: public CommandEditSysCoord {

public:

	/*------------------------------------------------------------------------*/
	/** \brief  Constructeur
	 * \param c le contexte (permet l'accès au manager)
	 * \param syscoord le repère à modifier
	 *  \param dp le vecteur de translation
	 */
	CommandTranslateSysCoord(Internal::Context& c,
			SysCoord* syscoord, const Vector& dp);

	/** \brief  Constructeur
	 * \param c le contexte (permet l'accès au manager)
	 * \param cmd la commande de création du repère à modifier
	 *  \param dp le vecteur de translation
	 */
	CommandTranslateSysCoord(Internal::Context& c,
			CommandDuplicateSysCoord* cmd, const Vector& dp);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction du repère
     */
    virtual ~CommandTranslateSysCoord();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();


private:

    //// repère initial, copié
    SysCoord* m_syscoord;

    /// commande de création du repère
    CommandDuplicateSysCoord* m_command;

    /// vecteur translation
    Vector m_translation;
};
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* PROTECTED_SYSCOORD_COMMANDTRANSLATESYSCOORD_H_ */
/*----------------------------------------------------------------------------*/
